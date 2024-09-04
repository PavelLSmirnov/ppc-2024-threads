// Copyright 2024 Smirnov Pavel
#include "tbb/smirnov_p_sort_double_batcher/include/ops_tbb.hpp"

namespace SmirnovPavelTbb {

constexpr int sizeDouble = sizeof(double);

bool RadixSortDoubleBatcherSequential::pre_processing() {
  internal_order_test();
  // Init value for input and output
  arr = std::vector<double>(taskData->inputs_count[0]);

  auto* temp_arr = reinterpret_cast<double*>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    arr[i] = temp_arr[i];
  }
  return true;
}

bool RadixSortDoubleBatcherSequential::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == taskData->outputs_count[0];
}

bool RadixSortDoubleBatcherSequential::run() {
  internal_order_test();
  res = radixSortBatcherSeq(arr);
  return true;
}

bool RadixSortDoubleBatcherSequential::post_processing() {
  internal_order_test();
  std::copy(res.begin(), res.end(), reinterpret_cast<double*>(taskData->outputs[0]));
  return true;
}

std::vector<double> batchersMergeSeq(std::vector<std::vector<double>>& subvectors) {
  std::vector<double> merged;
  if (subvectors.empty()) {
    return merged;
  }

  std::vector<std::pair<double, int>> indexedValues;
  for (size_t i = 0; i < subvectors.size(); ++i) {
    for (const auto& val : subvectors[i]) {
      indexedValues.emplace_back(val, i);
    }
  }

  std::sort(indexedValues.begin(), indexedValues.end());
  merged.resize(indexedValues.size());

  for (size_t i = 0; i < indexedValues.size(); ++i) {
    merged[i] = indexedValues[i].first;
  }

  return merged;
}

void partSortSeq(std::vector<std::vector<double>>& parts, std::vector<double>& side) {
  for (int i = 0; i < sizeDouble; ++i) {
    for (auto& j : side) {
      auto temp = static_cast<uint64_t>(j);
      temp >>= i * 8;
      temp &= 255;
      parts[temp].push_back(j);
    }

    side = batchersMergeSeq(parts);

    for (auto& part : parts) {
      part.clear();
    }
  }
}

std::vector<double> radixSortBatcherSeq(std::vector<double> vec) {
  uint64_t mask = static_cast<uint64_t>(1) << (sizeDouble * 8 - 1);
  std::vector<double> positive;
  std::vector<double> negative;

  for (auto& i : vec) {
    auto temp = static_cast<uint64_t>(i);

    if ((temp & mask) != 0) {
      negative.push_back(i);
    } else {
      positive.push_back(i);
    }
  }

  std::vector<std::vector<double>> parts(256);
  partSortSeq(parts, negative);
  partSortSeq(parts, positive);

  vec.clear();
  vec.reserve(negative.size() + positive.size());
  vec.insert(vec.end(), negative.begin(), negative.end());
  vec.insert(vec.end(), positive.begin(), positive.end());

  return vec;
}

bool RadixSortDoubleBatcherTbbParallel::pre_processing() {
  internal_order_test();
  // Init value for input and output
  arr = std::vector<double>(taskData->inputs_count[0]);

  auto* temp_arr = reinterpret_cast<double*>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    arr[i] = temp_arr[i];
  }
  return true;
}

bool RadixSortDoubleBatcherTbbParallel::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == taskData->outputs_count[0];
}

bool RadixSortDoubleBatcherTbbParallel::run() {
  internal_order_test();
  res = radixSortBatcherTbb(arr);
  return true;
}

bool RadixSortDoubleBatcherTbbParallel::post_processing() {
  internal_order_test();
  std::copy(res.begin(), res.end(), reinterpret_cast<double*>(taskData->outputs[0]));
  return true;
}

std::vector<double> batchersMergeTbb(std::vector<std::vector<double>>& subvectors) {
  std::vector<double> merged;
  if (subvectors.empty()) {
    return merged;
  }

  std::vector<std::pair<double, int>> indexedValues;
  for (size_t i = 0; i < subvectors.size(); ++i) {
    for (const auto& val : subvectors[i]) {
      indexedValues.emplace_back(val, i);
    }
  }

  std::sort(indexedValues.begin(), indexedValues.end());
  merged.resize(indexedValues.size());

  for (size_t i = 0; i < indexedValues.size(); ++i) {
    merged[i] = indexedValues[i].first;
  }

  return merged;
}

void partSortTbb(std::vector<std::vector<double>>& parts, std::vector<double>& side) {
  for (int i = 0; i < sizeDouble; ++i) {
    for (auto& j : side) {
      auto temp = static_cast<uint64_t>(j);
      temp >>= i * 8;
      temp &= 255;
      parts[temp].push_back(j);
    }

    side = batchersMergeTbb(parts);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, parts.size()), [&](const tbb::blocked_range<size_t>& r) {
      for (size_t j = r.begin(); j != r.end(); ++j) {
        parts[j].clear();
      }
    });
  }
}

std::vector<double> radixSortBatcherTbb(std::vector<double> vec) {
  uint64_t mask = static_cast<uint64_t>(1) << (sizeof(double) * 8 - 1);
  std::vector<double> positive;
  std::vector<double> negative;

  for (auto& i : vec) {
    auto temp = static_cast<uint64_t>(i);
    if ((temp & mask) != 0) {
      negative.push_back(i);
    } else {
      positive.push_back(i);
    }
  }

  tbb::parallel_invoke(
      [&]() {
        std::vector<std::vector<double>> partsNegative(256);
        partSortTbb(partsNegative, negative);
      },
      [&]() {
        std::vector<std::vector<double>> partsPositive(256);
        partSortTbb(partsPositive, positive);
      });

  vec.clear();
  vec.reserve(negative.size() + positive.size());
  vec.insert(vec.end(), negative.begin(), negative.end());
  vec.insert(vec.end(), positive.begin(), positive.end());

  return vec;
}

std::vector<double> randomVector(int sizeVec, double minValue, double maxValue) {
  std::random_device rd;
  std::default_random_engine gen{rd()};
  std::uniform_real_distribution<double> random(minValue, maxValue);

  std::vector<double> vec(sizeVec);

  for (double& value : vec) value = random(gen);

  return vec;
}
}  // namespace SmirnovPavelTbb
