@echo off

if "%1"=="build" (
  if "%2"=="" (
    if exist build\res (
        rmdir /s /q build\res
    )
    xcopy /s /e /i res build\res
    cd build
    cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
    cmake --build .
    goto :eof
  )
)

if "%1"=="run" (
  if "%2"=="" (
    if not exist build (
        echo Build folder not located: .\build.bat build
        goto :eof
    )
    cd build
    executable.exe
    goto :eof
  )
)

if "%1"=="run" (
  if "%2"=="server" (
    if not exist build (
        echo Build folder not located: .\build.bat build
        goto :eof
    )
    cd build
    executable.exe server
    goto :eof
  )
)

if "%1"=="test" (
  if not exist build (
      echo Build folder not located: .\build.bat build
      goto :eof
  )
  cd build
  test_runner.exe
  goto :eof
)

if "%1"=="build" (
  if "%2"=="run" (
    if "%3"=="server" (
      if exist build\res (
          rmdir /s /q build\res
      )
      xcopy /s /e /i res build\res
      cd build
      cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
      mingw32-make
      executable.exe server
      goto :eof
    )
  )
)

if "%1"=="build" (
  if "%2"=="test" (
    if exist build\res (
        rmdir /s /q build\res
    )
    xcopy /s /e /i res build\res
    cd build
    cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
    cmake --build .
    test_runner.exe
    goto :eof
  )
)

if "%1"=="build" (
  if "%2"=="test" (
    if "%3"=="run" (
      if exist build\res (
          rmdir /s /q build\res
      )
      xcopy /s /e /i res build\res
      cd build
      cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
      cmake --build .
      test_runner.exe
      executable.exe
      goto :eof
    )
  )
)

if "%1"=="build" (
  if "%2"=="test" (
    if "%3"=="run" (
      if "%4"=="server" (
        if exist build\res (
            rmdir /s /q build\res
        )
        xcopy /s /e /i res build\res
        cd build
        cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
        cmake --build .
        test_runner.exe
        executable.exe server
        goto :eof
      )
    )
  )
)

echo Command keywords: [build, run, server, test]

:eof
