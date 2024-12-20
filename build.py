import os
import sys
import shutil
import subprocess
import platform

def clean_and_copy_resources(hard=False):
  if os.path.exists("build/res"):
    if hard:
      shutil.copytree("build/res/assets/volatile", "temp/assets/volatile")
    shutil.rmtree("build/res")
  shutil.copytree("res", "build/res")
  if hard:
    shutil.rmtree("build/res/assets/volatile")
    shutil.copytree("temp/assets/volatile", "build/res/assets/volatile")
    shutil.rmtree("temp")

def run_cmake():
  os.makedirs("build", exist_ok=True)
  os.chdir("build")
  if platform.system() == "Windows":
    subprocess.run(["cmake", "-G", "MinGW Makefiles", "..", "-DCMAKE_C_COMPILER=gcc", "-DCMAKE_EXPORT_COMPILE_COMMANDS=1"])
  else:
    subprocess.run(["cmake", "..", "-DCMAKE_C_COMPILER=gcc", "-DCMAKE_EXPORT_COMPILE_COMMANDS=1"])

def build_project():
  subprocess.run(["cmake", "--build", "."])

def run_executable(args=""):
  executable = "executable.exe" if platform.system() == "Windows" else "./executable"
  subprocess.run([executable] + ([args] if args else []))

def run_test_runner():
  test_runner = "test_runner.exe" if platform.system() == "Windows" else "./test_runner"
  subprocess.run([test_runner])

def main():
  if len(sys.argv) < 2:
    print("Command keywords: [build, run, test]")
    return
  command = sys.argv[1]
  arg2 = sys.argv[2] if len(sys.argv) > 2 else ""
  arg3 = sys.argv[3] if len(sys.argv) > 3 else ""
  if command == "emplace" and not arg2:
    clean_and_copy_resources(True)
    run_cmake()
    build_project()
  elif command == "build" and not arg2:
    clean_and_copy_resources()
    run_cmake()
    build_project()
  elif command == "run" and not arg2:
    os.chdir("build")
    run_executable()
  elif command == "test" and not arg2:
    os.chdir("build")
    run_test_runner()
  elif command == "run":
    os.chdir("build")
    run_executable(arg2)
  elif command == "build" and arg2 == "run" and not arg3:
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_executable()
  elif command == "emplace" and arg2 == "run" and not arg3:
    clean_and_copy_resources(True)
    run_cmake()
    build_project()
    run_executable()
  elif command == "build" and arg2 == "run":
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_executable(arg3)
  elif command == "emplace" and arg2 == "run":
    clean_and_copy_resources(True)
    run_cmake()
    build_project()
    run_executable(arg3)
  elif command == "build" and arg2 == "test":
    clean_and_copy_resources()
    run_cmake()
    build_project()
    run_test_runner()
  elif command == "emplace" and arg2 == "test":
    clean_and_copy_resources(True)
    run_cmake()
    build_project()
    run_test_runner()
  else:
    print("Command keywords: [emplace, build, run, test]")

if __name__ == "__main__":
  main()

