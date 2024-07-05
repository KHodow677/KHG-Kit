#!/bin/bash
if [ "$1" = "build" ] && [ "$2" = "" ]
then
  rm -r build/res
  cp -r res build/res
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
elif [ "$1" = "test" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./test_runner
  fi
elif [ "$1" = "build" ] && [ "$2" = "run" ]
then
  rm -r build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./executable
elif [ "$1" = "build" ] && [ "$2" = "test" ]
then
  rm -r build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./test_runner
elif [ "$1" = "build" ] && [ "$2" = "test" ] && [ "$3" = "run" ]
then
  rm -r build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./test_runner
  ./executable
else
  echo "Command usage: ./build.sh command [command: build, run, build run]"
fi
