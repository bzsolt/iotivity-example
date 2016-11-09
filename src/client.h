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

#ifndef CLIENT_H_
#define CLIENT_H_

#include <memory>
#include <iostream>

#include <OCApi.h>
#include <OCPlatform.h>
#include <OCResource.h>


class Resource
{
        std::shared_ptr<OC::OCResource> m_OCResource;
        OC::OCRepresentation m_Representation;
        OC::PostCallback m_POSTCallback;
        void onPost(const OC::HeaderOptions &, const OC::OCRepresentation &, int);
    public:
        void post(std::string&);
        Resource(std::shared_ptr<OC::OCResource>);
        virtual ~Resource();
};


class IoTClient
{
    public:
        static const OC::ObserveType OBSERVE_TYPE_TO_USE;

        static void onObserve(const OC::HeaderOptions /*headerOptions*/,
                              const OC::OCRepresentation &rep,
                              const int &eCode, const int &sequenceNumber);
        static void menu();

        static int main(int argc, char *argv[]);
    public:
        std::shared_ptr<Resource> getResource();
        void start();
        void print(std::shared_ptr<OC::OCResource> resource);
        bool setValue(std::string& value);
        static IoTClient *getInstance();
    private:
        IoTClient();
        virtual ~IoTClient();
        void init();
        void onFind(std::shared_ptr<OC::OCResource>);
    private:
        static IoTClient *mInstance;
        std::shared_ptr<Resource> m_Resource;
        std::shared_ptr<OC::PlatformConfig> m_platformConfig;
        OC::FindCallback m_FindCallback;
        bool m_value;
};

#endif /* CLIENT_H_ */
