#!/bin/bash

CORE_MINUS_1=$(cat /proc/cpuinfo | grep processor | tail -n 1 | awk '{print $3}')
TARGET_FOLDER=target
BIN_FOLDER=bin
if [[ $@ == *"c"* && -d $TARGET_FOLDER ]]
then
    rm -rf $TARGET_FOLDER
fi
[[ ! -d $TARGET_FOLDER ]] && mkdir $TARGET_FOLDER
[[ ! -d $BIN_FOLDER ]] && mkdir $BIN_FOLDER
cd $TARGET_FOLDER

if [[ $@ == *"d"* ]]
then
    cmake -DCMAKE_BUILD_TYPE=Debug ../src/main/
elif [[ $@ == *"r"* ]]
then
    cmake -DCMAKE_BUILD_TYPE=Release ../src/main/
fi
MAKE_VERBOSITY=""
if [[ $@ == *"v"* ]]
then
    MAKE_VERBOSITY="VERBOSE=1"
fi
make -j $CORE_MINUS_1 $MAKE_VERBOSITY
find . -type f -executable | grep -v CMakeFiles | xargs -I{} cp {} ../$BIN_FOLDER