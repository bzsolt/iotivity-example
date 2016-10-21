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
#include "platform.h"
#include <iostream>
using namespace std;

/**
 * 21 : will work or ARTIK10 @J27/p12 (6th from right)
 **/
unsigned int Platform::m_gpio = 21;


Platform::~Platform()
{
    LOG();
}


void Platform::setValue(bool value)
{
    LOG();
    cout << value << endl;
    digitalPinWrite(Platform::m_gpio, value);
}


void Platform::setup(int argc, char *argv[])
{
    LOG();
    int gpio = m_gpio;
    if (argc>1 && argv[1])
    {
        gpio = atoi(argv[1]);
    }
    if (pinMode(gpio,Platform::OUTPUT))
        m_gpio = gpio;
}


void Platform::log(char const *const message)
{
}


bool Platform::pinMode(int gpio, int mode)
{
    FILE * fd;
    char fName[128];

    cerr << "gpio=" << gpio <<endl;

    if((fd = fopen("/sys/class/gpio/export", "w")) == NULL)
    {
        printf("error: unable to export gpio\n");
        return false;
    }

    fprintf(fd, "%d\n", gpio);
    fclose(fd);
    sprintf(fName, "/sys/class/gpio/gpio%d/direction", gpio);
    if((fd = fopen(fName, "w")) == NULL)
    {
        printf("error: can't open gpio direction\n");
        return false;
    }

    fprintf(fd, (mode) ? "in\n" : "out\n");
    fclose(fd);
    return true;
}


bool Platform::digitalPinWrite(int gpio, bool value)
{
    FILE * fd;
    char fName[128];

    // Exporting the gpio to be used
    if((fd = fopen("/sys/class/gpio/export", "w")) == NULL)
    {
        printf("Error: unable to export gpio\n");
        return false;
    }
    fprintf(fd, "%d\n", gpio);
    fclose(fd);
    // Setting direction of the gpio
    sprintf(fName, "/sys/class/gpio/gpio%d/value", gpio);
    if((fd = fopen(fName, "w")) == NULL)
    {
        printf("Error: can't open gpio direction\n");
        return false;
    }

    fprintf(fd, "%d\n" , value);

    fclose(fd);
    return true;
}
