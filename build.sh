#!/bin/bash
if [ "$1" = "build" ] && [ "$2" = "" ]
then
  rm -r build/
  mkdir build
  cd build
  cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
elif [ "$1" = "run" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable
  fi
elif [ "$1" = "build" ] && [ "$2" = "run" ]
then
  rm -r build/
  mkdir build
  cd build
  cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./executable
else
  echo "Command usage: ./build.sh command [command: build, run, build run]"
fi
