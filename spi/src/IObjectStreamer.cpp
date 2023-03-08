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
#include "IObjectStreamer.hpp"
#include "RuntimeError.hpp"
#include "Service.hpp"

SPI_BEGIN_NAMESPACE

std::map<std::string, IObjectStreamer::RegInfo> IObjectStreamer::g_registry;
std::vector<std::string> IObjectStreamer::g_formats;

bool IObjectStreamer::uses_recognizer() const
{
    return false;
}

bool IObjectStreamer::is_binary() const
{
    return false;
}

IObjectStreamer::RegInfo IObjectStreamer::FindRegInfo(const std::string& key)
{
    std::map<std::string, RegInfo>::const_iterator iter = g_registry.find(key);

    // the default is the first registered format
    if (iter == g_registry.end() && key.length() == 0 && g_formats.size() > 0)
    {
        iter = g_registry.find(g_formats[0]);
    }

    if (iter == g_registry.end())
    {
        throw RuntimeError("'%s' is not a registered object stream format",
                           key.c_str());
    }

    return iter->second;
}

const char* IObjectStreamer::Recognizer(const std::string& key)
{
    const char* recognizer = FindRegInfo(key).recognizer;
    if (!recognizer || !*recognizer)
        throw RuntimeError("No recognizer for format '%s'", key.c_str());

    return recognizer;
}

IObjectStreamerSP IObjectStreamer::Make(
    const ServiceConstSP& service,
    const std::string& key,
    const char* options)
{
    return FindRegInfo(key).maker(service, options);
}

IObjectStreamerSP IObjectStreamer::MakeWriter(
    const std::string& key,
    const char* options)
{
    return Make(spi::ServiceConstSP(), key, options);
}

void IObjectStreamer::Register(
    const std::string& key,
    Maker* maker,
    bool isBinary,
    const char* recognizer)
{
    if (g_registry.count(key) == 0)
    {
        g_formats.push_back(key);
    }
    g_registry[key] = RegInfo(maker, isBinary, recognizer);
}

std::vector<std::string> IObjectStreamer::Formats(bool allowBinary)
{
    std::vector<std::string> formats;
    for (size_t i = 0; i < g_formats.size(); ++i)
    {
        const std::string& format = g_formats[i];
        if (allowBinary || !(g_registry[format].isBinary))
            formats.push_back(format);
    }
    return formats;
}

IObjectStreamer::RegInfo::RegInfo()
:
maker(NULL),
isBinary(),
recognizer(NULL)
{}

IObjectStreamer::RegInfo::RegInfo(
    IObjectStreamer::Maker* maker,
    bool isBinary,
    const char* recognizer)
    :
    maker(maker),
    isBinary(isBinary),
    recognizer(recognizer)
{
    if (!maker)
        throw RuntimeError("%s: Maker undefined", __FUNCTION__);

    if (!recognizer || !*recognizer)
        throw RuntimeError("%s: Recognizer undefined", __FUNCTION__);
}

SPI_END_NAMESPACE
