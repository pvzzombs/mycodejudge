#include <cstdio>
#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <mutex>

#include <sodium.h>
#include "include/httplib.h"
#include "include/json.hpp"
#include "include/sqlitewrapper.hpp"

#define OUTPUTLOCATION "/home/guest/tempbin/"
#define FAKESYSTEMLOCATION "/home/guest/fakesystem"
#define ADMINNAME "admin"
#define ADMINPASS "admin"

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

bool runChroot(std::string name, std::string input, std::string &result) {
  std::FILE * pipe = NULL;
  char buffer[128];
  std::ofstream inputTextFile;
  std::string fileNameScoped = name + ".txt";
  std::string fileName = FAKESYSTEMLOCATION + fileNameScoped;
  std::string cmd = "timeout 2s unshare -r chroot " FAKESYSTEMLOCATION " " + name + ".out < " + fileName + " 2>&1";

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

std::string hash_password(std::string password) {
  // password hash len
  char passwordHash[crypto_pwhash_STRBYTES];
  if (crypto_pwhash_str(passwordHash, password.c_str(), password.size(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
    throw ;
  }
  return std::string(passwordHash);
}

bool verify_password(std::string password, std::string passwordHash) {
  return crypto_pwhash_str_verify(passwordHash.c_str(), password.c_str(), password.size()) == 0;
}

int main() {
  if (sodium_init() < 0) {
    std::cout << "libsodium not working" << std::endl;
    return 1;
  }

  std::string adminName = ADMINNAME;
  std::string adminPassword = hash_password(ADMINPASS);

  httplib::Server svr;

  std::string dbname = OUTPUTLOCATION;
  dbname += "test.db";

  Sqlite::SqliteConnection conn(dbname.c_str());

  std::srand(std::time(NULL));

  Sqlite::sqliteExecute(conn, "delete from admin");
  Sqlite::sqliteExecute(conn, "insert into admin(username, password) values(?, ?)", adminName, adminPassword);

  std::cout << "Running..." << std::endl;

  svr.Options("/submitsolution", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/submitsolution", [&](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string username = j["username"];
    std::cout << "Username is " << username << std::endl;
    std::string sessionid = j["sessionid"];
    std::string title = j["title"];
    std::string solution = j["solution"];
    std::string solndate = j["submitdate"];
    std::string solved = j["isSolved"];

    for (auto row: Sqlite::SqliteStatement(conn, "select sessionid from sessions where username = ?", username)) {
      if (verify_password(sessionid, row.getString(0))) {
        Sqlite::sqliteExecute(conn, "insert into solutions(username, title, submitdate, solution, isSolved) values(?, ?, ?, ?, ?)", username, title, solndate, solution, solved);

        std::cout << "Solution submit success!" << std::endl;
        res.set_content("{\"status\":\"success\"}", "application/json");
        return ;
      }
    }

    std::cout << "Solution submit failed!" << std::endl;
    res.set_content("{\"status\":\"failed\"}", "application/json");
  });

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

  svr.Options("/postsafe", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/postsafe", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string program = j["post"];
    std::string inputText = j["input"];
    std::string lang = j["lang"];
    std::string compileResult = "";
    std::string runResult = "";
    std::string scopedname = "/home/" + generateFileName();
    std::string name = FAKESYSTEMLOCATION + scopedname;
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
      runChroot(scopedname, inputText, runResult);
    } 

    res.set_content("{\"errors\":\"" + compileResult + "\",\"result\":\"" + runResult + "\"}", "application/json");
    std::cout << "Done!!!" << std::endl;
  });

  svr.Options("/deleteproblems", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/deleteproblems", [&](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string username = j["admin"];
    std::string password = j["password"];
    for (auto row: Sqlite::SqliteStatement(conn, "select password from admin where username = ?", username)) {
      if (verify_password(password, row.getString(0))) {
        Sqlite::sqliteExecute(conn, "delete from problems");

        std::cout << "All problems deleted" << std::endl;
        res.set_content("{\"status\":\"success\"}", "application/json");
        return ;
      }
    }
    std::cout << "Deleting problems failed" << std::endl;
    res.set_content("{\"status\":\"failed\"}", "application/json");
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
      
      if (!verify_password(sessionid, row.getString(0))) {
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
      if (verify_password(password, row.getString(0))) {
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
    std::string password = hash_password(j["password"]);

    for (auto row: Sqlite::SqliteStatement(conn, "select username from users where username = ?", username)) {
      std::cout << "Register duplicate" << std::endl;
      res.set_content("{\"status\":\"failed\"}", "application/json");
      return ;
    }

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
      if (verify_password(password, row.getString(0))) {
        // allow
        std::string sessionid = generateSessionID();
        std::string sessionidhash = hash_password(sessionid);
        Sqlite::sqliteExecute(conn, "insert into sessions(username, sessionid) values (?, ?)", username, sessionidhash);
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

  svr.Options("/logout", [](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
  });

  svr.Post("/logout", [&](const httplib::Request &req, httplib::Response &res){
    allowCORS(res);
    nlohmann::json j = nlohmann::json::parse(req.body);
    std::string username = j["username"];
    std::string sessionid = j["sessionid"];
    Sqlite::sqliteExecute(conn, "delete from sessions where username = ? and sessionid = ?", username, sessionid);
    res.set_content("{\"status\":\"success\"}", "application/json");
  });

  svr.listen("localhost", 8000);

  return 0;
}