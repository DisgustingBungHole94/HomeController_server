#include "mongodb_pool.h"

#include <homecontroller/exception/exception.h>

#include <mongocxx/uri.hpp>
#include <mongocxx/exception/exception.hpp>

MongoDBClient::MongoDBClient(mongocxx::pool::entry entry, const std::string& dbName) : 
    m_entry(std::move(entry))
{
    m_database = m_entry->database(dbName);
}

MongoDBClient::~MongoDBClient() {}

MongoDBPool::MongoDBPool() 
    : m_logger("MongoDBPool")
{}

MongoDBPool::~MongoDBPool() {}

void MongoDBPool::init(const std::string& host, int port, const std::string& dbName) {
    m_instance = std::make_unique<mongocxx::instance>();

    mongocxx::uri uri{"mongodb://" + host + ":" + std::to_string(port) + "/?maxPoolSize=100"};
    m_pool = std::make_unique<mongocxx::pool>(uri);

    m_dbName = dbName;

    m_logger.log("MongoDB driver started! (" + uri.to_string() + ")");
}

void MongoDBPool::stop() {
    m_pool.reset();
    m_instance.reset();

    m_logger.log("MongoDB driver stopped.");
}

std::unique_ptr<MongoDBClient> MongoDBPool::acquire() {
    auto entry = m_pool->acquire();
    std::unique_ptr<MongoDBClient> client = std::make_unique<MongoDBClient>(std::move(entry), m_dbName);

    return std::move(client);
}