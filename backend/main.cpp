#include <cstdio>
#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include "httplib.h"
#include "json.hpp"
#include "sqlitewrapper.hpp"

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

std::string generateSessionID() {
  std::string name = "";
  for (int i = 0; i < 128; i++) {
    name += (rand() % 26) + 'a';
  }
  return name;
}

int main() {
  httplib::Server svr;

  std::string dbname = OUTPUTLOCATION;
  dbname += "test.db";

  Sqlite::SqliteConnection conn(dbname.c_str());

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

  svr.Options("/getproblems", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/getproblems", [&](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string username = j["username"];
    std::string sessionid = j["sessionid"];

    for (auto row: Sqlite::SqliteStatement(conn, "select sessionid from sessions where username = ?", username)) {
      
      if (row.getString(0) != sessionid) {
        continue;
      }
      
      nlohmann::json out;
      out["status"] = "success";
      out["list"] = {};
      for (auto problem: Sqlite::SqliteStatement(conn, "select title, description, testcases, answers from problems")) {
        nlohmann::json p;
        p["title"] = problem.getString(0);
        p["desc"] = problem.getString(1);
        p["testcases"] = problem.getString(2);
        p["answers"] = problem.getString(3);
        out["list"].push_back(p);
      }
      std::cout << "Problem request success" << std::endl;
      res.set_content(out.dump(), "application/json");
      return ;
    }

    std::cout << "Problem request failed" << std::endl;
    res.set_content("{\"status\":\"failed\"}", "application/json");
    return ;
  });

  svr.Options("/postproblem", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/postproblem", [&](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string username = j["admin"];
    std::string password = j["password"];
    std::string title = j["title"];
    std::string description = j["desc"];
    std::string testcases = j["testcases"];
    std::string answers = j["answers"];

    for (auto row: Sqlite::SqliteStatement(conn, "select password from admin where username = ?", username)) {
      if (row.getString(0) == password) {
        Sqlite::sqliteExecute(conn, "insert into problems(title, description, testcases, answers) values(?, ?, ?, ?)", title, description, testcases, answers);

        std::cout << "Problem posted" << std::endl;
        res.set_content("{\"status\":\"success\"}", "application/json");
        return ;
      }
    }
    std::cout << "Problem error" << std::endl;
    res.set_content("{\"status\":\"failed\"}", "application/json");
  });

  svr.Options("/register", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/register", [&](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string username = j["username"];
    std::string password = j["password"];

    Sqlite::sqliteExecute(conn, "insert into users(username, password) values(?, ?)", username, password);

    std::cout << "Register done" << std::endl;
    res.set_content("{\"status\":\"success\"}", "application/json");

  });

  svr.Options("/login", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/login", [&](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string username = j["username"];
    std::string password = j["password"];

    for (auto row: Sqlite::SqliteStatement(conn, "select password from users where username = ?", username)) {
      if (row.getString(0) == password) {
        // allow
        std::string sessionid = generateSessionID();
        Sqlite::sqliteExecute(conn, "insert into sessions(username, sessionid) values (?, ?)", username, sessionid);
        std::cout << "Login Success!" << std::endl;
        nlohmann::json out;
        out["status"] = "success";
        out["username"] = username;
        out["sessionid"] = sessionid;
        res.set_content(out.dump(), "application/json");
        return;
      }
    }
    std::cout << "Login failed" << std::endl;
    res.set_content("{\"status\":\"failed\"}", "application/json");
  });

  svr.listen("localhost", 8000);

  return 0;
}