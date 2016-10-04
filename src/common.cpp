// -*-c++-*-
//******************************************************************
//
// Copyright 2014 Intel Corporation.
// Copyright 2015 Eurogiciel <philippe.coval@eurogiciel.fr>
// Copyright 2016 Samsung <philippe.coval@osg.samsung.com>
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "common.h"
#include <iotivity/resource/OCApi.h>

std::string Common::m_interface = OC_RSRVD_INTERFACE_DEFAULT; //"oic.if.baseline" 
std::string Common::m_type = "oic.r.switch.binary";
std::string Common::m_endpoint = "/switch";
std::string Common::m_propname = "value";
std::string Common::m_link(OC::LINK_INTERFACE);

#if 1 // __TIZEN__
#include <dlog.h>
#else
#define dlog_print(type,tag,message) printf(message)
#endif


void Common::log(char const * const message)
{
//	char const * const LOGTAG="LOG";
//	dlog_print(DLOG_INFO, LOGTAG, message);
    printlog(message);
}


Logger::Logger(const char * message)
{
    mMessage = message;
    std::cerr<<"log: { " << mMessage<<std::endl;
    printlog("log: {");
    printlog(mMessage);

}

Logger::~Logger()
{
    std::cerr<<"log: } " << mMessage<<std::endl;
    printlog(mMessage);
    printlog("log: }");
}

// 	Common::log(__PRETTY_FUNCTION__);
