#!/bin/bash
set -x
d=${PWD}

if [ ! -d ${d}/build ] ; then
    mkdir ${d}/build
fi

is_debug="n"
cmake_config="-DCMAKE_BUILD_TYPE=Release"
build_type="Release"

if [ "${1}" = "debug" ] ; then
    is_debug="y"
    cmake_config="-DCMAKE_BUILD_TYPE=Debug"
    build_type="Debug"
fi

cd ${d}/build

cmake -DCMAKE_INSTALL_PREFIX=${d}/../install \
      ${cmake_config} \
      ..

cmake --build . --config ${build_type} --target install

if [ $? -ne 0 ] ; then
    exit
fi

cd ${d}/../install/bin/test

if [ "${is_debug}" = "y" ] ; then
    gdb ./dxt_creator_debug
else
    ./dxt_creator
fi
