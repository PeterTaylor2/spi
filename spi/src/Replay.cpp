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
/*
***************************************************************************
* Replay.cpp
***************************************************************************
* Functions and classes for reading a replay log and playing it back.
***************************************************************************
*/

#include "Replay.hpp"

#include "MapReader.hpp"
#include "Function.hpp"
#include "Map.hpp"
#include "Service.hpp"
#include "RuntimeError.hpp"
#include "ObjectMap.hpp"
#include "CommonRuntime.hpp"

#include <spi_util/CommandLine.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi_util/StreamUtil.hpp>
#include <spi_util/ClockUtil.hpp>

#include <iostream>

SPI_BEGIN_NAMESPACE

void mainReplay(int argc, char* argv[])
{
    const char* usage = "Usage: %s [-w] [-v] [-t] [-rN] infile [outfile]";

    std::string infilename;
    std::string outfilename;

    bool waitAtStart = false;
    bool timings = false;
    bool verbose = false;
    int  repeats = 1;

    spi_util::CommandLine commandLine(argc, argv, "vwr=t");
    std::string opt;
    std::string val;
    while (commandLine.getOption(opt,val))
    {
        if (opt == "-w")
        {
            waitAtStart = true;
            continue;
        }
        if (opt == "-t")
        {
            timings = true;
            continue;
        }
        if (opt == "-r")
        {
            repeats = spi_util::StringToInt(val);
            continue;
        }
        if (opt == "-v")
        {
            verbose = true;
            continue;
        }
        throw RuntimeError(usage, argv[0]);
    }

    size_t numArgs = commandLine.args.size();

    switch (numArgs)
    {
    case 2:
        outfilename = commandLine.args[1];
        infilename  = commandLine.args[0];
        break;
    case 1:
        outfilename = "";
        infilename  = commandLine.args[0];
        break;
    default:
        throw RuntimeError(usage, argv[0]);
    }

    if (waitAtStart)
    {
        char buf[128];
        std::cout << "Enter to continue:";
        std::cin >> buf;
    }

    if (verbose)
        std::cout << "Reading " << infilename << std::endl;

    ReplayLogSP replayLog = ReplayLog::Read(infilename, verbose);

    // create a common service
    //
    // this has access to all object types and allows us to replay functions
    // from more than one service
    //
    // note that functor names contain the service namespace but that class
    // names do not - this allows classes to be created in one service and
    // consumed in another which also knows that class - the same logic does
    // not apply to functions
    //bool isLogging = false;
    //int logLevel = 0;
    ServiceSP svc = spi::Service::CommonService();

    replayLog->execute(svc, outfilename, verbose, timings);
}


namespace {

    // we need the following functions

    // keep track of objects created in output
    // keep track of objects seen for the first time in input
    void findObjectActions(
        const MapConstSP& aMap,
        std::vector<ReplayActionSP>& actions);

    void addObjectAction(
        const MapConstSP& aMap,
        std::vector<ReplayActionSP>& actions)
    {
        findObjectActions(aMap, actions);

        ReplayActionSP action(new ReplayObjectAction(aMap));
        actions.push_back(action);
    }

    // returns the map reference taking account of NULL pointers
    int getMapRef(const MapConstSP& m)
    {
        if (!m)
            return 0;

        return m->GetRef();
    }

    void findObjectActionsFromValue(
        const Value& value,
        std::vector<ReplayActionSP>& actions)
    {
        switch(value.getType())
        {
        case Value::MAP:
        {
            const MapConstSP& subMap = value.getMap();
            // it is only useful to examine this map if it has a reference
            // otherwise the actions added won't do anything
            if (getMapRef(subMap))
                addObjectAction(subMap, actions);
            break;
        }
        case Value::ARRAY:
        {
            const IArrayConstSP& anArray = value.getArray();
            size_t arraySize = anArray->size();
            for (size_t j = 0; j < arraySize; ++j)
            {
                const Value& item = anArray->getItem(j);
                findObjectActionsFromValue(item, actions);
            }
            break;
        }
        case Value::OBJECT:
            throw RuntimeError("Unexpected value type %s",
                Value::TypeToString(value.getType()));
        default: break;
        }
    }

    void findObjectActions(
        const MapConstSP& aMap,
        std::vector<ReplayActionSP>& actions)
    {
        const std::vector<std::string> names = aMap->FieldNames();
        size_t nbNames = names.size();
        for (size_t i = 0; i < nbNames; ++i)
        {
            const std::string& name = names[i];
            const Value& value = aMap->GetValue(name);
            findObjectActionsFromValue(value, actions);
        }
    }

    MapConstSP convertMapToRef(const MapConstSP& in)
    {
        MapSP out = in->Copy();

        const std::vector<std::string>& names = in->FieldNames();
        for (size_t i = 0; i < names.size(); ++i)
        {
            const std::string& name = names[i];
            const Value& value      = out->GetValue(name);

            switch(value.getType())
            {
            case Value::MAP:
            {
                const MapConstSP& item = value.getMap();
                int ref = getMapRef(item);

                if (ref != 0)
                {
                    out->SetValue(name, Value(ObjectRef(ref)));
                }
                break;
            }
            case Value::ARRAY:
            {
                const IArrayConstSP& items = value.getArray();
                // first of all loop through to see if we need to convert
                bool convert = false;
                size_t nbItems = items->size();
                for (size_t j = 0; j < nbItems; ++j)
                {
                    const Value& item = items->getItem(j);
                    if (item.getType() == Value::MAP &&
                        getMapRef(item.getMap()) != 0)
                    {
                        convert = true;
                        break;
                    }
                }

                if (convert)
                {
                    ValueArraySP newArray(new ValueArray(nbItems));
                    for (size_t j = 0; j < nbItems; ++j)
                    {
                        const Value& item = items->getItem(j);
                        if (item.getType() == Value::MAP)
                        {
                            int ref = getMapRef(item.getMap());
                            if (ref != 0)
                            {
                                newArray->assign(j, Value(ObjectRef(ref)));
                                continue;
                            }
                        }
                        newArray->assign(j, item);
                    }
                    out->SetValue(name, Value(IArrayConstSP(newArray)));
                }
                break;
            }
            default:
                break;
            }
        }
        return out;
    }

    void addOutputToCache(const Value& newValue,
                          const Value& oldValue,
                          const ObjectRefCacheSP& cache)
    {
        Value::Type newType = newValue.getType();
        Value::Type oldType = oldValue.getType();

        // if the values are both arrays, then get equivalent entries
        // and add these to the cache instead

        if (newType == Value::ARRAY &&
            oldType == Value::ARRAY)
        {
            IArrayConstSP oldArray = oldValue.getArray();
            IArrayConstSP newArray = newValue.getArray();
            size_t size = std::min(oldArray->size(), newArray->size());
            for (size_t i = 0; i < size; ++i)
            {
                addOutputToCache(
                    newArray->getItem(i), oldArray->getItem(i), cache);
            }
            return;
        }

        // we can only add to the cache (having dealt with the possibility
        // of arrays above) if the old type is a MAP and the new type is an
        // OBJECT (const or not)

        if (oldType != Value::MAP)
            return;

        if (newType != Value::OBJECT)
            return;

        ObjectConstSP newObject = newValue.getObject();
        if (!newObject)
            return;

        MapConstSP oldMap = oldValue.getMap();
        if (!oldMap)
            return;

        int ref = oldMap->GetRef();
        if (ref != 0)
            cache->add_object(ref, newObject);

        MapConstSP newMap = newObject->get_attributes();
        const std::vector<std::string>& fieldNames = oldMap->FieldNames();
        for (size_t i = 0; i < fieldNames.size(); ++i)
        {
            const std::string& name  = fieldNames[i];
            addOutputToCache(
                newMap->GetValue(name), oldMap->GetValue(name), cache);
        }
    }

} // end of anonymous namespace

/*
***************************************************************************
** Implementation of ReplayFunctionAction.
***************************************************************************
*/
ReplayFunctionAction::ReplayFunctionAction(
    const MapConstSP& inputs,
    const MapConstSP& output)
    :
    m_inputs(inputs),
    m_output(output)
{}

void ReplayFunctionAction::execute(
    const ServiceSP& svc,
    const ObjectRefCacheSP& cache)
{
    ObjectMap objMap(m_inputs);
    ObjectConstSP obj = svc->object_from_map(&objMap, cache);

    const Function* func = dynamic_cast<const Function*>(obj.get());
    if (!func)
        throw RuntimeError("not a function");

    Value output;
    try
    {
        output = func->call();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }
    catch (...)
    {
        // ignore any exceptions in the function call
        // these are allowed and will have been sent to the function log
        return;
    }

    if (!m_output)
        return;

    // m_output is typically a dictionary with one or more values
    // when there is more than one value we expect the func->call() to contain
    // an array of the same number of values
    //
    // hence in order to match types we need to convert the dictionary to
    // an array with the values in field name order
    const std::vector<std::string>& fieldNames = m_output->FieldNames();
    Value prevOutput;
    switch (fieldNames.size())
    {
    case 0:
        break;
    case 1:
        prevOutput = m_output->GetValue(fieldNames[0]);
        break;
    default:
    {
        std::vector<Value> values;
        for (size_t i = 0; i < fieldNames.size(); ++i)
        {
            values.push_back(m_output->GetValue(fieldNames[i]));
        }
        prevOutput = Value(values);
        break;
    }
    }
    addOutputToCache(output, prevOutput, cache);
}

void ReplayFunctionAction::generateCode(ReplayCodeGenerator* generator) const
{
    return generator->GenerateFunction(this);
}

std::string ReplayFunctionAction::description() const
{
    return spi_util::StringFormat("FUNCTION: %s", m_inputs->ClassName().c_str());
}

const MapConstSP & ReplayFunctionAction::Inputs() const
{
    return m_inputs;
}

const MapConstSP & ReplayFunctionAction::Output() const
{
    return m_output;
}

/*
***************************************************************************
** Implementation of ReplayObjectAction.
***************************************************************************
*/
ReplayObjectAction::ReplayObjectAction(
    const MapConstSP&  inputs)
    :
    m_inputs(inputs)
{}

void ReplayObjectAction::execute(
    const ServiceSP& svc,
    const ObjectRefCacheSP& cache)
{
    ObjectMap objMap(m_inputs);
    bool isLogging = svc->is_logging();
    if (isLogging)
        spi::IncrementLogLevel();

    ObjectConstSP obj = svc->object_from_map(&objMap, cache);

    if (isLogging)
        spi::DecrementLogLevel();

    int ref = m_inputs->GetRef();

    if (ref != 0)
        cache->add_object(ref, obj);
}

void ReplayObjectAction::generateCode(ReplayCodeGenerator* generator) const
{
    return generator->GenerateObject(this);
}

std::string ReplayObjectAction::description() const
{
    return spi_util::StringFormat("OBJECT: %s", m_inputs->ClassName().c_str());
}

const MapConstSP & ReplayObjectAction::Inputs() const
{
    return m_inputs;
}


/*
***************************************************************************
** Implementation of ReplayLog.
***************************************************************************
*/

ReplayLog::ReplayLog()
    :
    m_items(),
    m_actions()
{}

ReplayLog::ReplayLog(
    const std::vector<MapConstSP>& items)
    :
    m_items(items),
    m_actions()
{}

ReplayLog::~ReplayLog()
{}

/*
 * ReplayLog main methods.
 */

void ReplayLog::execute(const ServiceSP& svc, const std::string& logfilename, bool verbose, bool timings)
{
    findActions();

    spi_util::Clock clock;
    double totalTime = 0.0;
    double slowestTime = 0.0;
    std::string slowestAction;
    std::map<std::string, double> indexTime;

    if (!logfilename.empty())
        svc->start_logging(logfilename.c_str(), "");

    ObjectRefCacheSP cache(new ObjectRefCache());
    size_t nbActions = m_actions.size();
    for (size_t i = 0; i < nbActions; ++i)
    {
        if (verbose)
        {
            std::cout << (i + 1) << " of " << nbActions << ": "
                << m_actions[i]->description() << std::endl;
        }
        try
        {
            if (timings)
                clock.Start();
            m_actions[i]->execute(svc, cache);
            if (timings)
            {
                double thisTime = clock.Time();
                totalTime += thisTime;
                indexTime[m_actions[i]->description()] += thisTime;
                if (thisTime > slowestTime)
                {
                    slowestAction = m_actions[i]->description();
                    slowestTime = thisTime;
                }
                if (verbose)
                    std::cout << "time: " << thisTime << std::endl;
            }
        }
        catch (std::exception& e)
        {
            if (svc->is_logging())
                svc->log_error(e);

            if (verbose)
                std::cout << "ERROR: " << e.what() << std::endl;
            else
                std::cerr << "ERROR: " << m_actions[i]->description() << ": " << e.what() << std::endl;
        }
    }

    if (timings)
    {
        std::cout << "totalTime: " << totalTime << std::endl;
        for (std::map<std::string,double>::const_iterator iter = indexTime.begin(); iter != indexTime.end(); ++iter)
        {
            std::cout << spi_util::StringFormat("%-50s: %10.6f", iter->first.c_str(), iter->second) << std::endl;
        }
        std::cout << "slowest single action" << std::endl;
        std::cout << spi_util::StringFormat("%-50s: %10.6f", slowestAction.c_str(), slowestTime) << std::endl;
    }

    if (!logfilename.empty())
        svc->stop_logging();
}

void ReplayLog::generateCode(ReplayCodeGenerator* generator)
{
    findActions();

    size_t nbActions = m_actions.size();
    for (size_t i = 0; i < nbActions; ++i)
    {
        m_actions[i]->generateCode(generator);
    }
}

ReplayLogSP ReplayLog::Read(const std::string& infilename, bool verbose)
{
    std::ifstream istr(infilename.c_str(), std::ios::in);
    if (!istr)
        throw RuntimeError("Could not open input file %s", infilename.c_str());

    return ReplayLog::Read(infilename.c_str(), istr, verbose);
}

ReplayLogSP ReplayLog::Read(const char* streamName, std::istream& istr, bool verbose)
{
    ReplayLogSP output(new ReplayLog());
    ReplayLog* p = output.get();

    const std::string& contents = spi_util::StreamReadContents(istr);

    MapReader reader(streamName, contents.c_str());
    std::string token;
    MapReader::TokenType tokenType;

    bool moreTokens = reader.read(token, tokenType);
    int lastGoodLine = 0;
    int numItems = 0;
    try
    {
        while (moreTokens)
        {
            reader.returnToken(token, tokenType);

            MapSP aMap = ReadMap(&reader);
            p->m_items.push_back(aMap);

            lastGoodLine = reader.lineno();
            ++numItems;
            if (verbose)
            {
                std::cout << "Items: " << numItems << " Lines: " << lastGoodLine << std::endl;
            }
            moreTokens = reader.read(token, tokenType);
        }
    }
    catch (std::exception& e)
    {
        int line = reader.lineno();
        throw RuntimeError("Error between lines %d and %d of %s: %s",
            lastGoodLine, line, streamName, e.what());
    }
    return output;
}

const std::vector<MapConstSP>& ReplayLog::items() const
{
    return m_items;
}

const std::vector<ReplayActionSP>& ReplayLog::actions()
{
    findActions();
    return m_actions;
}

void ReplayLog::findActions()
{
    if (m_actions.size() > 0)
        return;

    size_t nbItems = m_items.size();
    size_t i;

    for (i = 0; i < nbItems; ++i)
    {
        const MapConstSP&  item      = m_items[i];
        const std::string& className = item->ClassName();
        if (className.empty())
        {
            continue;
        }
        findObjectActions(item, m_actions);
        // add an action for the functor

        MapConstSP inputs = convertMapToRef(item);
        MapConstSP output;
        if (i+1 < nbItems)
        {
            output = m_items[i+1];
            if (output->ClassName().empty())
            {
                ++i;
            }
            else
            {
                output.reset();
                // no output - possibly a warning rather an error
            }
        }
        ReplayActionSP action(new ReplayFunctionAction(inputs, output));
        m_actions.push_back(action);
    }
}

SPI_END_NAMESPACE
