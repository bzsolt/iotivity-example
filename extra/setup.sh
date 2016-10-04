#!/bin/sh
# //******************************************************************
# //
# // Copyright 2016 Samsung <philippe.coval@osg.samsung.com>
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

set -e
set -x

usage_()
{
    cat<<EOF
Subject: IoTivity as Native Tizen application
Info: https://wiki.iotivity.org/tizen
Contact: https://wiki.tizen.org/wiki/User:Pcoval


home=$HOME tmpdir=$HOME/tmp/$PWD profile="tizen_2_3_1_wearable" arch="armv7l" ./extra/setup.sh 

EOF
}


die_()
{
    local r=$?
    echo "error: $@"
    echo "exit: $r"
    return $r
}


setup_()
{
    which dirname \
        && which realpath \
        && which git \
        && which gbs \
        && which make \
        && return 0

    which apt-get \
        || die_ "TODO: port to non debian systen"

    sudo apt-get install \
        curl \
        git \
        make \
        realpath \
        wget \
        || die_ "TODO: missing tools"

    sudo apt-get install gbs \
        || die_ "TODO: missing tools"
}


build_()
{
    local config_all=true
    local package="tizen-helper"
    local url="https://notabug.org/tizen/${package}"
    local branch="master"

    ls "$package" || \
        $git clone --depth 1 -b "${branch}" "${url}"

    local make="make -f ${PWD}/tizen-helper/Makefile profile.${profile}_${arch}"

    echo "# check toolchain"
    make -C "${package}" deploy

    if ${config_all} ; then
        make -C "${package}" "profile.${profile}_${arch}"
    fi

    for t in $deps; do
        if [ "$t" = "scons" ] ; then

            package="boost-jam"
            branch="tizen"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
            
            package="dos2unix"
            branch="tizen"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"

            package="boost"
            branch="tizen"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"

            # Scons is not available on Tizen:2.3.1:Wearable (and earlier) yet
            package="scons"
            branch="tizen"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done

    for t in $deps; do
        if [ "$t" = "iotivity" ] ; then

            package="iotivity"
            url="https://git.tizen.org/cgit/contrib/${package}"
            branch="tizen"
            #TODO
            url="http://github.com/tizenteam/${package}"
            branch="sandbox/pcoval/on/latest/tizen"
            ls "$package" || \
                $git clone --depth 1 -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done

    for t in $deps; do
        if [ "$t" = "mraa" ] ; then

            # Only for example
            package="mraa"
            branch="sandbox/pcoval/tizen"
            url="https://git.tizen.org/cgit/contrib/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done

    for t in $deps; do
        if [ "$t" = "iotivity-example" ] ; then

            # Checking
            package="iotivity-example"
            url="https://notabug.org/tizen/${package}"
            branch="tizen"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done
}


deploy_()
{
    ls .tproject || die_ "TODO"

    rm -rf usr lib
    mkdir -p usr lib

    unp ${rpmdir}/iotivity-${version}-*.${arch}.rpm
    unp ${rpmdir}/iotivity-devel-${version}-*${arch}.rpm

    ln -fs ${rootfs}/usr/include/boost usr/include/
    # TODO
    cp -av ${rootfs}/usr/lib/libuuid.so.1.3.0 usr/lib/libuuid1.so  ||: #TODO might not be needed
    cp -av ${rootfs}/usr/lib/libconnectivity_abstraction.so  usr/lib/ ||: #TODO might not be needed
    rm -rf lib
    ln -fs usr/lib lib
}


main_()
{
    package="iotivity-example"

    thisdir=$(dirname -- "$0")
    thisdir=$(realpath -- "$thisdir/..")

    [ "" != "${profile}" ] || profile="tizen_2_4_mobile"

    [ "" != "${tmpdir}" ] \
        || tmpdir="${PWD}/tmp"

    [ "" != "${home}" ] \
        || home="${tmpdir}/out/${package}/${version}/${profile}"

    [ "" != "${version}" ] \
        || version="1.2.0+RC3"
    #  version="1.1.1"

    [ "" != "${arch}" ] || arch="armv7l"
    git="git"

    thisdir=$(dirname -- "$0")
    thisdir=$(realpath -- "$thisdir/..")
    HOME="${home}"

    export HOME

    gbsdir="${HOME}/tmp/gbs/tmp-GBS-${profile}_${arch}/"
    rootfs="${gbsdir}/local/BUILD-ROOTS/scratch.${arch}.0/"
    rpmdir="${gbsdir}/local/repos/${profile}_${arch}/${arch}/RPMS/"

    mkdir -p "${HOME}"
    mkdir -p "${tmpdir}"

    case "$profile" in
        "tizen_2_3_1_wearable")
            deps="scons iotivity"
            ;;
        "tizen_2_4_mobile")
            deps="iotivity"
            ;;
        *)
            deps="scons iotivity iotivity-example"
            ;;
    esac

    usage_

    # checking tools
    setup_

    ls ${rpmdir}/iotivity-devel-${version}-*${arch}.rpm \
        || build_ "${profile}" ;

    cd "${thisdir}"
    deploy_
}


main_ "$@"
