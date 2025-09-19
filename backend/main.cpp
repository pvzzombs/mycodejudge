#include <cstdio>
#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include "httplib.h"
#include "json.hpp"

#define OUTPUTLOCATION "/home/guest/tempbin/"

void allowCORS(httplib::Response &res) {
  res.set_header("Access-Control-Allow-Origin", "*");
  res.set_header("Allow", "GET, POST, HEAD, OPTIONS");
  res.set_header("Access-Control-Allow-Headers", "X-Requested-With, Content-Type, Accept, Origin, Authorization");
  res.set_header("Access-Control-Allow-Methods", "OPTIONS, GET, POST, HEAD");
}

bool compileCpp(std::string name, std::string &result) {
  std::FILE * pipe = NULL;
  char buffer[128];
  std::string cmd = "g++ -std=c++11 " + name + ".cpp -o " + name + ".out 2>&1";

  pipe = popen(cmd.c_str(), "r");
  while(fgets(buffer, 128, pipe) != NULL) {
    int i = 0;
    while(i < 128 && buffer[i] != 0) {
      if (buffer[i] == '\n') {
        result += "\\n";
      } else {
        result += buffer[i];
      }
      i++;
    }
  }
  pclose(pipe);

  if (result.find("error") != std::string::npos) {
    return 0;
  }
  return 1;
}

bool compileC(std::string name, std::string &result) {
  std::FILE * pipe = NULL;
  char buffer[128];
  std::string cmd = "gcc " + name + ".c -o " + name + ".out 2>&1";

  pipe = popen(cmd.c_str(), "r");
  while(fgets(buffer, 128, pipe) != NULL) {
    int i = 0;
    while(i < 128 && buffer[i] != 0) {
      if (buffer[i] == '\n') {
        result += "\\n";
      } else {
        result += buffer[i];
      }
      i++;
    }
  }
  pclose(pipe);

  if (result.find("error") != std::string::npos) {
    return 0;
  }
  return 1;
}

bool run(std::string name, std::string input, std::string &result) {
  std::FILE * pipe = NULL;
  char buffer[128];
  std::ofstream inputTextFile;
  std::string fileName = name + ".txt";
  std::string cmd = "timeout 2s " + name + ".out < " + fileName + " 2>&1";

  inputTextFile.open(fileName.c_str());
  inputTextFile << input;
  inputTextFile.close();

  pipe = popen(cmd.c_str(), "r");
  while(fgets(buffer, 128, pipe) != NULL) {
    int i = 0;
    while(i < 128 && buffer[i] != 0) {
      if (buffer[i] == '\n') {
        result += "\\n";
      } else {
        result += buffer[i];
      }
      i++;
    }
  }
  pclose(pipe);
  return 1;
}

std::string generateFileName() {
  std::string name = "";
  for (int i = 0; i < 16; i++) {
    name += (rand() % 26) + 'a';
  }
  return name;
}

int main() {
  httplib::Server svr;

  std::srand(std::time(NULL));

  std::cout << "Running..." << std::endl;

  svr.Options("/post", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/post", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string program = j["post"];
    std::string inputText = j["input"];
    std::string lang = j["lang"];
    std::string compileResult = "";
    std::string runResult = "";
    std::string name = OUTPUTLOCATION + generateFileName();
    std::string fileName = name;

    if (lang == "cpp") {
      fileName += ".cpp";
    } else if (lang == "c") {
      fileName += ".c";
    }

    std::cout << "Received..." << std::endl;

    std::ofstream cppFile;
    cppFile.open(fileName);
    cppFile << program;
    cppFile.close();
    //compile
    int r = 0;
    if (lang == "cpp") {
      r = compileCpp(name, compileResult);
    } else if (lang == "c") {
      r = compileC(name, compileResult);
    }
    if (r) {
      run(name, inputText, runResult);
    } 

    res.set_content("{\"errors\":\"" + compileResult + "\",\"result\":\"" + runResult + "\"}", "application/json");
    std::cout << "Done!!!" << std::endl;
  });

  svr.listen("localhost", 8000);

  return 0;
}