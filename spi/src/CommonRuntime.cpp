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
#include "CommonRuntime.hpp"

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

std::vector<std::string> CommonRuntime::GetContextNames()
{
    return Make()->get_client_names();
}

void CommonRuntime::AddContext(const char* contextName)
{
    return Make()->add_client(contextName);
}

bool CommonRuntime::HasContext(const char* contextName)
{
    return Make()->has_client(contextName);
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

SPI_END_NAMESPACE

