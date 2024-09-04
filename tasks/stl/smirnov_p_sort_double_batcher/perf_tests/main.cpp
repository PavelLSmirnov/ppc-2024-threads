// Copyright 2024 Smirnov Pavel
#include <gtest/gtest.h>

#include "core/perf/include/perf.hpp"
#include "stl/smirnov_p_sort_double_batcher/include/ops_stl.hpp"

TEST(smirnov_p_sort_double_batcher, test_pipeline_run) {
  // Create data
  std::vector<double> array = SmirnovPavelStl::randomVector(1000000, -1000000, 1000000);
  std::vector<double> out(array.size());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> RSDBDataStl = std::make_shared<ppc::core::TaskData>();
  RSDBDataStl->inputs.emplace_back(reinterpret_cast<uint8_t *>(array.data()));
  RSDBDataStl->inputs_count.emplace_back(array.size());
  RSDBDataStl->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  RSDBDataStl->outputs_count.emplace_back(out.size());

  std::vector<double> result(array);
  std::sort(result.begin(), result.end());

  // Create Task
  auto testRSDBstlParallel = std::make_shared<SmirnovPavelStl::RadixSortDoubleBatcherStlParallel>(RSDBDataStl);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testRSDBstlParallel);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  for (size_t i = 0; i < result.size(); ++i) {
    ASSERT_EQ(result[i], out[i]);
  }
}

TEST(smirnov_p_sort_double_batcher, test_task_run) {
  // Create data
  std::vector<double> array = SmirnovPavelStl::randomVector(1000000, -1000000, 1000000);
  std::vector<double> out(array.size());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> RSDBDataStl = std::make_shared<ppc::core::TaskData>();
  RSDBDataStl->inputs.emplace_back(reinterpret_cast<uint8_t *>(array.data()));
  RSDBDataStl->inputs_count.emplace_back(array.size());
  RSDBDataStl->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  RSDBDataStl->outputs_count.emplace_back(out.size());

  std::vector<double> result(array);
  std::sort(result.begin(), result.end());

  // Create Task
  auto testRSDBstlParallel = std::make_shared<SmirnovPavelStl::RadixSortDoubleBatcherStlParallel>(RSDBDataStl);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testRSDBstlParallel);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  for (size_t i = 0; i < result.size(); ++i) {
    ASSERT_EQ(result[i], out[i]);
  }
}
