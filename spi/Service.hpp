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
#ifndef SPI_SERVICE_HPP
#define SPI_SERVICE_HPP

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

class SPI_IMPORT Service : public RefCounter
{
public:
    friend class CommonRuntime;

    // constructs stand-alone service (or service to be used as base for
    // other services)
    static ServiceSP Make(
        const char* name,
        const char* ns,
        const char* version);
    // constructs a service using an existing service as a base
    // the namespace must be the same for all services linked in this manner
    static ServiceSP Make(
        const char* name,
        const ServiceSP& svc);
    // returns a singleton service which has no object types itself, but which
    // will have access to all the other object types
    static ServiceSP CommonService();

    // shut down operations applied to a service
    void start_up();
    void shut_down();
    bool is_shut_down() const;

    // calls shut_down if it has not already been called
    ~Service();

    // record the satellites that use this service as a base service
    void add_svo(const char* name);
    std::vector<std::string> svos() const;

    // object registry functions
    void add_object_type(ObjectType* type);
    ObjectType* get_object_type(const char* className) const;
    ObjectType* get_object_type(const std::string& className) const;
    ObjectConstSP object_from_map(
        IObjectMap* m,
        const ObjectRefCacheSP& cache,
        const MapConstSP& metaData = MapConstSP()) const;
    ObjectConstSP object_from_data(const std::string& data,
        const std::string& streamName,
        bool allowBinary,
        const MapSP& metaData = MapSP()) const;
    // convenience methods that call the fundamental object_from_data method
    ObjectConstSP object_from_string(const std::string& objectString) const;
    ObjectConstSP object_from_file(const std::string& filename) const;

    ObjectConstSP object_from_url(const std::string& url,
        int timeout = 0,
        int cacheAge = 0) const;

    ObjectConstSP object_coerce(const std::string& className,
        const Value& value,
        const InputContext* context=0);

    // function registry functions
    void add_function_caller(FunctionCaller* caller);
    FunctionCaller* get_function_caller(const char* funcName) const;
    std::vector<std::string> get_function_names() const;

    // enum registry functions
    void add_enum_info(EnumInfo* info);
    EnumInfo* get_enum_info(const std::string& enumName) const;
    std::vector<std::string> get_enums() const;

    // logging methods
    bool is_logging() const;
    const bool* is_logging_flag() const;
    //void increment_log_level();
    //void decrement_log_level();
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

    // functions for managing the cache of previously read objects
    void implement_read_cache(bool useReadCache);
    void clear_read_cache();

    // define client types, e.g. EXCEL, PYTHON, DOTNET etc
    void add_client(const char* client);
    bool has_client(const char* client) const;

    // this method is a bit of a cop-out from general principles
    CommonRuntimeSP common_runtime() const;

    // information methods
    const std::string& get_name() const;
    const std::string& get_namespace() const;
    const std::string& get_version() const;
    bool is_common_service() const;

private:
    Service(const char* name, const char* ns, const char* version);
    Service(const char* name, const ServiceSP& svc);
    Service();

    bool                   m_commonService;
    const std::string      m_name;
    const std::string      m_namespace;
    const std::string      m_version;
    ObjectType::Registry m_typeRegistry;
    std::map<std::string, FunctionCaller*> m_functions;
    std::map<std::string, EnumInfo*> m_enums;
    int                    m_connections;
    bool                   m_isLogging;
    std::vector<std::string> m_svos;

    CommonRuntimeSP        m_commonRuntime;
    void set_logging(bool logging);

    // not implemented - prevents compiler construction
    Service(const Service&);
    Service& operator=(const Service&);

    // initialisation - called by all constructors
    void Init();
};

SPI_END_NAMESPACE

#endif
