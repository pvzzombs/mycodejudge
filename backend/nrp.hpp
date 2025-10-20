#pragma once
#include <cstdio>
#include <atomic>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <loguru.hpp>

#define OUTPUT_FILE "/mnt/sda2/fakesystem/home/sandbox/rout.txt"
#define INPUT_FILE "/mnt/sda2/fakesystem/home/sandbox/rin.txt"

std::string generateRandomHeader() {
  std::string name = "";
  for (int i = 0; i < 16; i++) {
    name += (rand() % 26) + 'a';
  }
  return name;
}

class NRP {
public:
  std::atomic<bool> quit;
  std::vector<std::string> arr;
  std::mutex arrMutex;
  std::condition_variable cv;
  NRP(): quit(false) {
    std::srand(std::time(NULL));
  }
  void wait() {
    std::ofstream nro;
    nro.open(INPUT_FILE);
    nro.close();
    while (!quit.load()) {
      std::unique_lock<std::mutex> lck(arrMutex);
      cv.wait(lck, [&]() {
        return arr.size() || quit.load();
      });
      while (arr.size()) {
        if (quit.load()) {
          break;
        }
        std::ifstream f;
        std::string s;
        f.open(OUTPUT_FILE);
        if (f.is_open()) {
          std::getline(f, s);
          std::streampos pos;
          if (s == "ready") {
            std::ofstream o;
            // size_t n = arr.size();
            // int i = 0;
            o.open(INPUT_FILE, std::ios::app);
            if (o.is_open()) {
              pos = o.tellp();
              if (pos == 0) {
                o << generateRandomHeader() << std::endl;
                // std::cout << "Stub was written" << std::endl;
                LOG_F(INFO, "Stub was written...");
              }
              o << arr.front() << std::endl;
              arr.erase(arr.begin());
            }
            o.close();
          }
        }
        f.close();
      }
      // std::cout << "Ended" << std::endl;
    }
  }

  bool isAvailable(std::string fileBaseName) {
    std::string withTxt = fileBaseName + ".done";
    std::ifstream f;
    f.open(withTxt);
    return f.good();
  }
};