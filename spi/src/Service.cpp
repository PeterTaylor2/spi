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
#include "Service.hpp"

#include "ObjectTextStreamer.hpp"
#include "ObjectJSONStreamer.hpp"
#include "Function.hpp"
#include "EnumHelper.hpp"
#include "ValueToObject.hpp"
#include "IObjectMap.hpp"
#include "RuntimeError.hpp"
#include "InputValues.hpp"
#include "StringUtil.hpp"
#include "Session.hpp"

#ifndef SPI_STATIC
#include "ObjectURL.hpp"
#endif

#include <spi_util/Utils.hpp>

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>
#include <spi_util/FileUtil.hpp>

#include <time.h>

SPI_BEGIN_NAMESPACE

namespace {
    int g_log_level = 0;
}

int IncrementLogLevel()
{
    // POST-INCREMENT to return previous value
    return g_log_level++;
}

void DecrementLogLevel()
{
    if (g_log_level > 0)
        --g_log_level;
    else
        g_log_level = 0;
}

CommonRuntime::CommonRuntime()
    :
    use_read_cache(false)
{
    // construction common to all services
    //
    // the order of registration is important since the first streamer
    // registered is the default when you stream an object without giving
    // the format string
    ObjectTextStreamer::Register();
    ObjectJSONStreamer::Register();
    ObjectCompressedTextStreamer::Register();
}

CommonRuntime::~CommonRuntime()
{
    if (is_logging())
        stop_logging();
}

CommonRuntimeSP CommonRuntime::Make()
{
    // the danger of this approach is that crt once created
    // is never deleted until all the services have themselves
    // been deleted, but on the other hand it doesn't point to
    // any persistent memory within the services
    //
    // there is a case for using a pointer to static instead of
    // shared pointer
    static CommonRuntimeSP crt;

    if (!crt)
        crt = new CommonRuntime();

    return crt;
}

ObjectConstSP CommonRuntime::object_from_read_cache(
    const std::string & filename,
    double timestamp)
{
    ReadCache::const_iterator iter = m_readCache.find(filename);

    if (iter != m_readCache.end())
    {
        if (timestamp == iter->second.second)
            return iter->second.first;
        else
            m_readCache.erase(filename);
    }

    return ObjectConstSP();
}

void CommonRuntime::object_to_read_cache(
    const ObjectConstSP & obj,
    const std::string & filename,
    double timestamp)
{
    m_readCache[filename] = ReadCacheEntry(obj, timestamp);
    // might we do something if the cache gets too big?
}

void CommonRuntime::clear_read_cache()
{
    m_readCache.clear();
}

bool CommonRuntime::is_logging() const
{
    return !!m_logger;
}

void CommonRuntime::start_logging(const char* filename, const char* options)
{
    // we will always use maximum accuracy - hence we ignore options at present
    // and no longer pass them onto the ObjectTextStreamer constructor
    //
    // perhaps we could 'append' to an existing logfile as an option?
    stop_logging();
    m_logStream.open(filename);
    m_logStream << "# Logging starts: " << spi_util::Timestamp() << std::endl;
    m_logger = ObjectTextStreamer::Make(ServiceConstSP(), "ACC");
    for (std::list<Service*>::const_iterator iter = m_allServices.begin();
         iter != m_allServices.end(); ++iter)
    {
        Service* svc = *iter;
        svc->set_logging(true);
        const std::string& serviceNamespace = svc->get_namespace();
        if (serviceNamespace.length() > 0)
        {
            m_logStream << "# Service: " << serviceNamespace
                << " " << svc->get_version() << std::endl;
        }
    }
}

void CommonRuntime::clear_logging_cache()
{
    if (m_logger)
        m_logger->clear_cache();
}

void CommonRuntime::stop_logging()
{
    if (m_logStream.is_open())
    {
        m_logStream << "# Logging ends: " << spi_util::Timestamp() << std::endl;
        m_logStream.close();
    }
    m_logger.reset();
    for (std::list<Service*>::const_iterator iter = m_allServices.begin();
         iter != m_allServices.end(); ++iter)
    {
        Service* svc = *iter;
        svc->set_logging(false);
    }
    g_log_level = 0;
}

void CommonRuntime::log_inputs(const FunctionConstSP& func)
{
    if (m_logger)
    {
        m_logger->to_stream(m_logStream, func);
        m_logStream << std::endl;
    }
}

void CommonRuntime::log_output(const Value& output)
{
    if (m_logger)
    {
        m_logger->to_stream(m_logStream, "output", output);
        m_logStream << std::endl;
    }
}

void CommonRuntime::log_outputs(const MapConstSP& outputs)
{
    if (m_logger)
    {
        m_logger->to_stream(m_logStream, outputs);
        m_logStream << std::endl;
    }
}

void CommonRuntime::log_error(const std::exception& e)
{
    if (m_logger)
    {
        m_logger->to_stream(m_logStream, "error", Value(e));
        m_logStream << std::endl;
    }
    set_last_error(e.what());
}

void CommonRuntime::log_message(const std::string& msg)
{
    if (m_logger)
    {
        m_logStream << "# " << msg << std::endl;
    }
}

void CommonRuntime::set_last_error(const std::string& error)
{
    m_lastError = error;
}

const std::string& CommonRuntime::get_last_error() const
{
    return m_lastError;
}

std::vector<std::string> CommonRuntime::get_service_names() const
{
    std::vector<std::string> serviceNames;

    for (std::list<Service*>::const_iterator iter = m_allServices.begin();
        iter != m_allServices.end(); ++iter)
    {
        Service* svc = *iter;
        serviceNames.push_back(svc->get_name());
    }

    return serviceNames;
}

std::vector<std::string> CommonRuntime::get_client_names() const
{
    std::vector<std::string> clientNames(m_clients.begin(), m_clients.end());

    return clientNames;
}

void CommonRuntime::add_service(Service* svc)
{
    m_allServices.push_back(svc);
    m_typeRegistry.clear();
}

void CommonRuntime::remove_service(Service* svc)
{
    clear_read_cache();
    m_allServices.remove(svc);
    m_typeRegistry.clear();
}

void CommonRuntime::add_client(const char * client)
{
    if (client)
        m_clients.insert(std::string(client));
}

bool CommonRuntime::has_client(const char * client) const
{
    if (!client)
        return false;

    std::set<std::string>::const_iterator iter = m_clients.find(std::string(client));
    if (iter == m_clients.end())
        return false;

    return true;
}

ObjectType* CommonRuntime::get_object_type(const std::string& className) const
{
    // first time we ask for an object type we create the typeRegistry from
    // the registered services - note that every time we change the list of
    // services we clear the type registry
    //
    // previously we looped through the services to search for the object type
    // this was inefficient since each time we couldn't find an object we
    // threw and ignored an exception - as well as having to do multiple
    // searches instead of just the one

    if (m_typeRegistry.size() == 0)
    {
        std::list<Service*>::const_iterator iter;
        for (iter = m_allServices.begin(); iter != m_allServices.end(); ++iter)
        {
            Service* svc = *iter;
            m_typeRegistry.import_types(svc->m_typeRegistry);
        }
    }

    return m_typeRegistry.get_type(className);
}

ServiceSP Service::Make(
    const char* name,
    const char* ns,
    const char* version)
{
    return ServiceSP(new Service(name, ns, version));
}

ServiceSP Service::Make(
    const char* name,
    const ServiceSP& svc)
{
    return ServiceSP(new Service(name, svc));
}

ServiceSP Service::CommonService()
{
    // as such this will get deleted in random sequence once created
    // if we want to control the deletion we need a specific shutdown
    // routine and change commonService to be a global static
    static ServiceSP commonService;

    if (!commonService)
    {
        commonService = new Service();
    }

    return commonService;
}

Service::Service(
    const char* name,
    const char* ns,
    const char* version)
    :
    m_commonService(false),
    m_name(name),
    m_namespace(ns),
    m_version(version),
    m_typeRegistry(),
    m_functions(),
    m_enums(),
    m_connections(0),
    m_isLogging(false),
    m_commonRuntime(CommonRuntime::Make())
{
    Init();
}

Service::Service(
    const char* name,
    const ServiceSP& svc)
    :
    m_commonService(false),
    m_name(name),
    m_namespace(svc->m_namespace),
    m_version(svc->m_version),
    m_typeRegistry(),
    m_functions(svc->m_functions),
    m_enums(svc->m_enums),
    m_connections(0),
    m_isLogging(false),
    m_commonRuntime(svc->m_commonRuntime)
{
    SPI_PRE_CONDITION(svc);
    m_typeRegistry.import_types(svc->m_typeRegistry);
    Init();
}

Service::Service()
    :
    m_commonService(true),
    m_name(),
    m_namespace(),
    m_typeRegistry(),
    m_functions(),
    m_enums(),
    m_connections(0),
    m_isLogging(false),
    m_commonRuntime(CommonRuntime::Make())
{
    Init();
}

void Service::Init()
{
    m_commonRuntime->add_service(this);
}

void Service::set_logging(bool logging)
{
    m_isLogging = logging;
}

// this function is called by the start_service function for a specific service
// it is not required to call start_service and stop_service
void Service::start_up()
{
    if (m_connections == 0)
    {
        // do nothing for the moment
    }
    ++m_connections;
}

// this function is called by the stop_service function for a specific service
// it is not required to call start_service and stop_service
void Service::shut_down()
{
    if (m_connections <= 0)
        return;

    --m_connections;

    if (m_connections == 0)
    {
        m_commonRuntime->remove_service(this);
    }
}

bool Service::is_shut_down() const
{
    return m_connections <= 0;
}

Service::~Service()
{
    if (m_connections > 0)
    {
        m_connections = 1; // to force a shut-down
        shut_down();
    }
    else if (m_connections == 0)
    {
        // this means that start_up / shut_down have not been used
        // however we still need to remove the service from the common runtime
        m_commonRuntime->remove_service(this);
    }
}

void Service::add_svo(const char* name)
{
    if (name)
        m_svos.push_back(std::string(name));
}

std::vector<std::string> Service::svos() const
{
    return m_svos;
}

void Service::add_object_type(ObjectType* type)
{
    // we allow the common service types to be overwritten
    // at present only native Python types can be put into the common service
    m_typeRegistry.add_type(type, m_commonService);
    type->set_service(this);
}

ObjectType* Service::get_object_type(const char* className) const
{
    SPI_UTIL_CLOCK_FUNCTION();

    if (m_commonService)
        return m_commonRuntime->get_object_type(className);
    return m_typeRegistry.get_type(className);
}

ObjectType* Service::get_object_type(const std::string& className) const
{
    SPI_UTIL_CLOCK_FUNCTION();

    if (m_commonService)
        return m_commonRuntime->get_object_type(className);
    return m_typeRegistry.get_type(className);
}

void Service::add_function_caller(FunctionCaller* caller)
{
    std::string funcName(caller->name);
    if (m_functions.count(funcName) != 0)
        throw RuntimeError("Duplicate function name %s in service %s",
            caller->name, m_name.c_str());

    m_functions[funcName] = caller;
}

FunctionCaller* Service::get_function_caller(const char* funcName) const
{
    std::map<std::string,FunctionCaller*>::const_iterator iter =
        m_functions.find(std::string(funcName));

    if (iter == m_functions.end())
        throw RuntimeError("Unknown function name %s in service %s",
            funcName, m_name.c_str());

    return iter->second;
}

std::vector<std::string> Service::get_function_names() const
{
    std::vector<std::string> names;
    names.reserve(m_functions.size());

    std::map<std::string,FunctionCaller*>::const_iterator iter;
    for (iter = m_functions.begin(); iter != m_functions.end(); ++iter)
        names.push_back(iter->first);

    return names;
}

void Service::add_enum_info(EnumInfo* info)
{
    const std::string& enumName = info->Name();
    if (m_enums.count(enumName) != 0)
        throw RuntimeError("Duplicate enum name %s in service %s",
            enumName.c_str(), m_name.c_str());

    m_enums[enumName] = info;
}

EnumInfo* Service::get_enum_info(const std::string& enumName) const
{
    std::map<std::string,EnumInfo*>::const_iterator iter =
        m_enums.find(enumName);

    if (iter == m_enums.end())
        throw RuntimeError("Unknown enum name %s in service %s",
            enumName.c_str(), m_name.c_str());

    return iter->second;
}

std::vector<std::string> Service::get_enums() const
{
    std::vector<std::string> enums;
    for (std::map<std::string,EnumInfo*>::const_iterator iter = m_enums.begin();
         iter != m_enums.end(); ++iter)
    {
        enums.push_back(iter->first);
    }
    return enums;
}

ObjectConstSP Service::object_from_map(
    IObjectMap* aMap,
    const ObjectRefCacheSP& cache,
    const MapConstSP& metaData) const
{
    SPI_UTIL_CLOCK_FUNCTION();

    const std::string& className = aMap->ClassName();
    if (className.empty())
        throw RuntimeError("%s: Object map has no name", __FUNCTION__);
    ObjectType* type = get_object_type(className);

    ValueToObject valueToObject(share_this(this), cache);
    return type->make_from_map(aMap, valueToObject, metaData);
}

ObjectConstSP Service::object_from_data(
    const std::string& data, // can be binary or text
    const std::string& streamName,
    bool allowBinary,
    const MapConstSP& metaData) const
{
    // we will try the stream with all the streamers of the matching type
    // (binary or text)
    //
    // the first N characters of the stream must match the recognizer
    // for the registered format
    //
    // hence we fail if we don't recognize the object or when having
    // recognized the object we cannot parse the remainder of the stream

    SPI_UTIL_CLOCK_FUNCTION();

    const std::vector<std::string>& formats = IObjectStreamer::Formats(allowBinary);

    for (std::vector<std::string>::const_iterator iter = formats.begin();
         iter != formats.end(); ++iter)
    {
        const char* recognizer = IObjectStreamer::Recognizer(*iter);

        SPI_POST_CONDITION(recognizer);
        SPI_POST_CONDITION(*recognizer);

        // read the first N characters from the stream
        // if it matches the recognizer then we are good to go

        SPI_UTIL_CLOCK_EVENTS_LOG("recognizing");
        size_t rlen = strlen(recognizer);

        std::string buf = data.substr(0, rlen);

        if (buf == recognizer)
        {
            IObjectStreamerSP streamer = IObjectStreamer::Make(
                share_this(this), *iter);

            size_t offset = streamer->uses_recognizer() ? 0 : rlen;

            SPI_UTIL_CLOCK_EVENTS_LOG("streamer->from_data");
            return streamer->from_data(streamName, data, offset, metaData);
        }
    }
    throw RuntimeError("%s: Could not recognize any of the file formats: %s",
        streamName.c_str(),
        spi_util::StringJoin(",", formats).c_str());
}

ObjectConstSP Service::object_from_string(
    const std::string& objectString) const
{
    std::istringstream iss(objectString);

    // strings are always in text format so we won't try binary formats
    return object_from_data(objectString, std::string(), false);
}

ObjectConstSP Service::object_from_file(const std::string& filename) const
{
#ifndef SPI_STATIC
    if (spi_util::StringStartsWith(filename, "http://") ||
        spi_util::StringStartsWith(filename, "file://"))
    {
        return object_from_url(share_this(this), filename);
    }
#endif

#ifdef SPI_UTIL_CLOCK_EVENTS
    std::string logfilename = spi_util::path::basename(filename);
    SPI_UTIL_CLOCK_EVENTS_START();
#endif

    const std::vector<std::string> formats = IObjectStreamer::Formats(true);
    if (formats.size() == 0)
        throw RuntimeError("No registered formats for reading objects");

    // we read the whole file and close it as soon as possible
    // then we use a stringstream for the actual parsing
    //
    // we read in binary format - a text streamer should have no
    // problem with extra white space
    //
    // we will have written text files in text mode simply so that humans
    // can read the files using notepad on windows (for example)
    SPI_UTIL_CLOCK_EVENTS_LOG("readfile");

    double timestamp = spi_util::FileLastUpdateTime(filename);
    if (m_commonRuntime->use_read_cache)
    {
        ObjectConstSP obj = m_commonRuntime->object_from_read_cache(
            filename, timestamp);
        if (obj)
            return obj;
    }

    // use default method for fastest read of a file
    const std::string& data = spi_util::FileReadContents(filename.c_str());
    size_t size = data.size();

    MapSP metaData(new Map(""));
    metaData->SetValue("filename", Value(filename));
    metaData->SetValue("filetime", Value(DateTime(timestamp)));

    ObjectConstSP obj = object_from_data(data, filename, true, metaData);
    // to save memory, we need to avoid keeping huge objects in the read_cache
    // we cannot measure the amount of memory used by obj
    // our best effort is to look at the size of the serialized object
    if (m_commonRuntime->use_read_cache && size > 0 && size < 65536)
    {
        m_commonRuntime->object_to_read_cache(obj, filename, timestamp);
    }
    // note that is a NO-OP unless spi::session::start_session() has been called
    spi::session::add_file_name(filename);
    SPI_UTIL_CLOCK_EVENTS_WRITE(logfilename.c_str());
    return obj;
}

ObjectConstSP Service::object_coerce(
    const std::string& className,
    const Value& value,
    const InputContext* context)
{
    ObjectType* objectType = get_object_type(className);

    // ValueToObject performs simple coercion but not type checking
    ObjectConstSP obj = context->ValueToObject(value,objectType);

    // hence call ObjectType::coerce_from_object to complete the job
    return objectType->coerce_from_object(obj);
}

bool Service::is_logging() const
{
    return m_commonRuntime->is_logging();
}

const bool* Service::is_logging_flag() const
{
    return &m_isLogging;
}

void Service::start_logging(const char* filename, const char* options)
{
    m_commonRuntime->start_logging(filename, options);
}

void Service::clear_logging_cache()
{
    m_commonRuntime->clear_logging_cache();
}

void Service::stop_logging()
{
    m_commonRuntime->stop_logging();
}

void Service::log_inputs(const FunctionConstSP& func)
{
    m_commonRuntime->log_inputs(func);
}

void Service::log_output(const Value& output)
{
    m_commonRuntime->log_output(output);
}

void Service::log_outputs(const MapConstSP& outputs)
{
    m_commonRuntime->log_outputs(outputs);
}

void Service::log_error(const std::exception& e)
{
    m_commonRuntime->log_error(e);
}

void Service::log_message(const std::string& msg)
{
    m_commonRuntime->log_message(msg);
}

void Service::set_last_error(const std::string& error)
{
    m_commonRuntime->set_last_error(error);
}

const std::string& Service::get_last_error() const
{
    return m_commonRuntime->get_last_error();
}

std::vector<std::string> Service::get_service_names() const
{
    return common_runtime()->get_service_names();
}

std::vector<std::string> Service::get_client_names() const
{
    return common_runtime()->get_client_names();
}

void Service::implement_read_cache(bool useReadCache)
{
    m_commonRuntime->use_read_cache = useReadCache;
}

void Service::clear_read_cache()
{
    m_commonRuntime->clear_read_cache();
}

void Service::add_client(const char * client)
{
    if (m_commonRuntime)
        m_commonRuntime->add_client(client);
}

bool Service::has_client(const char * client) const
{
    if (m_commonRuntime)
        return m_commonRuntime->has_client(client);
    return false;
}

CommonRuntimeSP Service::common_runtime() const
{
    return m_commonRuntime;
}

const std::string& Service::get_name() const
{
    return m_name;
}

const std::string& Service::get_namespace() const
{
    return m_namespace;
}

const std::string& Service::get_version() const
{
    return m_version;
}

bool Service::is_common_service() const
{
    return m_commonService;
}


SPI_END_NAMESPACE

