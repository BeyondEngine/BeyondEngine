#pragma once

#include <sqlite3.h>
#include <string>
#include "proto/battle.pb.h"

// one battle log
class ScopedBattleLog
{
public:
    explicit ScopedBattleLog(const std::string& filename, bool dump);
    ~ScopedBattleLog();

    // battle AI request message
    void SetRequest(proto::CombatPlayback* msg);

    // battle AI result message
    void SetResult(proto::CombatCasualtyInfo* msg);

    // HTTP status code
    void SetStatusCode(int code) { status_ = code; };

private:
    void OpenDB(const std::string& filename);
    void CloseDB();

    // save to sqlite3 db
    void SaveDB();

private:
    sqlite3*                    db_ = nullptr;
    int                         rc_ = SQLITE_OK;
    int                         status_ = 0;
    bool                        dump_ = false;
    proto::CombatPlayback*      request_ = nullptr;
    proto::CombatCasualtyInfo*  result_ = nullptr;
};

// Log battle request and response
class BattleLogger
{
public:
    BattleLogger();
    ~BattleLogger();

    // abort if failed
    void OpenDB(const std::string& filename);

    void SetDumpMessage(bool v);

    ScopedBattleLog CreateScopedLog();

private:
    void CreateTable(sqlite3* db);

private:
    bool            dump_ = false;
    std::string     filename_;
};

// current date time in millseconds
std::string GetNow();
