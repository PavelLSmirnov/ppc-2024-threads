// Copyright 2024 Kosarev Egor
#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "core/task/include/task.hpp"

struct Point {
  int x, y;

  bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

class TestTaskSequentialKosarevJarvisHull : public ppc::core::Task {
 public:
  explicit TestTaskSequentialKosarevJarvisHull(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<Point> points;
  std::vector<Point> pointsHull;
};

class TestOMPTaskParallelKosarevJarvisHull : public ppc::core::Task {
 public:
  explicit TestOMPTaskParallelKosarevJarvisHull(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<Point> points;
  std::vector<Point> pointsHull;
};

int orientation(const Point& p, const Point& q, const Point& r);

double distance(const Point& p1, const Point& p2);

std::vector<Point> JarvisAlgo(const std::vector<Point>& arrPoints);

std::vector<Point> JarvisAlgo_omp(const std::vector<Point>& arrPoints, int threadsNom);