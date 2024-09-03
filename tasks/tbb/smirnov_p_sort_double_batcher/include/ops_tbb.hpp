// Copyright 2024 Smirnov Pavel
#pragma once

#include <tbb/tbb.h>

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

#include "core/task/include/task.hpp"

namespace SmirnovPavelTbb {
class RadixSortDoubleBatcherSequential : public ppc::core::Task {
 public:
  explicit RadixSortDoubleBatcherSequential(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}

  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<double> arr;
  std::vector<double> res;
};

class RadixSortDoubleBatcherTbbParallel : public ppc::core::Task {
 public:
  explicit RadixSortDoubleBatcherTbbParallel(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}

  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<double> arr;
  std::vector<double> res;
};

std::vector<double> batchersMergeSeq(std::vector<std::vector<double>>& subvectors);
void partSortSeq(std::vector<std::vector<double>>& parts, std::vector<double>& side);
std::vector<double> radixSortBatcherSeq(std::vector<double> v);

std::vector<double> batchersMergeTbb(std::vector<std::vector<double>>& subvectors);
void partSortTbb(std::vector<std::vector<double>>& parts, std::vector<double>& side);
std::vector<double> radixSortBatcherTbb(std::vector<double> v);

std::vector<double> randomVector(int sizeVec, double minValue, double maxValue);
}  // namespace SmirnovPavelTbb
