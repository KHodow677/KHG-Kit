#!/bin/bash
if [ "$1" = "build" ] && [ "$2" = "" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
elif [ "$1" = "run" ]  && [ "$2" = "" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable
  fi
elif [ "$1" = "run" ]  && [ "$2" = "server" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable server
  fi
elif [ "$1" = "test" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./test_runner
  fi
elif [ "$1" = "build" ] && [ "$2" = "run" ] && [ "$3" = "" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./executable
elif [ "$1" = "build" ] && [ "$2" = "run" ] && [ "$3" = "server" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./executable server
elif [ "$1" = "build" ] && [ "$2" = "test" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
  ./test_runner
elif [ "$1" = "build" ] && [ "$2" = "test" ] && [ "$3" = "run" ] && [ "$4" = "" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
  ./test_runner
  ./executable
elif [ "$1" = "build" ] && [ "$2" = "test" ] && [ "$3" = "run" ] && [ "$4" = "server" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
  ./test_runner
  ./executable server
else
  echo "Command keywords: [build, run, server, test]"
fi
