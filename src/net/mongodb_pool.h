#pragma once

#include "../util/logger.h"

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

#include <memory>

class MongoDBClient {
    public:
        MongoDBClient(mongocxx::pool::entry entry, const std::string& dbName);
        ~MongoDBClient();

        mongocxx::database* getDatabase() { return &m_database; }

    private:
        mongocxx::pool::entry m_entry;
        mongocxx::database m_database;
};

class MongoDBPool {
    public:
        MongoDBPool();
        ~MongoDBPool();

        void init(const std::string& host, int port, const std::string& dbName);

        void stop();

        std::unique_ptr<MongoDBClient> acquire();

    private:
        Logger m_logger;

        std::unique_ptr<mongocxx::instance> m_instance;
        std::unique_ptr<mongocxx::pool> m_pool;

        std::string m_dbName;
};