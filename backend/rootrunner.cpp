#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>

#include <loguru.hpp>

#define OUTPUT_FILE "/mnt/sda2/fakesystem/home/sandbox/rout.txt"
#define INPUT_FILE "/mnt/sda2/fakesystem/home/sandbox/rin.txt"
#define FAKESYSTEMLOCATION "/mnt/sda2/fakesystem"
#define FAKESYSTEMUSER "sandbox"

bool isAlphaNumeric(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::string replaceJavaClassNameHelper(std::string fileContents, std::string baseName) {
  std::string output;
  int i = 0;
  std::string temp;
  int flag = 0;
  while (i < fileContents.size()) {
    if (!isAlphaNumeric(fileContents.at(i))) {
      if (temp != "")  {
        if (flag == 0) {
          if (temp == "class") {
            flag = 1;
          }
        } else if (flag == 1) {
          if (temp == "Main") {
            temp = baseName;
          } 
          flag = 0;
        }
        output += temp;
        temp = "";
      }
      output += fileContents.at(i);
    } else {
      temp += fileContents.at(i);
    }
    i++;
  }
  if (temp.size()) {
    output += temp;
    temp = "";
  }
  // std::cout << "Done replacing file" << std::endl;
  return output;
}

std::string extractBaseName(std::string name) {
  std::string result;
  bool hasDot = false;

  // find the dot
  int i = name.size() - 1;
  while (i > -1) {
    if (name.at(i) == '.') {
      hasDot = true;
      break;
    }
    i--;
  }

  if (!hasDot) {
    i = name.size() - 1;
  } else {
    i--;
  }

  while (i > -1 && name.at(i) != '/') {
    result = name.at(i) + result;
    i--;
  }

  // std::cout << "Done extract base name" << std::endl;
  return result;
}

void replaceJavaClassName(std::string fileName) {
  std::ifstream inputFile;
  std::string fileContents;
  std::string l;
  std::string baseName = extractBaseName(fileName);
  inputFile.open(fileName);
  if (inputFile.is_open()) {
    while (std::getline(inputFile, l)) {
      fileContents += l + "\n";
    }
  }
  inputFile.close();

  // do the processing 
  std::string newContents = replaceJavaClassNameHelper(fileContents, baseName);

  //rewrite file
  std::ofstream outFile;
  outFile.open(fileName);
  if (outFile.is_open()) {
    outFile << newContents;
  }
  outFile.close();
  // std::cout << "finished file replacing" << std::endl;
}

void wakeUpJava() {
  std::FILE * pipe = NULL;
  std::string cmd = "chroot " FAKESYSTEMLOCATION " /bin/su - " FAKESYSTEMUSER " -c 'java -version'";
  pipe = popen(cmd.c_str(), "r");
  pclose(pipe);
  std::cout << "Waking up java done..." << std::endl; 
}

void compileSourceCode(std::string fileName) {
  //detect code type:
  std::string baseName;
   if (fileName.find(".cpp") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 4);
    // std::cout << baseName << std::endl;
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
    // std::cout << baseName << std::endl;
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
  } else if (fileName.find(".java") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 5);
    std::string cmd = "export PATH=$PATH:/opt/jdk/bin; javac " + baseName + ".java 2>&1";
    // std::cout << "Hello" << std::endl;
    replaceJavaClassName(fileName);
    // std::cout << "Hey" << std::endl;
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
    // std::cout << baseName << std::endl;
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    // std::cout << "name: " << name << std::endl;
    // bash -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; /home/guest/a.out'
    std::string cmd = "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; chroot " FAKESYSTEMLOCATION " /bin/su - " FAKESYSTEMUSER " -c '\\''" + name + ".out'\\'' < " + baseName + ".txt 2>&1'";
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
    pclose(pipe);

    file.open(baseName + ".done");
    file.close();
  } else if (fileName.find(".c") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 2);
    // std::cout << baseName << std::endl;
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    // std::cout << "name: " << name << std::endl;
    // bash -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; /home/guest/a.out'
    std::string cmd = "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; chroot " FAKESYSTEMLOCATION " /bin/su - " FAKESYSTEMUSER" -c '\\''" + name + ".out'\\'' < " + baseName + ".txt 2>&1'";
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
    pclose(pipe);

    file.open(baseName + ".done");
    file.close();
  } else if (fileName.find(".java") != std::string::npos) {
    baseName = fileName.substr(0, fileName.size() - 5);
    // std::cout << baseName << std::endl;
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    std::string baseNameNoExtension = extractBaseName(fileName);
    // std::cout << "name: " << name << std::endl;
    // bash -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; /home/guest/a.out'
    std::string cmd = "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/guest/cgroup.procs; chroot " FAKESYSTEMLOCATION " /bin/su - " FAKESYSTEMUSER " -c '\\''java -Xmx4m -Xms1m -Xss256k " + baseNameNoExtension + "'\\'' < " + baseName + ".txt 2>&1'";
    std::cout << "command is " << cmd << std::endl;
    std::FILE * pipe = NULL;
    char buffer[128];

    std::ofstream file;

    wakeUpJava();

    file.open(baseName + ".result");

    pipe = popen(cmd.c_str(), "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      file << buffer;
    }
    file.close();
    pclose(pipe);

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
    std::streampos lastPos = 0;

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
      in.open(INPUT_FILE);
      if (in.is_open()) {
        in.seekg(lastPos);

        while (std::getline(in, l)) {
          fileNames.push_back(l);
          lastPos = in.tellg();
        }

        for (int i = 0; i < fileNames.size(); i++) {
          compileSourceCode(fileNames.at(i));
          runExecutable(fileNames.at(i));
        }

        fileNames.clear();
      }
      in.close();

      // out.open(INPUT_FILE);
      // out.close();
    }
  });

  t1.join();
  t2.join();
  return 0;
}