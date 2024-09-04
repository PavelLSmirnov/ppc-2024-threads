// Copyright 2024 Smirnov Pavel
#include "stl/smirnov_p_sort_double_batcher/include/ops_stl.hpp"

namespace SmirnovPavelStl {

constexpr int sizeDouble = sizeof(double);

bool RadixSortDoubleWithBatcherSequential::pre_processing() {
  internal_order_test();
  // Init value for input and output
  array = std::vector<double>(taskData->inputs_count[0]);

  auto *tmp_arr = reinterpret_cast<double *>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    array[i] = tmp_arr[i];
  }
  return true;
}

bool RadixSortDoubleWithBatcherSequential::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == taskData->outputs_count[0];
}

bool RadixSortDoubleWithBatcherSequential::run() {
  internal_order_test();
  result = radixSortWithBatcherSeq(array);
  return true;
}

bool RadixSortDoubleWithBatcherSequential::post_processing() {
  internal_order_test();
  std::copy(result.begin(), result.end(), reinterpret_cast<double *>(taskData->outputs[0]));
  return true;
}

std::vector<double> batchersMergeSeq(std::vector<std::vector<double>> &subVec) {
  std::vector<double> result;
  if (subVec.empty()) {
    return result;
  }

  std::vector<std::pair<double, int>> valueIndices;
  for (size_t i = 0; i < subVec.size(); ++i) {
    for (const auto &val : subVec[i]) {
      valueIndices.emplace_back(val, i);
    }
  }

  std::sort(valueIndices.begin(), valueIndices.end());
  result.resize(valueIndices.size());

  for (size_t i = 0; i < valueIndices.size(); ++i) {
    result[i] = valueIndices[i].first;
  }

  return result;
}

void sortPartSeq(std::vector<std::vector<double>> &parts, std::vector<double> &vec) {
  for (int i = 0; i < sizeDouble; ++i) {
    for (auto &j : vec) {
      auto tmp = static_cast<uint64_t>(j);
      tmp >>= i * 8;
      tmp &= 255;
      parts[tmp].push_back(j);
    }

    vec = batchersMergeSeq(parts);

    for (auto &part : parts) {
      part.clear();
    }
  }
}

std::vector<double> radixSortWithBatcherSeq(std::vector<double> vec) {
  uint64_t mask = static_cast<uint64_t>(1) << (sizeDouble * 8 - 1);
  std::vector<double> positive;
  std::vector<double> negative;

  for (auto &i : vec) {
    auto tmp = static_cast<uint64_t>(i);

    if ((tmp & mask) != 0) {
      negative.push_back(i);
    } else {
      positive.push_back(i);
    }
  }

  std::vector<std::vector<double>> parts(256);
  sortPartSeq(parts, negative);
  sortPartSeq(parts, positive);

  vec.clear();
  vec.reserve(negative.size() + positive.size());
  vec.insert(vec.end(), negative.begin(), negative.end());
  vec.insert(vec.end(), positive.begin(), positive.end());

  return vec;
}

bool RadixSortDoubleWithBatcherStlParallel::pre_processing() {
  internal_order_test();
  // Init value for input and output
  array = std::vector<double>(taskData->inputs_count[0]);

  auto *tmp_arr = reinterpret_cast<double *>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    array[i] = tmp_arr[i];
  }
  return true;
}

bool RadixSortDoubleWithBatcherStlParallel::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == taskData->outputs_count[0];
}

bool RadixSortDoubleWithBatcherStlParallel::run() {
  internal_order_test();
  result = radixSortWithBatcherStl(array);
  return true;
}

bool RadixSortDoubleWithBatcherStlParallel::post_processing() {
  internal_order_test();
  std::copy(result.begin(), result.end(), reinterpret_cast<double *>(taskData->outputs[0]));
  return true;
}

std::vector<double> batchersMergeStl(std::vector<std::vector<double>> &subVec) {
  std::vector<double> result;
  if (subVec.empty()) {
    return result;
  }

  std::vector<std::pair<double, int>> valueIndices;
  for (size_t i = 0; i < subVec.size(); ++i) {
    for (const auto &val : subVec[i]) {
      valueIndices.emplace_back(val, i);
    }
  }

  std::sort(valueIndices.begin(), valueIndices.end());
  result.resize(valueIndices.size());

  std::transform(valueIndices.begin(), valueIndices.end(), result.begin(),
                 [](const std::pair<double, int> &p) { return p.first; });

  return result;
}

void sortPartStl(std::vector<std::vector<double>> &parts, std::vector<double> &vec) {
  for (int i = 0; i < sizeDouble; ++i) {
    for (auto &j : vec) {
      auto tmp = static_cast<uint64_t>(j);
      tmp >>= i * 8;
      tmp &= 255;
      parts[tmp].push_back(j);
    }

    vec = batchersMergeStl(parts);

    for (auto &part : parts) {
      part.clear();
    }
  }
}

std::vector<double> radixSortWithBatcherStl(std::vector<double> vec) {
  uint64_t mask = static_cast<uint64_t>(1) << (sizeof(double) * 8 - 1);
  std::vector<double> positive;
  std::vector<double> negative;

  for (auto &i : vec) {
    auto tmp = static_cast<uint64_t>(i);
    if ((tmp & mask) != 0) {
      negative.push_back(i);
    } else {
      positive.push_back(i);
    }
  }

  auto sortNegative = std::async(std::launch::async, [&]() {
    std::vector<std::vector<double>> partsNegative(256);
    sortPartStl(partsNegative, negative);
  });

  auto sortPositive = std::async(std::launch::async, [&]() {
    std::vector<std::vector<double>> partsPositive(256);
    sortPartStl(partsPositive, positive);
  });

  sortNegative.wait();
  sortPositive.wait();

  vec.clear();
  vec.reserve(negative.size() + positive.size());
  vec.insert(vec.end(), negative.begin(), negative.end());
  vec.insert(vec.end(), positive.begin(), positive.end());

  return vec;
}

std::vector<double> randomVector(int size, double min, double max) {
  std::random_device rd;
  std::default_random_engine gen{rd()};
  std::uniform_real_distribution<double> random(min, max);
  std::vector<double> randVec(size);

  for (double &value : randVec) value = random(gen);

  return randVec;
}
}  // namespace SmirnovPavelStl
