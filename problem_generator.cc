// Author: Mingcheng Chen (linyufly@gmail.com)

#include "problem_generator.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <vector>

namespace {

double rand_double() {
  return static_cast<double>(rand()) / INT_MAX;
}

}

void ProblemGenerator::generate(
    int num_courses,
    int num_interesting_courses,
    int c_min, int c_max,
    int max_credit,
    int max_price,
    double dependency_rate,
    std::vector<int> *fall_prices,
    std::vector<int> *spring_prices,
    std::vector<int> *credits,
    std::vector<int> *interesting_courses,
    std::vector<std::vector<int> > *prerequisites) {
  srand(time(0));

  std::vector<int> courses(num_courses);
  for (int index = 0; index < num_courses; index++) {
    courses[index] = index;
  }

  random_shuffle(courses.begin(), courses.end());

  interesting_courses->clear();
  for (int index = 0; index < num_interesting_courses; index++) {
    interesting_courses->push_back(courses[index]);
  }

  fall_prices->resize(num_courses);
  spring_prices->resize(num_courses);
  credits->resize(num_courses);
  for (int course_id = 0; course_id < num_courses; course_id++) {
    (*fall_prices)[course_id] = rand() % max_price + 1;
    (*spring_prices)[course_id] =
        max_price + 1 - (*fall_prices)[course_id];  // rand() % max_price + 1;
    (*credits)[course_id] = rand() % max_credit + 1;
  }

  prerequisites->clear();
  prerequisites->resize(num_courses);

  random_shuffle(courses.begin(), courses.end());

  for (int curr_index = 1; curr_index < num_courses; curr_index++) {
    for (int prev_index = 0; prev_index < curr_index; prev_index++) {
      if (rand_double() < dependency_rate) {
        (*prerequisites)[courses[curr_index]].push_back(courses[prev_index]);
      }
    }
  }
}

void ProblemGenerator::write_file(
    const char *file_name,
    const std::vector<int> &fall_prices,
    const std::vector<int> &spring_prices,
    const std::vector<int> &credits,
    const std::vector<std::vector<int> > &prerequisites,
    const std::vector<int> &interesting_courses,
    int c_min, int c_max) {
  FILE *fout = fopen(file_name, "w");

  int num_courses = static_cast<int>(fall_prices.size());
  fprintf(fout, "%d %d %d\n", num_courses, c_min, c_max);

  for (int course_id = 0; course_id < num_courses; course_id++) {
    fprintf(fout, "%d %d %d\n",
            fall_prices[course_id], spring_prices[course_id],
            credits[course_id]);
  }

  for (int course_id = 0; course_id < num_courses; course_id++) {
    fprintf(fout, "%d", static_cast<int>(prerequisites[course_id].size()));

    for (std::vector<int>::const_iterator course_itr =
         prerequisites[course_id].begin();
         course_itr != prerequisites[course_id].end();
         course_itr++) {
      fprintf(fout, " %d", *course_itr);
    }

    fprintf(fout, "\n");
  }

  fprintf(fout, "%d", static_cast<int>(interesting_courses.size()));
  for (std::vector<int>::const_iterator course_itr =
       interesting_courses.begin();
       course_itr != interesting_courses.end();
       course_itr++) {
    fprintf(fout, " %d", *course_itr);
  }

  fprintf(fout, "\n");

  fprintf(fout, "-1\n");

  fclose(fout);
}

