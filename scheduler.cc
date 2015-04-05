// Author: Mingcheng Chen (linyufly@gmail.com)

#include "scheduler.h"

#include <cstdio>

#include <algorithm>
#include <vector>

namespace {

struct CourseComparator {
  const std::vector<int> &current_price_;
  const std::vector<int> &other_price_;
  const std::vector<bool> &required_;

  CourseComparator(const std::vector<int> &current_price,
                   const std::vector<int> &other_price,
                   const std::vector<bool> &required) :
      current_price_(current_price),
      other_price_(other_price),
      required_(required) {}

  bool operator () (int course_1, int course_2) const {
    // If one is required and the other is not required, the required one
    // should come first.
    if (required_[course_1] != required_[course_2]) {
      return required_[course_1];
    }

    int discount_1 = other_price_[course_1] - current_price_[course_1];
    int discount_2 = other_price_[course_2] - current_price_[course_2];

    // The course with larger discount in the current semester should come
    // first.
    return discount_1 > discount_2;
  }
};

void get_plan(const std::vector<int> &semester_taken,
              std::vector<std::vector<int> > *plan) {
  int num_courses = static_cast<int>(semester_taken.size());

  int num_semesters = 0;
  for (int course_id = 0; course_id < num_courses; course_id++) {
    if (semester_taken[course_id] > num_semesters) {
      num_semesters = semester_taken[course_id];
    }
  }

  num_semesters++;
  plan->clear();
  plan->resize(num_semesters);

  for (int course_id = 0; course_id < num_courses; course_id++) {
    if (semester_taken[course_id] != -1) {
      (*plan)[semester_taken[course_id]].push_back(course_id);
    }
  }
}

void update_dependents(int candidate, int delta,
                       const std::vector<std::vector<int> > &dependents,
                       std::vector<int> *num_remaining_prerequisites) {
  for (std::vector<int>::const_iterator course_itr =
       dependents[candidate].begin();
       course_itr != dependents[candidate].end();
       course_itr++) {
    (*num_remaining_prerequisites)[*course_itr] += delta;
  }
}

void invoke_selection(int candidate, int current_semester,
                      const std::vector<std::vector<int> > &dependents,
                      std::vector<int> *last_semester_courses,
                      std::vector<int> *semester_taken,
                      std::vector<int> *num_remaining_prerequisites) {
  last_semester_courses->push_back(candidate);
  (*semester_taken)[candidate] = current_semester;
}

void revoke_selection(int candidate,
                      const std::vector<std::vector<int> > &dependents,
                      std::vector<int> *last_semester_courses,
                      std::vector<int> *semester_taken,
                      std::vector<int> *num_remaining_prerequisites) {
  last_semester_courses->pop_back();
  (*semester_taken)[candidate] = -1;
}

void trace_prerequisites(int course_id,
                         const std::vector<std::vector<int> > &prerequisites,
                         std::vector<bool> *required) {
  (*required)[course_id] = true;

  for (std::vector<int>::const_iterator course_itr =
       prerequisites[course_id].begin();
       course_itr != prerequisites[course_id].end();
       course_itr++) {
    if (!(*required)[*course_itr]) {
      trace_prerequisites(*course_itr, prerequisites, required);
    }
  }
}

}

void Scheduler::explore_in_dfs(
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
    int *best_price) {
  if ((*semester_taken)[candidate] != -1
      || last_semester_credits_so_far + credits[candidate] > c_max
      || (*num_remaining_prerequisites)[candidate] > 0) {
    return;
  }

  invoke_selection(candidate, current_semester, dependents,
                   last_semester_courses, semester_taken,
                   num_remaining_prerequisites);

  int delta_required = required[candidate]? 1 : 0;

  depth_first_search(
      current_prices, other_prices, credits, required_courses, dependents,
      required, current_order, other_order, candidate_id, c_min, c_max,
      num_remaining_required - delta_required,
      cost_so_far + current_prices[candidate],
      last_semester_credits_so_far + credits[candidate], current_semester,
      num_remaining_prerequisites, semester_taken, last_semester_courses,
      best_price, plan);

  revoke_selection(candidate, dependents,
                   last_semester_courses, semester_taken,
                   num_remaining_prerequisites);
}

// Search rules:
//   1. For each semester, arrange required courses first;
//   2. Arrange courses cheaper in the current semester first
//      (sort by the discount);
//   3. In the current semester, if the minimum credit hours
//      are already taken, the option moving on to the next
//      semester comes before the option trying any non-required
//      courses or courses more expensive in this semester.
void Scheduler::depth_first_search(
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
    std::vector<std::vector<int> > *plan) {
  // Check whether the DFS has arrived at a solution.
  if (num_remaining_required == 0 && last_semester_credits_so_far >= c_min) {
    if (*best_price == -1 || cost_so_far < *best_price) {
      *best_price = cost_so_far;
      get_plan(*semester_taken, plan);

      /// DEBUG ///
      printf("new_best_price = %d, num_states = %d\n", *best_price, num_states_);
    }
    return;
  }

  // Check whether it is possible to beat the current best price.
  if (*best_price != -1) {
    int minimum_cost = 0;
    for (std::vector<int>::const_iterator course_itr =
         required_courses.begin();
         course_itr != required_courses.end();
         course_itr++) {
      if ((*semester_taken)[*course_itr] == -1) {
        minimum_cost +=
            std::min(current_prices[*course_itr], other_prices[*course_itr]);
      }
    }

    if (cost_so_far + minimum_cost >= *best_price) {
      return;
    }
  }

  // Try all the required courses which are cheaper in the current semester.
  int num_courses = static_cast<int>(current_prices.size());
  int candidate_id = last_selected + 1;

  for (; candidate_id < num_courses; candidate_id++) {
    int candidate = current_order[candidate_id];

    if (!required[candidate]
        || current_prices[candidate] >= other_prices[candidate]) {
      break;
    }

    num_states_++;

    explore_in_dfs(
        candidate, current_semester, candidate_id, c_min, c_max,
        last_semester_credits_so_far, num_remaining_required, cost_so_far,
        current_prices, other_prices, credits, required_courses,
        current_order, other_order, dependents, required, semester_taken,
        num_remaining_prerequisites, last_semester_courses, plan, best_price);
  }

  // If the minimum credits requirement is already satisfied in the current
  // semester, try to move on to the next semester.
  if (last_semester_credits_so_far >= c_min) {
    for (std::vector<int>::iterator course_itr =
         last_semester_courses->begin();
         course_itr != last_semester_courses->end();
         course_itr++) {
      update_dependents(
          *course_itr, -1, dependents, num_remaining_prerequisites);
    }

    std::vector<int> empty_course_list;

    depth_first_search(
        other_prices, current_prices, credits, required_courses, dependents,
        required, other_order, current_order, -1, c_min, c_max,
        num_remaining_required, cost_so_far, 0, current_semester + 1,
        num_remaining_prerequisites, semester_taken, &empty_course_list,
        best_price, plan);

    for (std::vector<int>::iterator course_itr =
         last_semester_courses->begin();
         course_itr != last_semester_courses->end();
         course_itr++) {
      update_dependents(
          *course_itr, 1, dependents, num_remaining_prerequisites);
    }
  }

  // Try all the required but more expensive courses, followed by non-required
  // courses sorted by the discount.
  for (; candidate_id < num_courses; candidate_id++) {
    int candidate = current_order[candidate_id];

    num_states_++;

    explore_in_dfs(
        candidate, current_semester, candidate_id, c_min, c_max,
        last_semester_credits_so_far, num_remaining_required, cost_so_far,
        current_prices, other_prices, credits, required_courses,
        current_order, other_order, dependents, required, semester_taken,
        num_remaining_prerequisites, last_semester_courses, plan, best_price);
  }
}

int Scheduler::minimum_cost(
    const std::vector<int> &fall_prices,
    const std::vector<int> &spring_prices,
    const std::vector<int> &credits,
    const std::vector<std::vector<int> > &prerequisites,
    const std::vector<int> &interesting_courses,
    int c_min, int c_max, int budget,
    std::vector<std::vector<int> > *plan) {
  int num_courses = static_cast<int>(fall_prices.size());

  printf("num_courses = %d\n\n", num_courses);

  for (int course_id = 0; course_id < num_courses; course_id++) {
    printf("Course %d: %d, %d, %d\n",
           course_id, fall_prices[course_id], spring_prices[course_id],
           credits[course_id]);
  }
  printf("\n");

  std::vector<std::vector<int> > dependents(num_courses);
  std::vector<int> num_remaining_prerequisites(num_courses);

  // Construct dependents and num_remaining_prerequisites.
  for (int course_id = 0; course_id < num_courses; course_id++) {
    int num_prerequisites = static_cast<int>(prerequisites[course_id].size());

    num_remaining_prerequisites[course_id] = num_prerequisites;

    for (int prerequisite_id = 0; prerequisite_id < num_prerequisites;
         prerequisite_id++) {
      int curr_prerequisite = prerequisites[course_id][prerequisite_id];
      dependents[curr_prerequisite].push_back(course_id);
    }
  }

  for (int course_id = 0; course_id < num_courses; course_id++) {
    printf("%d:", course_id);
    for (std::vector<int>::const_iterator course_itr =
         dependents[course_id].begin();
         course_itr != dependents[course_id].end();
         course_itr++) {
      printf(" %d", *course_itr);
    }
    printf("\n");
  }
  printf("\n");

  // Find all the required courses for interesting courses.
  std::vector<bool> required(num_courses);
  std::fill(required.begin(), required.end(), false);

  for (std::vector<int>::const_iterator course_itr =
       interesting_courses.begin();
       course_itr != interesting_courses.end();
       course_itr++) {
    if (!required[*course_itr]) {
      trace_prerequisites(*course_itr, prerequisites, &required);
    }
  }

  std::vector<int> required_courses;

  for (int course_id = 0; course_id < num_courses; course_id++) {
    if (required[course_id]) {
      required_courses.push_back(course_id);
    }
  }

  printf("required_courses:");
  for (std::vector<int>::const_iterator course_itr = required_courses.begin();
       course_itr != required_courses.end();
       course_itr++) {
    printf(" %d", *course_itr);
  }
  printf("\n\n");

  int num_required = static_cast<int>(required_courses.size());

  // Get the consideration order in Fall and Spring semesters.
  std::vector<int> fall_order(num_courses), spring_order(num_courses);
  for (int index = 0; index < num_courses; index++) {
    fall_order[index] = spring_order[index] = index;
  }

  std::sort(fall_order.begin(), fall_order.end(),
            CourseComparator(fall_prices, spring_prices, required));
  std::sort(spring_order.begin(), spring_order.end(),
            CourseComparator(spring_prices, fall_prices, required));

  printf("fall_order:");
  for (std::vector<int>::const_iterator course_itr = fall_order.begin();
       course_itr != fall_order.end();
       course_itr++) {
    printf(" %d", *course_itr);
  }
  printf("\n");
  printf("spring_order:");
  for (std::vector<int>::const_iterator course_itr = spring_order.begin();
       course_itr != spring_order.end();
       course_itr++) {
    printf(" %d", *course_itr);
  }
  printf("\n\n");

  // semester_taken records the semester taken for each courses.
  std::vector<int> semester_taken(num_courses);
  std::fill(semester_taken.begin(), semester_taken.end(), -1);

  // last_semester_courses records the courses for the current semester.
  std::vector<int> last_semester_courses;

  int best_price = (budget == -1) ? -1 : budget + 1;

  num_states_ = 1;

  depth_first_search(fall_prices, spring_prices,
                     credits, required_courses,
                     dependents, required,
                     fall_order, spring_order, -1,
                     c_min, c_max,
                     num_required, 0, 0, 0,
                     &num_remaining_prerequisites,
                     &semester_taken,
                     &last_semester_courses,
                     &best_price, plan);
 
  printf("overall_num_states = %d\n", num_states_);

  if (budget != -1 && best_price == budget + 1) {
    best_price = -1;
  }
 
  return best_price;
}

