#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

#ifndef RESOURCE_LEVELING_H
#define RESOURCE_LEVELING_H

struct Assignment {
  int machine_id;
  int start_time;
};

class SchedulingProblem {
  int machines;
  std::vector<int> durations;
  std::vector<int> resources;
  std::function<double(int)> costfunc;

public:
  SchedulingProblem(int machines, const std::vector<int> &durations,
                    const std::vector<int> &resources,
                    std::function<double(int)> costfunc);

  void print();
  std::vector<int> sort_by_key(std::vector<int> key, std::string dir);
  std::vector<Assignment> greedy_solve();
  std::vector<Assignment> simple_solve();
  std::vector<Assignment> solve();
  friend struct Solution;
};

struct Solution {
  std::vector<Assignment> schedule;
  std::map<int, int> events;
  int T;
  double cost;
  void print();
  Solution();
  Solution(const SchedulingProblem &problem,
           const std::vector<Assignment> &schedule);
};

void export_csv(const Solution &sol, const std::vector<int> &durations,
                const std::vector<int> &resources, int test_number);
#endif
