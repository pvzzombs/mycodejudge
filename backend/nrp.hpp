#pragma once
#include <atomic>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define OUTPUT_FILE "/home/guest/fakesystem/home/sandbox/rout.txt"
#define INPUT_FILE "/home/guest/fakesystem/home/sandbox/rin.txt"
#define TEXT_DIR "/home/guest/fakesystem/home"

class NRP {
public:
  std::atomic<bool> quit;
  std::vector<std::string> arr;
  std::mutex arrMutex;
  std::condition_variable cv;
  NRP(): quit(false) {
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
          if (s == "ready") {
            std::ofstream o;
            // size_t n = arr.size();
            // int i = 0;
            o.open(INPUT_FILE, std::ios::app);
            o << arr.front() << std::endl;
            arr.erase(arr.begin());
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