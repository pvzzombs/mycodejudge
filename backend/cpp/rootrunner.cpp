#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>

#include <loguru.hpp>
#include <ThreadPool.h>

#include "includes.hpp"
#include "utils.hpp"

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
  char buffer_cmd[1024];
   if (fileName.find(".cpp") != std::string::npos) {
    #ifndef _WIN32
    baseName = fileName.substr(0, fileName.size() - 4);
    COMPILE_COMMAND_EXEC_CPP;
    std::ofstream warnings;
    std::FILE * pipe = NULL;
    char buffer[128];
    warnings.open(baseName + ".w");
    pipe = popen(buffer_cmd, "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      warnings << buffer;
    }
    pclose(pipe);
    warnings.close();
    #endif
  } else if (fileName.find(".c") != std::string::npos) {
    #ifndef _WIN32
    baseName = fileName.substr(0, fileName.size() - 2);
    COMPILE_COMMAND_EXEC_C;
    std::ofstream warnings;
    std::FILE * pipe = NULL;
    char buffer[128];
    warnings.open(baseName + ".w");
    pipe = popen(buffer_cmd, "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      warnings << buffer;
    }
    pclose(pipe);
    warnings.close();
    #endif
  } else if (fileName.find(".java") != std::string::npos) {
    #ifndef _WIN32
    baseName = fileName.substr(0, fileName.size() - 5);
    COMPILE_COMMAND_EXEC_JAVA;
    replaceJavaClassName(fileName);
    std::ofstream warnings;
    std::FILE * pipe = NULL;
    char buffer[128];
    warnings.open(baseName + ".w");
    pipe = popen(buffer_cmd, "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      warnings << buffer;
    }
    pclose(pipe);
    warnings.close();
    #endif
  }
}

void runExecutable(std::string fileName) {
  std::string baseName;
  char buffer_cmd[1024];
  if (fileName.find(".cpp") != std::string::npos) {
    #ifndef _WIN32
    baseName = fileName.substr(0, fileName.size() - 4);
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    RUN_COMMAND_EXEC_CPP;
    std::cout << "command is " << buffer_cmd << std::endl;
    std::FILE * pipe = NULL;
    char buffer[128];

    std::ofstream file;

    file.open(baseName + ".result");

    pipe = popen(buffer_cmd, "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      file << buffer;
    }
    file.close();
    pclose(pipe);

    file.open(baseName + ".done");
    file.close();
    #endif
  } else if (fileName.find(".c") != std::string::npos) {
    #ifndef _WIN32
    baseName = fileName.substr(0, fileName.size() - 2);
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    RUN_COMMAND_EXEC_C;
    std::cout << "command is " << buffer_cmd << std::endl;
    std::FILE * pipe = NULL;
    char buffer[128];

    std::ofstream file;

    file.open(baseName + ".result");

    pipe = popen(buffer_cmd, "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      file << buffer;
    }
    file.close();
    pclose(pipe);

    file.open(baseName + ".done");
    file.close();
    #endif
  } else if (fileName.find(".java") != std::string::npos) {
    #ifndef _WIN32
    baseName = fileName.substr(0, fileName.size() - 5);
    std::string fakesystempath = FAKESYSTEMLOCATION;
    std::string name = baseName.substr(fakesystempath.size(), baseName.size());
    std::string baseNameNoExtension = extractBaseName(fileName);
    RUN_COMMAND_EXEC_JAVA;
    std::cout << "command is " << buffer_cmd << std::endl;
    std::FILE * pipe = NULL;
    char buffer[128];

    std::ofstream file;

    wakeUpJava();

    file.open(baseName + ".result");

    pipe = popen(buffer_cmd, "r");
    while(fgets(buffer, 128, pipe) != NULL) {
      file << buffer;
    }
    file.close();
    pclose(pipe);

    file.open(baseName + ".done");
    file.close();
    #endif
  }
}

int main(int argc, char * argv[]) {
  std::ofstream out;
  std::vector<std::string> fileNames;
  std::atomic<bool> quit;
  uid_t euid = geteuid();
  ThreadPool pool(4);

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
    std::string headerStub;

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
        std::getline(in, l);
        if (l != headerStub) {
          in.seekg(in.tellg());
          headerStub = l;
        } else {
          in.seekg(lastPos);
        }

        while (std::getline(in, l)) {
          fileNames.push_back(l);
          lastPos = in.tellg();
        }

        for (int i = 0; i < fileNames.size(); i++) {
          std::string currentFileName = fileNames.at(i);
          pool.enqueue([currentFileName]() {
            compileSourceCode(currentFileName);
            runExecutable(currentFileName);
          });
        }

        fileNames.clear();
      }
      in.close();
    }
  });

  t1.join();
  t2.join();
  return 0;
}
