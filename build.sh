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
elif [ "$1" = "run" ] && [ "$2" = "sender" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable sender
  fi
elif [ "$1" = "run" ] && [ "$2" = "receiver" ]
then
  if ! [ -d "./build" ]
  then
    echo "Build folder not located: ./build.sh build"
  else
    cd build
    ./executable receiver
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
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  ./executable
elif [ "$1" = "build" ] && [ "$2" = "test" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
  ./test_runner
elif [ "$1" = "build" ] && [ "$2" = "test" ] && [ "$3" = "run" ]
then
  rm -rf build/res
  cp -r res build/res
  cd build
  cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cmake --build .
  ./test_runner
  ./executable
elif [ "$1" = "server" ]
then
  cd svr
  npm start
else
  echo "Command keywords: [build, run, test]"
fi
