// Copyright 2024 Kiselev Igor
#include "seq/kiselev_i_shell_simple/include/shell_simple.hpp"

#include <thread>

using namespace std::chrono_literals;

bool Kiselev_seq::KiselevTaskSequential::pre_processing() {
  try {
    internal_order_test();
    size_t n = taskData->inputs_count[0];
    arr = std::vector<int>(n, 0);
    for (size_t i = 0; i < n; ++i) {
      int *elem = reinterpret_cast<int *>(taskData->inputs[0] + i * sizeof(int));
      arr[i] = *elem;
    }
  } catch (...) {
    return false;
  }
  return true;
}

bool Kiselev_seq::KiselevTaskSequential::validation() {
  try {
    internal_order_test();
    return taskData->inputs_count[0] != 0 && taskData->inputs_count[0] == taskData->outputs_count[0];
  } catch (...) {
    return false;
  }
}

bool Kiselev_seq::KiselevTaskSequential::run() {
  try {
    internal_order_test();
    int n = arr.size();
    int incr = n / 2;
    while (incr > 0) {
      for (int i = incr; i < n; i++) {
        int j = i - incr;
        while (j > 0)
          if (arr[j] > arr[j + incr]) {
            arr[j] += arr[j + incr];
            arr[j + incr] = arr[j] - arr[j + incr];
            arr[j] = arr[j] - arr[j + incr];
            j = j - incr;
          } else
            j = 0;
      }
      incr = incr / 2;
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool Kiselev_seq::KiselevTaskSequential::post_processing() {
  try {
    internal_order_test();
    size_t n = arr.size();
    for (size_t i = 0; i < n; i++) {
      int *res = reinterpret_cast<int *>(taskData->outputs[0] + i * sizeof(int));
      *res = arr[i];
    }
    return true;
  } catch (...) {
    return false;
  }
}
