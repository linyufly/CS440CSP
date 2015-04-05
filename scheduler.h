// Author: Mingcheng Chen (linyufly@gmail.com)

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <vector>

class Scheduler {
 public:
  // Courses are numbered from 0.
  // If budget equals -1, it means the budget is unlimited.
  int minimum_cost(const std::vector<int> &fall_prices,
                   const std::vector<int> &spring_prices,
                   const std::vector<int> &credits,
                   const std::vector<std::vector<int> > &prerequisites,
                   const std::vector<int> &interesting_courses,
                   int c_min, int c_max, int budget,
                   std::vector<std::vector<int> > *plan);

 private:
  void explore_in_dfs(
    int candidate, int current_semester,
    int candidate_id, int c_min, int c_max,
    int last_semester_credits_so_far,
    int num_remaining_required, int cost_so_far,
    const std::vector<int> &current_prices,
    const std::vector<int> &other_prices,
    const std::vector<int> &credits,
    const std::vector<int> &required_courses,
    const std::vector<int> &current_order,
    const std::vector<int> &other_order,
    const std::vector<std::vector<int> > &dependents,
    const std::vector<bool> &required,
    std::vector<int> *semester_taken,
    std::vector<int> *num_remaining_prerequisites,
    std::vector<int> *last_semester_courses,
    std::vector<std::vector<int> > *plan,
    int *best_price);

  void depth_first_search(
    const std::vector<int> &current_prices,
    const std::vector<int> &other_prices,
    const std::vector<int> &credits,
    const std::vector<int> &required_courses,
    const std::vector<std::vector<int> > &dependents,
    const std::vector<bool> &required,
    const std::vector<int> &current_order,
    const std::vector<int> &other_order,
    int last_selected,
    int c_min, int c_max,
    int num_remaining_required,
    int cost_so_far, int last_semester_credits_so_far,
    int current_semester,
    std::vector<int> *num_remaining_prerequisites,
    std::vector<int> *semester_taken,
    std::vector<int> *last_semester_courses,
    int *best_price,
    std::vector<std::vector<int> > *plan);

  int num_states_;
};

#endif  // SCHEDULER_H_
