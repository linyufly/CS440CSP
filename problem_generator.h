// Author: Mingcheng Chen (linyufly@gmail.com)

#ifndef PROBLEM_GENERATOR_H_
#define PROBLEM_GENERATOR_H_

#include <vector>

class ProblemGenerator {
 public:
  void generate(
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
      std::vector<std::vector<int> > *prerequisites);

  void write_file(const char *file_name,
                  const std::vector<int> &fall_prices,
                  const std::vector<int> &spring_prices,
                  const std::vector<int> &credits,
                  const std::vector<std::vector<int> > &prerequisites,
                  const std::vector<int> &interesting_courses,
                  int c_min, int c_max);
};

#endif  // PROBLEM_GENERATOR_H_
