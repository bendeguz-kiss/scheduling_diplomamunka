#include "resource_leveling.h"

double quadratic(int load) { return load * load; }

int main() {

  std::vector<int> durations = {1, 1, 1, 1, 1, 1};
  std::vector<int> resources = {2, 1, 10, 3, 2, 1};
  int machines_bound = 5;

  for (int machines = 1; machines <= machines_bound; ++machines) {
    SchedulingProblem problem(machines, durations, resources, quadratic);
    problem.print();
    std::vector<Assignment> schedules = problem.solve();
    Solution sol = Solution(problem, schedules);
    sol.print();
    export_csv(sol, durations, resources, machines);
  }

  return 0;
}
