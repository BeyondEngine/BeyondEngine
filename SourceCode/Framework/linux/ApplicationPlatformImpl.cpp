#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <typeinfo>
#include <exception>
#include <boost/program_options.hpp>
#include "crow.h"
#include "BinaryVersion.h"
#include "Framework/Application.h"
#include "Battle/BattleManager.h"
#include "Utility/ScopeGuard.h"
#include "Utility/Logging.h"
#include "Utility/Strings.h"
#include "Utility/SnowFlake.h"
#include "Net/Http/Uri.h"
#include "proto/battle.pb.h"
#include "BattleLogger.h"
#include "BeatsUtility/md5.h"
#include "Resource/ResourceManager.h"
#include "EnginePublic/BeyondEngineVersion.h"
#include "EnginePublic/TimeMeter.h"

#ifdef NDEBUG
#define BUILD_TYPE "release"
#else
#define BUILD_TYPE "debug"
#endif

using std::cout;
using std::endl;

namespace po = boost::program_options;

static void RunHttpService(int argc, char * argv[]);
static void handleBattleAIRequest(BattleLogger& logger, const crow::request& request, crow::response& response);
static void handleVersionInfo(const crow::request& request, crow::response& response);
static void handleStoreBattleLog(const crow::request& request, crow::response& response);


static uint64_t NextUUID()
{
    static SnowFlake generator;
    return generator.Next();
}


int CApplication::Run(int argc, char * argv[], void* pData)
{
    BEYONDENGINE_UNUSED_PARAM(pData);
#ifdef USE_GLOG
    google::InitGoogleLogging(argv[0]);
    SCOPE_EXIT{ google::ShutdownGoogleLogging(); };
#endif
    try
    {
        RunHttpService(argc, argv);
    }
    catch(std::exception& ex)
    {
        LOG(ERROR) << typeid(ex).name() << ": " << ex.what();
        return 1;
    }
    return 0;
}

void CApplication::OnSwitchToBackground()
{
    Pause();
}

void CApplication::OnSwitchToForeground()
{
    Resume();
}


void RunHttpService(int argc, char * argv[])
{
    std::string url = "http://127.0.0.1:10086/battleai";
    int loglevel = 1; // Warning level
    bool dump = true; // dump battle proto message
    std::string savedb = "battle.db";

    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "produce help message")
        ("version", "show current version")
        ("dump", po::value<bool>(&dump)->default_value(dump), "dump battle proto message")
        ("url", po::value<std::string>(&url)->default_value(url), "service url path")
        ("loglevel", po::value<int>(&loglevel)->default_value(loglevel), "HTTP log level")
        ("savedb", po::value<std::string>(&savedb)->default_value(savedb), "battle log file name");

    po::variables_map options;
    po::store(po::parse_command_line(argc, argv, desc), options);
    po::notify(options);

    if (options.count("help"))
    {
        std::cout << desc << std::endl;
        return ;
    }
    if (options.count("version"))
    {
        std::cout << BUILD_TYPE " build, binary " << BEYONDENGINE_VER_REVISION << " res " << COMPILE_REVISION << std::endl;
        return ;
    }

    BattleLogger logger;
    if (!savedb.empty())
    {
        logger.SetDumpMessage(dump);
        logger.OpenDB(savedb);
    }

    LOG(INFO) << "Initialize battle";
    CApplication::GetInstance()->Initialize();
    CBattleManager::GetInstance()->InitBattleServer();

    // uncomment this when exit crash fixed
    //SCOPE_EXIT{ CApplication::GetInstance()->Uninitialize(); }; // defered

    LOG(INFO) << "Initialize webapp";
    crow::SimpleApp app;
    crow::logger::setLogLevel((crow::LogLevel)loglevel);

    app.route_dynamic("/battleai").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& request, crow::response& response){
        SCOPE_EXIT{ response.end(); }; // defered
        try
        {
            handleBattleAIRequest(logger, request, response);
        }
        catch(std::exception& ex)
        {
            response.code = 500; // HTTP Internal Server Error
            LOG(ERROR) << typeid(ex).name() << ": " << ex.what();
        }
    });

    app.route_dynamic("/verinfo")
    ([=](const crow::request& request, crow::response& response) {
        SCOPE_EXIT{ response.end(); }; // defered
        handleVersionInfo(request, response);
    });

    app.route_dynamic("/storelog").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& request, crow::response& response){
        SCOPE_EXIT{ response.end(); }; // defered
        try
        {
            handleStoreBattleLog(request, response);
        }
        catch(std::exception& ex)
        {
            response.code = 500; // HTTP Internal Server Error
            LOG(ERROR) << typeid(ex).name() << ": " << ex.what();
        }
    });

    cout << CTimeMeter::GetCurrentClockTimeString() << ", API URL: " << url << endl;

    // route url path
    net::Uri uri(url);
    app.port(uri.port());
    app.bindaddr(uri.host());
    app.run(); // single thread

    LOG(INFO) << "application exit.";
}

static bool GetResourceInfo(std::string* checksum, std::string* mtime)
{
    std::string filepath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    filepath.append(_T("/")).append(BINARIZE_FILE_NAME);
    FILE* fp = fopen(filepath.c_str(), "r");
    if (fp != nullptr)
    {
        CMD5 md5;
        md5.Update(fp);
        *checksum = md5.ToString();
        fclose(fp);
    }
    else
    {
        LOG(ERROR) << "fopen: " << filepath;
        return false;
    }

    struct stat info = {};
    int r = stat(filepath.c_str(), &info);
    if (r == 0)
    {
        *mtime = to<std::string>(info.st_mtime); //time of last modification
    }
    else
    {
        LOG(ERROR) << "stat: " << filepath;
        return false;
    }
    return true;
}


void handleVersionInfo(const crow::request& request, crow::response& response)
{
    static __thread std::string checksum;
    static __thread std::string mtime;
    if (checksum.empty() && mtime.empty())
    {
        GetResourceInfo(&checksum, &mtime);
    }

    response.write(to<std::string>(BEYONDENGINE_VER_REVISION));
    response.write("\n");
    response.write(checksum);
    response.write("\n");
    response.write(mtime);
}

// write battle log to file system
void handleStoreBattleLog(const crow::request& request, crow::response& response)
{
    const std::string& content = request.body;
    auto filename = to<std::string>(NextUUID(), ".txt");
    FILE* fp = fopen(filename.c_str(), "w");
    if (fp != NULL)
    {
        fwrite(content.c_str(), content.length(), 1, fp);
        fclose(fp);
    }
    else
    {
        LOG(ERROR) << "Open file [" << filename << "] failed";
    }
}

// handle battle AI request
void handleBattleAIRequest(BattleLogger& logger, const crow::request& request, crow::response& response)
{
    proto::CombatPlayback msg;
    ScopedBattleLog log = logger.CreateScopedLog();
    SCOPE_EXIT { log.SetStatusCode(response.code); };
    if (!msg.ParseFromString(request.body))
    {
        response.code = 400; // HTTP Bad Request
        return;
    }
    log.SetRequest(&msg);
    if (msg.scene().empty() || msg.buildings_size() == 0)
    {
        LOG(ERROR) << "Scene: " << msg.scene() << ", building count: " << msg.buildings_size();
        response.code = 400; // HTTP Bad Request
        return ;
    }

    proto::CombatCasualtyInfo* result = CBattleManager::GetInstance()->RunBattleServer(msg);
    if (result != nullptr)
    {
        log.SetResult(result);
        std::string binary;
        result->SerializeToString(&binary);
        response.write(binary);
        return ;
    }
    else
    {
        response.code = 503; // HTTP Service Unavailable
        LOG(ERROR) << "RunBattleServer return nullptr.\n";
    }
}

#ifdef DEVELOP_VERSION
// replace default glibc behavior
extern "C"
void __assert_fail(const char* assertion, const char* file, unsigned int line, const char* function) {

    fprintf(stderr, ("%s:%u, %s, Assertion `%s' failed.\n"), file, line, function, assertion);
    //raise(SIGTRAP);
}
#endif
