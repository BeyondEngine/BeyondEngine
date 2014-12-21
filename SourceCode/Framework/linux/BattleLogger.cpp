#include "BattleLogger.h"
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <iostream>
#include <stdexcept>
#include <google/protobuf/util/json_util.h>
#include "Utility/Strings.h"
#include "Utility/Logging.h"
#include "Utility/ScopeGuard.h"
#include "EnginePublic/TimeMeter.h"


ScopedBattleLog::ScopedBattleLog(const std::string& filename, bool dump)
: dump_(dump)
{
    OpenDB(filename);
}

ScopedBattleLog::~ScopedBattleLog()
{
    if (db_ != nullptr && rc_== SQLITE_OK)
    {
        SaveDB();
    }
    CloseDB();
}

void ScopedBattleLog::OpenDB(const std::string& filename)
{
    if (!filename.empty())
    {
        rc_ = sqlite3_open(filename.c_str(), &db_);
        LOG_IF(ERROR, rc_ != SQLITE_OK) << sqlite3_errmsg(db_);
    }
}

void ScopedBattleLog::CloseDB()
{
    if (db_ != nullptr)
    {
        sqlite3_close(db_);
        db_ = nullptr;
    }
    rc_ = SQLITE_OK;
}

void ScopedBattleLog::SetRequest(proto::CombatPlayback* msg)
{
    request_ = msg;
    if (dump_)
    {
        std::string output;
        if (msg != nullptr)
        {
            google::protobuf::util::MessageToJsonString(*msg, &output);
        }
        auto s = stringPrintf("request (%s)\n", CTimeMeter::GetCurrentClockTimeString().c_str());
        std::cout << s << output << std::endl;
    }
};

// battle AI result message
void ScopedBattleLog::SetResult(proto::CombatCasualtyInfo* msg)
{
    result_ = msg;
    if (dump_)
    {
        std::string output;
        if (msg != nullptr)
        {
            google::protobuf::util::MessageToJsonString(*msg, &output);
        }
        std::cout << "result (" << CTimeMeter::GetCurrentClockTimeString() << "):\n" << output << std::endl;
    }
};

void ScopedBattleLog::SaveDB()
{
    int64_t uuid = 0;
    std::string reqJson = "{}";
    std::string resJson = "{}";
    if (request_ != nullptr)
    {
        reqJson.clear();
        uuid = request_->uuid();
        auto status = google::protobuf::util::MessageToJsonString(*request_, &reqJson);
        if (!status.ok())
        {
            reqJson = "{}";
            LOG(ERROR) << status.ToString();
        }
    }
    if (result_ != nullptr)
    {
        resJson.clear();
        auto status = google::protobuf::util::MessageToJsonString(*result_, &resJson);
        if (!status.ok())
        {
            resJson = "{}";
            LOG(ERROR) << status.ToString();
        }
    }

    const char* format = "INSERT INTO `battle`(`time`, `pid`, `status`, `uuid`, `request`, `result`)"
         "VALUES('%s', %d, %d, %zu, '%s', '%s')";
    const std::string& now = CTimeMeter::GetCurrentClockTimeString();
    std::string stmt = stringPrintf(format, now.c_str(), getpid(), status_, uuid, reqJson.c_str(), resJson.c_str());
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, stmt.c_str(), nullptr, nullptr, &errmsg);
    if(rc != SQLITE_OK)
    {
        LOG(ERROR) << "sqlite3_exec: " << errmsg;
        sqlite3_free(errmsg);
    }
}


BattleLogger::BattleLogger()
{
}

BattleLogger::~BattleLogger()
{
}

void BattleLogger::OpenDB(const std::string& filename)
{
    filename_ = filename;
    if (!filename.empty())
    {
        sqlite3* db = nullptr;
        int rc = sqlite3_open(filename.c_str(), &db);
        CHECK(rc == SQLITE_OK) << sqlite3_errmsg(db);
        CreateTable(db);
        sqlite3_close(db);
    }
}

void BattleLogger::SetDumpMessage(bool v)
{
    dump_ = v;
}

ScopedBattleLog BattleLogger::CreateScopedLog()
{
    return ScopedBattleLog(filename_, dump_);
}

void BattleLogger::CreateTable(sqlite3* db)
{
    const char* stmt = ""
    "CREATE TABLE IF NOT EXISTS `battle`("
        "`status` INTEGER NOT NULL,"
        "`pid` INTEGER NOT NULL,"
        "`time` DATETIME NOT NULL,"
        "`uuid` INTEGER NOT NULL,"
        "`request` TEXT NOT NULL,"
        "`result` TEXT NOT NULL);"
    "CREATE INDEX IF NOT EXISTS `user_idx` ON `battle`(`uuid`);"
    "CREATE INDEX IF NOT EXISTS `time_idx` ON `battle`(`time`);"
    ;
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db, stmt, nullptr, nullptr, &errmsg);
    if(rc != SQLITE_OK)
    {
        SCOPE_EXIT{ sqlite3_free(errmsg); };
        LOG(FATAL) << "sqlite3_exec: " << errmsg;
    }
}



