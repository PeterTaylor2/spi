/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/
#ifndef SPI_COMMON_RUNTIME_HPP
#define SPI_COMMON_RUNTIME_HPP

#include "Object.hpp"

#include <fstream>
#include <vector>
#include <list>
#include <set>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(CommonRuntime);
SPI_DECLARE_RC_CLASS(Service);
SPI_DECLARE_RC_CLASS(Function);
SPI_DECLARE_RC_CLASS(ObjectWriter);
SPI_DECLARE_RC_CLASS(ObjectTextStreamer);
SPI_DECLARE_RC_CLASS(ObjectRefCache);

class Value;
class EnumInfo;
struct FunctionCaller;
struct ServiceFunctionCaller;
class InputContext;
class ValueToObject;

// increments the log level and returns the previous log level
SPI_IMPORT
int IncrementLogLevel();

SPI_IMPORT
void DecrementLogLevel();

class SPI_IMPORT CommonRuntime : public RefCounter
{
public:
    friend class Service;

    ~CommonRuntime();

    // logging methods
    bool is_logging() const;
    void start_logging(const char* filename, const char* options);
    void clear_logging_cache();
    void stop_logging();
    void log_inputs(const FunctionConstSP& func);
    void log_output(const Value& output);
    void log_outputs(const MapConstSP& outputs);
    void log_error(const std::exception& e);
    void log_message(const std::string& msg);
    void set_last_error(const std::string& error);
    const std::string& get_last_error() const;

    // information methods
    std::vector<std::string> get_service_names() const;
    std::vector<std::string> get_client_names() const;

    static CommonRuntimeSP Make();
    static std::vector<std::string> GetContextNames();
    static void AddContext(const char* contextName);
    bool HasContext(const char* contextName);

    bool use_read_cache;
    ObjectConstSP object_from_read_cache(
        const std::string& filename,
        double timestamp);

    void object_to_read_cache(
        const ObjectConstSP& obj,
        const std::string& filename,
        double timestamp);

    void clear_read_cache();

private:
    CommonRuntime();

    void add_service(Service* svc);
    void remove_service(Service* svc);
    void add_client(const char* client);
    bool has_client(const char* client) const;

    ObjectType* get_object_type(const std::string& className) const;

    std::ofstream         m_logStream;
    ObjectTextStreamerSP  m_logger;
    std::list<bool*>      m_isLoggingLocations;
    std::list<Service*>   m_allServices;
    std::string           m_lastError;
    std::set<std::string> m_clients;

    mutable ObjectType::Registry m_typeRegistry;

    typedef std::pair<ObjectConstSP, double> ReadCacheEntry;
    typedef std::map <std::string, ReadCacheEntry> ReadCache;
    ReadCache m_readCache;

    // not implemented - prevents compiler construction
    CommonRuntime(const CommonRuntime&);
    CommonRuntime& operator=(const CommonRuntime&);
};

SPI_END_NAMESPACE

#endif
