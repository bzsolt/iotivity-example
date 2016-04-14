// -*-c++-*-
//******************************************************************
//
// Copyright 2014 Intel Corporation.
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
#include <cstdio>
#include <iostream>

#include "config.h"
#include "observer.h"

using namespace std;
using namespace OC;

IoTObserver* IoTObserver::mInstance = nullptr;
const OC::ObserveType IoTObserver::OBSERVE_TYPE_TO_USE = OC::ObserveType::Observe;


IoTObserver::IoTObserver()
{
	Config::log(__PRETTY_FUNCTION__);
  initializePlatform();
}

IoTObserver* IoTObserver::getInstance()
{
	Config::log(__PRETTY_FUNCTION__);
	if ( IoTObserver::mInstance == 0 ) {
	 mInstance = new IoTObserver;
	}
	return mInstance;
}


IoTObserver::~IoTObserver()
{
	Config::log(__PRETTY_FUNCTION__);
}

void IoTObserver::initializePlatform()
{
	//Config::log(__PRETTY_FUNCTION__);

  m_platformConfig = make_shared<PlatformConfig>(ServiceType::InProc, ModeType::Client, "0.0.0.0",
						 0, OC::QualityOfService::HighQos);
  OCPlatform::Configure(*m_platformConfig);
  m_resourceDiscoveryCallback = bind(&IoTObserver::discoveredResource, this, placeholders::_1);
}

void IoTObserver::findResource()
{
	//Config::log(__PRETTY_FUNCTION__);

	string coap_multicast_discovery(OC_RSRVD_WELL_KNOWN_URI "?if=" );
  coap_multicast_discovery += Config::m_interface;
  OCConnectivityType connectivityType(CT_ADAPTER_IP);
  OCPlatform::findResource("", coap_multicast_discovery.c_str(),
			   connectivityType,
			   m_resourceDiscoveryCallback,
			   OC::QualityOfService::LowQos);
}

void IoTObserver::discoveredResource(shared_ptr<OCResource> resource)
{
	//Config::log(__PRETTY_FUNCTION__);

  try
    {
      if (resource)
        {
	  string resourceUri = resource->uri();
	  string hostAddress = resource->host();

	  cout << "\nFound Resource" << endl << "Resource Types:" << endl;
	  for (auto & resourceTypes : resource->getResourceTypes())
            {
	      cout << "\t" << resourceTypes << endl;
            }

	  cout << "Resource Interfaces: " << endl;
	  for (auto & resourceInterfaces : resource->getResourceInterfaces())
            {
	      cout << "\t" << resourceInterfaces << endl;
            }
	  cout << "Resource uri: " << resourceUri << endl;
	  cout << "host: " << hostAddress << endl;

	  if (resourceUri == Config::m_endpoint)
            {
	      QueryParamsMap test;
	      resource->observe(OBSERVE_TYPE_TO_USE, test, &IoTObserver::onObserve);
 
            }
        }
    }
  catch (OCException &ex)
    {
      cerr << "Caught exception in discoveredResource: " << ex.reason() << endl;
    }
}



void IoTObserver::onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep,
			    const int& eCode, const int& sequenceNumber)
{
	//Config::log(__PRETTY_FUNCTION__);

  try
    {
      if(eCode == OC_STACK_OK && sequenceNumber != OC_OBSERVE_NO_OPTION)
        {
	  if(sequenceNumber == OC_OBSERVE_REGISTER)
            {
	      std::cout << "Observe registration action is successful" << std::endl;
            }
	  else if(sequenceNumber == OC_OBSERVE_DEREGISTER)
            {
	      std::cout << "Observe De-registration action is successful" << std::endl;
            }

	  std::cout << "OBSERVE RESULT:"<<std::endl;
	  std::cout << "\tSequenceNumber: "<< sequenceNumber << std::endl;
	  string value = "42";

	  if ( rep.hasAttribute(Config::m_key) ) {
		  value = rep.getValueToString( Config::m_key );
	  }

	  std::cout << "value="<< value <<std::endl;
	  char line[1024];
	  snprintf(line,1024,"### [%d], %s=%s\n\n",
			  rep.numberOfAttributes(),
			  Config::m_key.c_str(),
			  value.c_str());

	  Config::log(line);

        }
      else
        {
	  if(sequenceNumber == OC_OBSERVE_NO_OPTION)
            {
	      std::cout << "Observe registration or de-registration action is failed" << std::endl;
            }
	  else
            {
	      std::cout << "onObserve Response error: " << eCode << std::endl;
	      std::exit(-1);
            }
        }
    }
  catch(std::exception& e)
    {
      std::cout << "Exception: " << e.what() << " in onObserve" << std::endl;
    }

}
#if 1
#define main observer_main
#endif

int main(int argc, char *argv[])
{
  IoTObserver observer;
  cout << "Performing Discovery..." << endl;
  observer.findResource();
  int choice;
  do
    {
      cin >> choice;
      switch (choice)
        {
	case 9:
	default:
	  return 0;
        }
    }
  while (choice != 9);
  return 0;
}
