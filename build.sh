#!/bin/bash
if [ "$1" = "build" ] && [ "$2" = "" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
elif [ "$1" = "run" ] && [ "$2" = "" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable
  fi
elif [ "$1" = "run" ] && [ "$2" = "hoster" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable hoster 
  fi
elif [ "$1" = "run" ] && [ "$2" = "joiner" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable joiner 
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
elif [ "$1" = "build" ] && [ "$2" = "run" ] && [ "$3" = "hoster" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./executable hoster
elif [ "$1" = "build" ] && [ "$2" = "run" ] && [ "$3" = "joiner" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./executable joiner 
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
elif [ "$1" = "build" ] && [ "$2" = "test" ] && [ "$3" = "run" ] && [ "$4" = "hoster" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
  ./test_runner
  ./executable hoster
elif [ "$1" = "build" ] && [ "$2" = "test" ] && [ "$3" = "run" ] && [ "$4" = "joiner" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
  ./test_runner
  ./executable joiner 
else
  echo "Command keywords: [build, run, test]"
fi
