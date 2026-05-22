#include "resource_leveling.h"
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

SchedulingProblem::SchedulingProblem(int machines,
                                     const std::vector<int> &durations,
                                     const std::vector<int> &resources,
                                     std::function<double(int)> costfunc)
    : machines(machines), durations(durations), resources(resources),
      costfunc(costfunc) {}

void SchedulingProblem::print() {
  std::cout << "Problem:\n";
  std::cout << "Machines: " << machines << "\n";
  for (int j = 0; j < static_cast<int>(durations.size()); ++j) {
    std::cout << "Job " << j << ", Duration: " << durations[j] << ", Resource "
              << resources[j] << "\n";
  }
}

std::vector<int> SchedulingProblem::sort_by_key(std::vector<int> key,
                                                std::string dir) {
  int n = key.size();

  std::vector<int> idx(n);
  std::iota(idx.begin(), idx.end(), 0);

  if (dir == "asc") {
    std::sort(idx.begin(), idx.end(),
              [&](int i, int j) { return key[i] < key[j]; });
  } else if (dir == "desc") {
    std::sort(idx.begin(), idx.end(),
              [&](int i, int j) { return key[i] > key[j]; });
  }

  return idx;
}
std::vector<Assignment> SchedulingProblem::simple_solve() {

  int n = durations.size();
  std::vector<Assignment> sol(n);
  std::vector<int> order = sort_by_key(resources, "desc");
  int T = (n + 1) / 2;

  for (int k = 0; k < T; ++k) {
    int j1 = order[k];
    int j2 = -1;
    if (n % 2 == 1 && k > 0) {
      j2 = order[n - k];
    } else {
      j2 = order[n - k - 1];
    }
    if (j2 == -1) {
      sol[j1] = {0, k};
    } else {
      sol[j1] = {0, k};
      sol[j2] = {1, k};
    }
  }

  return sol;
}

std::vector<Assignment> SchedulingProblem::greedy_solve() {

  int n = durations.size();

  std::vector<Assignment> sol(n);

  std::vector<int> machine_available(machines, 0);
  std::vector<int> profile;
  std::vector<int> order = sort_by_key(durations, "desc");

  for (int k = 0; k < n; ++k) {
    int j = order[k];
    int best_machine = 0;
    int best_start = machine_available[0];

    for (int m = 0; m < machines; ++m) {

      if (machine_available[m] < best_start) {
        best_machine = m;
        best_start = machine_available[m];
      }
    }

    sol[j] = {best_machine, best_start};
    machine_available[best_machine] += durations[j];
    int end = best_start + durations[j];

    if (end > static_cast<int>(profile.size()))
      profile.resize(end, 0);
  }

  return sol;
}

std::vector<Assignment> SchedulingProblem::solve() {
  bool one_duration_jobs = true;
  int n = durations.size();
  int j = 0;
  while (j < n) {
    if (durations[j] == 1) {
      j++;
    } else {
      one_duration_jobs = false;
      break;
    }
  }

  if (one_duration_jobs && machines == 2) {
    std::cout << "The solution is optimal" << '\n';
    return SchedulingProblem::simple_solve();
  } else {
    return SchedulingProblem::greedy_solve();
  }
}

Solution::Solution(const SchedulingProblem &problem,
                   const std::vector<Assignment> &schedule)
    : schedule(schedule), T(0), cost(0.0) {

  std::map<int, int> events;

  int n = schedule.size();

  for (int j = 0; j < n; ++j) {
    int start = schedule[j].start_time;
    int end = start + problem.durations[j];

    events[start] += problem.resources[j];
    events[end] -= problem.resources[j];

    T = std::max(T, end);
  }

  int current_load = 0;
  int current_time = 0;

  for (auto [time, delta] : events) {
    cost += (time - current_time) * problem.costfunc(current_load);
    current_load += delta;
    current_time = time;
  }
}

void export_csv(const Solution &sol, const std::vector<int> &durations,
                const std::vector<int> &resources, int test_number) {
  std::ofstream file("schedule" + std::to_string(test_number) + ".csv");

  file << "job;machine;start;end;resource\n";

  for (int j = 0; j < sol.schedule.size(); ++j) {
    int m = sol.schedule[j].machine_id;
    int s = sol.schedule[j].start_time;
    int e = s + durations[j];

    file << j << ";" << m << ";" << s << ";" << e << ";" << resources[j]
         << "\n";
  }
}

void Solution::print() {
  std::cout << "Schedule:\n";
  for (int j = 0; j < static_cast<int>(schedule.size()); ++j) {
    std::cout << "Job " << j << " -> Machine " << schedule[j].machine_id
              << ", Start time " << schedule[j].start_time << "\n";
  }

  std::cout << "\nTotal cost = " << cost << "\n";
  std::cout << "Project duration = " << T << "\n";
}
