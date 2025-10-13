#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>

#include <loguru.hpp>

#define OUTPUT_FILE "/home/guest/fakesystem/home/rout.txt"
#define INPUT_FILE "/home/guest/fakesystem/home/rin.txt"
#define FAKESYSTEMLOCATION "/home/guest/fakesystem"

void compileSourceCode(std::string fileName) {
  //detect code type:
  std::string baseName;
   if (fileName.find(".cpp") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 4);
    std::cout << baseName << std::endl;
    std::string cmd = "g++ " + baseName + ".cpp -o " + baseName + ".out 2>&1";
    std::ofstream warnings;
    std::FILE * pipe = NULL;
    char buffer[128];
    warnings.open(baseName + ".w");
    pipe = popen(cmd.c_str(), "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      warnings << buffer;
    }
    pclose(pipe);
    warnings.close();
  } else if (fileName.find(".c") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 2);
    std::cout << baseName << std::endl;
    std::string cmd = "gcc " + baseName + ".c -o " + baseName + ".out 2>&1";
    std::ofstream warnings;
    std::FILE * pipe = NULL;
    char buffer[128];
    warnings.open(baseName + ".w");
    pipe = popen(cmd.c_str(), "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      warnings << buffer;
    }
    pclose(pipe);
    warnings.close();
  }
}

void runExecutable(std::string fileName) {
  std::string baseName;
  if (fileName.find(".cpp") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 4);
    std::cout << baseName << std::endl;
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    std::cout << "name: " << name << std::endl;
    // bash -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; /home/guest/a.out'
    std::string cmd = "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; chroot " FAKESYSTEMLOCATION " " + name + ".out < " + baseName + ".txt 2>&1'";
    std::cout << "command is " << cmd << std::endl;
    std::FILE * pipe = NULL;
    char buffer[128];

    std::ofstream file;

    file.open(baseName + ".result");

    pipe = popen(cmd.c_str(), "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      file << buffer;
    }
    file.close();

    file.open(baseName + ".done");
    file.close();
  } else if (fileName.find(".c") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 2);
    std::cout << baseName << std::endl;
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    std::cout << "name: " << name << std::endl;
    // bash -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; /home/guest/a.out'
    std::string cmd = "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; chroot " FAKESYSTEMLOCATION " " + name + ".out < " + baseName + ".txt 2>&1'";
    std::cout << "command is " << cmd << std::endl;
    std::FILE * pipe = NULL;
    char buffer[128];

    std::ofstream file;

    file.open(baseName + ".result");

    pipe = popen(cmd.c_str(), "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      file << buffer;
    }
    file.close();

    file.open(baseName + ".done");
    file.close();
  }
}

int main(int argc, char * argv[]) {
  std::ofstream out;
  std::vector<std::string> fileNames;
  std::atomic<bool> quit;
  uid_t euid = geteuid();

  quit.store(false);

  loguru::init(argc, argv);

  LOG_F(INFO, "Root runner started...");

  if (euid != 0) {
    LOG_F(ERROR, "Please run as root, continuing will cause errors!");
  }

  std::thread t1([&]() {
    bool shouldExit = false;
    do {
      std::string s;
      std::getline(std::cin, s);
      if (s.size() > 0 && s.at(0) == 'q') {
        shouldExit = true;
        quit.store(true);
      }
    } while(!shouldExit);
  });

  std::thread t2([&]() {
    while(!quit.load()) {
      out.open(OUTPUT_FILE);
      out << "ready" << std::endl;
      out.close();

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      out.open(OUTPUT_FILE);
      out << "close" << std::endl;
      out.close();

      std::vector<std::string> fileNames;

      std::ifstream in;
      std::string l;
      std::streampos lastPos = 0;
      in.open(INPUT_FILE);
      while (std::getline(in, l)) {
        fileNames.push_back(l);
        lastPos = in.tellg();
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(50));

      in.clear();
      in.seekg(lastPos);
      while (std::getline(in, l)) {
        fileNames.push_back(l);
        // lastPos = in.tellg();
      }

      for (int i = 0; i < fileNames.size(); i++) {
        compileSourceCode(fileNames.at(i));
        runExecutable(fileNames.at(i));
      }

      fileNames.clear();
      in.close();

      out.open(INPUT_FILE);
      out.close();
    }
  });

  t1.join();
  t2.join();
  return 0;
}