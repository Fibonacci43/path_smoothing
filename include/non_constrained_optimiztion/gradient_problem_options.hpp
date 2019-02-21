//
// Created by yangt on 18-12-24.
//

#ifndef PATH_SMOOTHING_GRADIENT_PROBLEM_OPTIONS_HPP
#define PATH_SMOOTHING_GRADIENT_PROBLEM_OPTIONS_HPP

#include "type.hpp"

namespace ncopt {

class BasicOption {
 public:
  BasicOption() {
      minimizer_type = LINE_SEARCH;
      function_tolerance = 1e-6;
      gradient_norm_tolerance = 1e-6;
      gradine_norm_threshold = 1e-3;
      max_solve_iterations_num = 50;
  }
  MinimizerType minimizer_type;
  double function_tolerance;
  double gradient_norm_tolerance;
  double gradine_norm_threshold;
  int max_solve_iterations_num;
};

class LineSearchOption {
 public:
  LineSearchType line_search_type = WOLFE;
  LineSearchInterpolationType interpolation_type = QUADRATIC;
  LineSearchDirectionType line_search_direction_type = //STEEPEST_DESCENT;
         NONLINEAR_CONJUGATE_GRADIENT;
  NonlinearConjugateGradientType nonlinear_conjugate_gradient_type =
          FR_PR;
  double sufficient_decrease = 1e-4;
  double sufficient_curvature_decrease = 0.4;
  double min_line_search_step_length = 1e-16;
  double max_step_decrease_rate = 1e-3;
  double min_step_decrease_rate = 0.9;
};

class GradientProblemOption : public BasicOption, public LineSearchOption {

};

}

#endif //PATH_SMOOTHING_GRADIENT_PROBLEM_OPTIONS_HPP
