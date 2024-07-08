#!/bin/bash
if [ "$1" = "build" ] && [ "$2" = "" ]
then
  rm -r build/res
  cp -r res build/res
  cd build
  cmake -G "MinGW Makefiles" ..
  cmake --build .
elif [ "$1" = "run" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable.exe
  fi
elif [ "$1" = "test" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./test_runner.exe
  fi
elif [ "$1" = "build" ] && [ "$2" = "run" ]
then
  rm -r build/res
  cp -r res build/res
  cd build
  cmake -G "MinGW Makefiles" ..
  cmake --build .
  ./executable.exe
elif [ "$1" = "build" ] && [ "$2" = "test" ]
then
  rm -r build/res
  cp -r res build/res
  cd build
  cmake -G "MinGW Makefiles" ..
  cmake --build .
  ./test_runner.exe
elif [ "$1" = "build" ] && [ "$2" = "test" ] && [ "$3" = "run" ]
then
  rm -r build/res
  cp -r res build/res
  cd build
  cmake -G "MinGW Makefiles" ..
  cmake --build .
  ./test_runner.exe
  ./executable.exe
else
  echo "Command usage: ./build.sh command [command: build, run, build run]"
fi
