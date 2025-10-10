#pragma once
#include <atomic>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>

#define OUTPUT_FILE "/home/guest/fakesystem/home/rout.txt"
#define INPUT_FILE "/home/guest/fakesystem/home/rin.txt"
#define TEXT_DIR "/home/guest/fakesystem/home"

class NRP {
public:
  std::atomic<bool> quit;
  std::vector<std::string> arr;
  std::mutex arrMutex;
  NRP(): quit(false) {
  }
  void timer() {
    while (!quit.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  void wait() {
    while (!quit.load()) {
      std::ifstream f;
      std::string s;
      f.open(OUTPUT_FILE);
      if (f.is_open()) {
        std::getline(f, s);
        if (s == "ready") {
          std::ofstream o;
          // size_t n = arr.size();
          // int i = 0;
          o.open(INPUT_FILE, std::ios::app);
          if (arr.size()) {
            o << arr.front() << std::endl;
            // i++;
            arrMutex.lock();
            arr.erase(arr.begin());
            arrMutex.unlock();
          }
          o.close();
        }
      }
      f.close();
    }
  }

  bool isAvailable(std::string fileBaseName) {
    std::string withTxt = fileBaseName + ".done";
    std::ifstream f;
    f.open(withTxt);
    return f.good();
  }
};