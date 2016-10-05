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
#include <unistd.h>
#include <ctime>
#include "client.h"
#include "platform.h"

using namespace std;
using namespace OC;


Resource::Resource(shared_ptr<OCResource> resource)
{
    LOG();
    m_resourceHandle = resource;
    m_GETCallback = bind(&Resource::onGet, this,
                         placeholders::_1, placeholders::_2, placeholders::_3);

    m_PUTCallback = bind(&Resource::onPut, this,
                         placeholders::_1, placeholders::_2, placeholders::_3);
}

Resource::~Resource()
{
}


void Resource::onGet(const HeaderOptions &headerOptions, const OCRepresentation &representation,
                     int errCode)
{
    LOG();
    if(errCode == OC_STACK_OK \
       || errCode == OC_STACK_RESOURCE_CHANGED)
    {
        bool value;
        representation.getValue(Common::m_propname, value);
        cout<<value<<endl;
    }
    else
    {
        Common::log("error:");
        Common::log( STR( __PRETTY_FUNCTION__) );
        cerr <<"errror:: in GET response from Resource resource" << endl;
    }
    IoTClient::DisplayMenu();
}

void Resource::onPut(const HeaderOptions &headerOptions, const OCRepresentation &representation,
                     int eCode)
{
    LOG();
    if (eCode == OC_STACK_OK \
        || eCode == OC_STACK_RESOURCE_CREATED \
        || eCode == OC_STACK_RESOURCE_CHANGED)
    {
        Common::log( __PRETTY_FUNCTION__ );
        bool value;
        representation.getValue(Common::m_propname, value);
        cout << value << endl;
    }
    else
    {
        Common::log( "error: ");
        Common::log( __PRETTY_FUNCTION__ );
        cerr << "error: in PUT response: " << eCode<<  endl;
    }
    IoTClient::DisplayMenu();
}

void Resource::get()
{
    LOG();
    QueryParamsMap params;
    assert(m_resourceHandle); //TODO display alert if not ready
    m_resourceHandle->get(params, m_GETCallback);
}

void Resource::put(bool value)
{
    LOG();
    QueryParamsMap params;
    OCRepresentation rep;
    rep.setValue(Common::m_propname, value);
    m_resourceHandle->put(rep, params, m_PUTCallback);
}


IoTClient::IoTClient()
{
    LOG();
    init();
}

IoTClient::~IoTClient()
{
    LOG();
}


IoTClient *IoTClient::mInstance = nullptr;

IoTClient *IoTClient::getInstance()
{
    if (! IoTClient::mInstance)
    {
        mInstance = new IoTClient;
    }
    return mInstance;
}


void IoTClient::init()
{
    LOG();

    m_platformConfig = make_shared<PlatformConfig>
                       (ServiceType::InProc, //
                        ModeType::Client, //
                        "0.0.0.0", //
                        0, //
                        OC::QualityOfService::HighQos //
                       );
    OCPlatform::Configure(*m_platformConfig);
    m_FindCallback = bind(&IoTClient::onFind, this, placeholders::_1);
}


void IoTClient::start()
{
    LOG();
    string coap_multicast_discovery = string(OC_RSRVD_WELL_KNOWN_URI);
    OCConnectivityType connectivityType(CT_ADAPTER_IP);
    OCPlatform::findResource("", //
                             coap_multicast_discovery.c_str(),
                             connectivityType,
                             m_FindCallback,
                             OC::QualityOfService::HighQos);
}

shared_ptr<Resource> IoTClient::getPlatformResource()
{
    return m_platformResource;
}


void IoTClient::onFind(shared_ptr<OCResource> resource)
{
    LOG();
    try
    {
        if (resource)
        {
            print(resource);
            string resourceUri = resource->uri();
            if (resourceUri == Common::m_endpoint)
            {
                m_platformResource = make_shared<Resource>(resource);
            }
        }
        IoTClient::DisplayMenu();
    }
    catch (OCException &ex)
    {
        Common::log( "error: ");
        Common::log(__PRETTY_FUNCTION__ );
        cerr << "error: Caught exception in discoveredResource: " << ex.reason() << endl;
    }
}


void IoTClient::print(shared_ptr<OCResource> resource)
{
    string resourceUri = resource->uri();
    string hostAddress = resource->host();

    cerr << "log: Resource uri: " << resourceUri << endl;
    cerr << "log: Resource host: " << hostAddress << endl;

    cerr << "log: Found Resource" << endl << "Resource Types:" << endl;
    for (auto & resourceTypes : resource->getResourceTypes())
    {
        cerr << "log: \t" << resourceTypes << endl;
    }

    cerr << "log: Resource Interfaces: " << endl;
    for (auto & resourceInterfaces : resource->getResourceInterfaces())
    {
        cerr << "log: \t" << resourceInterfaces << endl;
    }
}


void IoTClient::DisplayMenu()
{
    cout << "\nEnter:" << endl
         << "*) Display this menu" << endl
         << "0) Set value off" << endl
         << "1) Set value on" << endl
         << "2) Toggle value" << endl
         << "9) Quit" << endl;
}


int IoTClient::main(int argc, char *argv[])
{
    IoTClient::getInstance()->start();

    int choice;
    bool value = false;
    do
    {
        cin >> choice;
        switch (choice)
        {
            case 0:
                value = false;
                break;
            case 1:
                value = true;
                break;
            case 2:
                value = !value;
                break;
            case 9:
                return 0;
            default:
                IoTClient::DisplayMenu();
                break;
        }

        if (IoTClient::getInstance()->getPlatformResource())
            IoTClient::getInstance()->getPlatformResource()->put(value);
        else
            cout << "log: resource not yet discovered" << endl;

    }
    while (choice != 9);
    return 0;
}


#ifdef CONFIG_CLIENT_MAIN
int main(int argc, char *argv[])
{
    return IoTClient::main(argc, argv);
}
#endif
