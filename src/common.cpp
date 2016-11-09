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
#include <OCApi.h>
#include "platform.h"

std::string Common::m_interface = OC_RSRVD_INTERFACE_DEFAULT; //"oic.if.baseline"
std::string Common::m_type = "oic.r.text.scroller";
std::string Common::m_endpoint = "/TextScrollerResURI";
std::string Common::m_propname = "value";

int Common::m_logLevel = 0;
int Common::m_period = 5;

void Common::log(char const *const message)
{
    if (Common::m_logLevel)
    {
        std::cerr << "log: " << message << std::endl;
    }
}


Logger::Logger(const char *message)
{
    mMessage = message;
    if (Common::m_logLevel)
    {
        std::cerr << "log: { " << mMessage << std::endl;
    }
}

Logger::~Logger()
{
    if (Common::m_logLevel)
    {
        std::cerr << "log: } " << mMessage << std::endl;
    }
}
