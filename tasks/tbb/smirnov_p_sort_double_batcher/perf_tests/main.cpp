// Copyright 2024 Smirnov Pavel
#include <gtest/gtest.h>
#include <oneapi/tbb.h>

#include "core/perf/include/perf.hpp"
#include "tbb/smirnov_p_sort_double_batcher/include/ops_tbb.hpp"

TEST(smirnov_p_sort_double_batcher, test_pipeline_run) {
  // Create data
  std::vector<double> array = SmirnovPavelTbb::randomVector(1000000, -1000000, 1000000);
  std::vector<double> out(array.size());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> RSDBDataTbb = std::make_shared<ppc::core::TaskData>();
  RSDBDataTbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(array.data()));
  RSDBDataTbb->inputs_count.emplace_back(array.size());
  RSDBDataTbb->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  RSDBDataTbb->outputs_count.emplace_back(out.size());

  std::vector<double> result(array);
  std::sort(result.begin(), result.end());

  // Create Task
  auto testRSDBtbbParallel = std::make_shared<SmirnovPavelTbb::RadixSortDoubleBatcherTbbParallel>(RSDBDataTbb);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = oneapi::tbb::tick_count::now();
  perfAttr->current_timer = [&] { return (oneapi::tbb::tick_count::now() - t0).seconds(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testRSDBtbbParallel);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  for (size_t i = 0; i < result.size(); ++i) {
    ASSERT_EQ(result[i], out[i]);
  }
}

TEST(smirnov_p_sort_double_batcher, test_task_run) {
  // Create data
  std::vector<double> array = SmirnovPavelTbb::randomVector(1000000, -1000000, 1000000);
  std::vector<double> out(array.size());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> RSDBDataTbb = std::make_shared<ppc::core::TaskData>();
  RSDBDataTbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(array.data()));
  RSDBDataTbb->inputs_count.emplace_back(array.size());
  RSDBDataTbb->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  RSDBDataTbb->outputs_count.emplace_back(out.size());

  std::vector<double> result(array);
  std::sort(result.begin(), result.end());

  // Create Task
  auto testRSDBtbbParallel = std::make_shared<SmirnovPavelTbb::RadixSortDoubleBatcherTbbParallel>(RSDBDataTbb);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = oneapi::tbb::tick_count::now();
  perfAttr->current_timer = [&] { return (oneapi::tbb::tick_count::now() - t0).seconds(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testRSDBtbbParallel);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  for (size_t i = 0; i < result.size(); ++i) {
    ASSERT_EQ(result[i], out[i]);
  }
}
