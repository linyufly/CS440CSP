// Author: Mingcheng Chen (linyufly@gmail.com)

#include "scheduler.h"

#include <cstdio>

#include <vector>

// const char *kInputFile = "data/smallScenario.txt";
// const char *kInputFile = "data/mediumScenario.txt";
// const char *kInputFile = "data/bigScenario_1.txt";
// const char *kInputFile = "data/bigScenario_2.txt";
// const char *kInputFile = "sampleScenario.txt";
// const char *kInputFile = "firstScenario.txt";
const char *kInputFile = "secondScenario.txt";
// const char *kInputFile = "thirdScenario.txt";
// const char *kInputFile = "fourthScenario.txt";
// const char *kInputFile = "input.txt";

void minimum_cost_test() {
  printf("minimum_cost_test {\n");

  FILE *fin = fopen(kInputFile, "r");

  int num_courses, c_min, c_max;
  fscanf(fin, "%d %d %d", &num_courses, &c_min, &c_max);

  std::vector<int> fall_prices(num_courses);
  std::vector<int> spring_prices(num_courses);
  std::vector<int> credits(num_courses);

  for (int course_id = 0; course_id < num_courses; course_id++) {
    fscanf(fin, "%d %d %d", &fall_prices[course_id],
                            &spring_prices[course_id],
                            &credits[course_id]);
  }

  std::vector<std::vector<int> > prerequisites(num_courses);

  for (int course_id = 0; course_id < num_courses; course_id++) {
    int num_prerequisites;
    fscanf(fin, "%d", &num_prerequisites);
    for (int count = 0; count < num_prerequisites; count++) {
      int prerequisite;
      fscanf(fin, "%d", &prerequisite);
      prerequisites[course_id].push_back(prerequisite - 1);
    }
  }

  std::vector<int> interesting_courses;
  int num_interesting_courses;
  fscanf(fin, "%d", &num_interesting_courses);
  interesting_courses.resize(num_interesting_courses);

  for (int count = 0; count < num_interesting_courses; count++) {
    fscanf(fin, "%d", &interesting_courses[count]);
    interesting_courses[count]--;
  }

  int budget;

  fscanf(fin, "%d", &budget);

  fclose(fin);

  printf("Finished reading the data.\n");

  std::vector<std::vector<int> > plan;

  Scheduler scheduler;
  int best_price = scheduler.minimum_cost(
      fall_prices, spring_prices, credits, prerequisites,
      interesting_courses, c_min, c_max, budget, &plan);

  printf("best_price = %d\n", best_price);

  for (int semester = 0;
       semester < static_cast<int>(plan.size()); semester++) {
    printf("Semester %d:", semester);

    for (std::vector<int>::iterator course_itr = plan[semester].begin();
         course_itr != plan[semester].end(); course_itr++) {
      printf(" %d", *course_itr + 1);
    }

    printf("\n");
  }

  for (int semester = 0;
       semester < static_cast<int>(plan.size()); semester++) {
    if (semester > 0) {
      printf(" ");
    }

    int sum = 0;
    for (std::vector<int>::iterator course_itr = plan[semester].begin();
         course_itr != plan[semester].end(); course_itr++) {
      if (semester % 2 == 0) {
        sum += fall_prices[*course_itr];
      } else {
        sum += spring_prices[*course_itr];
      }
    }

    printf("%d", sum);
  }
  printf("\n");
 
  printf("} minimum_cost_test\n");
}

int main() {
  minimum_cost_test();

  return 0;
}
