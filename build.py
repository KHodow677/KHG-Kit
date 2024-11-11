import os
import sys
import shutil
import subprocess
import platform

def clean_and_copy_resources():
  if os.path.exists("build/res"):
    shutil.rmtree("build/res")
  shutil.copytree("res", "build/res")

def run_cmake():
  os.makedirs("build", exist_ok=True)
  os.chdir("build")
  subprocess.run(["cmake", "..", "-DCMAKE_C_COMPILER=gcc", "-DCMAKE_EXPORT_COMPILE_COMMANDS=1"])

def build_project():
  subprocess.run(["cmake", "--build", "."])

def run_executable(args=""):
  executable = "executable.exe" if platform.system() == "Windows" else "./executable"
  if os.path.isdir("./build"):
    os.chdir("build")
    subprocess.run([executable] + ([args] if args else []))
  else:
    print("Build folder not located: run `build.py build` first")

def run_test_runner():
  test_runner = "test_runner.exe" if platform.system() == "Windows" else "./test_runner"
  if os.path.isdir("./build"):
    os.chdir("build")
    subprocess.run([test_runner])
  else:
    print("Build folder not located: run `build.py build` first")

def main():
  if len(sys.argv) < 2:
    print("Command keywords: [build, run, test]")
    return
  command = sys.argv[1]
  arg2 = sys.argv[2] if len(sys.argv) > 2 else ""
  arg3 = sys.argv[3] if len(sys.argv) > 3 else ""
  arg4 = sys.argv[4] if len(sys.argv) > 4 else ""
  if command == "build" and not arg2:
    clean_and_copy_resources()
    run_cmake()
    build_project()
  elif command == "run" and not arg2:
    run_executable()
  elif command == "run" and arg2 == "hoster":
    run_executable("hoster")
  elif command == "run" and arg2 == "joiner":
    run_executable("joiner")
  elif command == "test":
    run_test_runner()
  elif command == "build" and arg2 == "run" and not arg3:
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_executable()
  elif command == "build" and arg2 == "run" and arg3 == "hoster":
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_executable("hoster")
  elif command == "build" and arg2 == "run" and arg3 == "joiner":
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_executable("joiner")
  elif command == "build" and arg2 == "test":
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_test_runner()
  elif command == "build" and arg2 == "test" and arg3 == "run" and not arg4:
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_test_runner()
    run_executable()
  elif command == "build" and arg2 == "test" and arg3 == "run" and arg4 == "hoster":
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_test_runner()
    run_executable("hoster")
  elif command == "build" and arg2 == "test" and arg3 == "run" and arg4 == "joiner":
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_test_runner()
    run_executable("joiner")
  else:
    print("Command keywords: [build, run, test]")

if __name__ == "__main__":
  main()

