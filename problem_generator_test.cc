// Author: Mingcheng Chen (linyufly@gmail.com)

#include "problem_generator.h"

#include <cstdio>

#include <vector>

const char *kOutputFile = "input.txt";
// const char *kOutputFile = "sampleScenario.txt";
// const char *kOutputFile = "bigScenario.txt";

void generate_test() {
  printf("generate_test {\n");

  std::vector<int> fall_prices, spring_prices, credits;
  std::vector<int> interesting_courses;
  std::vector<std::vector<int> > prerequisites;

  ProblemGenerator generator;
  generator.generate(
      40, 10, 10, 20, 8, 1000, 0.1,
      &fall_prices, &spring_prices, &credits,
      &interesting_courses, &prerequisites);

  generator.write_file(
      kOutputFile, fall_prices, spring_prices, credits,
      prerequisites, interesting_courses, 10, 20);

  printf("} generate_test\n\n");
}

int main() {
  generate_test();

  return 0;
}
