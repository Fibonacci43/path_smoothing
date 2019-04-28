//
// Created by yangt on 19-2-25.
//


#include "path_smoothing/path_smoothing.hpp"

namespace path_smoothing {

template<>
double &PathSmoothing::getX(geometry_msgs::Pose &point) {
    return point.position.x;
}

template<>
double &PathSmoothing::getX(hmpl::Circle &point) {
    return point.position.x;
}

template<>
double &PathSmoothing::getY(geometry_msgs::Pose &point) {
    return point.position.y;
}

template<>
double &PathSmoothing::getY(hmpl::Circle &point) {
    return point.position.y;
}

template<>
const double &PathSmoothing::getX(const geometry_msgs::Pose &point) {
    return point.position.x;
}

template<>
const double &PathSmoothing::getX(const hmpl::Circle &point) {
    return point.position.x;
}

template<>
const double &PathSmoothing::getY(const geometry_msgs::Pose &point) {
    return point.position.y;
}

template<>
const double &PathSmoothing::getY(const hmpl::Circle &point) {
    return point.position.y;
}

PathSmoothing::PathSmoothing(const int path_size) : path_size_(path_size) {

}

CgSmoothing::CgSmoothing(const Options &options,
                         const ncopt::Vector &start,
                         const ncopt::Vector &end,
                         const CgSmoothingFunction::Vector &param)
        : params_(param),
          PathSmoothing(param.rows() / 2 + 2) {
    settings_.heading_term_coe = options.cg_heading_term_coe;
    settings_.curvature_term_coe = options.cg_curvature_term_coe;
    settings_.obstacle_term_coe = options.cg_obstacle_term_coe;
    settings_.type = options.cg_difference_type;
    settings_.degree = 2;
    settings_.param_num = param.rows();
    settings_.start = start;
    settings_.end = end;
    settings_.function_ = options.function;
}

void CgSmoothing::smoothPath(const Options &options) {
    CgSmoothingFunction *smooth_function =
            CgSmoothingFunction::createCgSmoothingFunction(settings_,
                                                           params_);
    switch (options.cg_solver) {
        case CERES_SOLVER: {
            ceres::GradientProblemSolver::Options option;
            ceres::GradientProblemSolver::Summary summary;
            ceres::GradientProblem problem(smooth_function);
            option.logging_type = ceres::SILENT;
            option.nonlinear_conjugate_gradient_type = ceres::FLETCHER_REEVES;
            option.line_search_interpolation_type = ceres::QUADRATIC;
            option.line_search_type = ceres::WOLFE;
            option.line_search_sufficient_function_decrease = 1e-4;
            option.line_search_sufficient_curvature_decrease = 0.2;
//            option.min_line_search_step_contraction = 0.92;
            option.max_line_search_step_contraction = 1e-4;
            option.line_search_direction_type =
                    ceres::NONLINEAR_CONJUGATE_GRADIENT;//ceres::STEEPEST_DESCENT; //
            option.max_num_iterations = 20;
            ceres::Solve(option, problem, params_.data(), &summary);
            std::cout << summary.FullReport();
            break;
        }
        case SELF_SOLVER: {
            ncopt::GradientProblemOption solver_options;
            ncopt::Summary summarys;
            ncopt::GradientProblemSolver solver(smooth_function);
            solver.Solve(params_.data(), solver_options, &summarys);
            ncopt::Summary::PrintSummary(summarys);
            break;
        }
    }
}

double CgSmoothing::x(int i) const {
    if (i > 0 && i < pathSize() - 1) {
        return params_((i - 1) * settings_.degree);
    } else if (i == 0) {
        return settings_.start(0);
    } else if (i == pathSize() - 1) {
        return settings_.end(0);
    }
}

double CgSmoothing::y(int i) const {
    if (i > 0 && i < pathSize() - 1) {
        return params_((i - 1) * settings_.degree + 1);
    } else if (i == 0) {
        return settings_.start(1);
    } else if (i == pathSize() - 1) {
        return settings_.end(1);
    }
}

void NonDerivativeSmoothing::smoothPath(const Options &options) {
    this->optimizePathLength();
}

#ifdef GPMP2_SMOOTHING_ENABLE
void GpSmoothing::smoothPath(const Options &options) {
    switch (options.gp_solver) {
        case GAUSS_NEWTON: {
            gtsam::GaussNewtonParams params;
            params.setVerbosity("SILENT");
            gtsam::GaussNewtonOptimizer
                    optimizer(graph_, initial_guess, params);
            optimizer.optimize();
            result_ = optimizer.values();
            return;
        }
        case LEVENBERG_MARQUARDT: {
            gtsam::LevenbergMarquardtParams params;
            params.setVerbosity("SILENT");
            gtsam::LevenbergMarquardtOptimizer
                    optimizer(graph_, initial_guess, params);
            optimizer.optimize();
            result_ = optimizer.values();
            return;
        }
        case DOGLEG: {
            gtsam::DoglegParams params;
            params.setVerbosity("SILENT");
            gtsam::DoglegOptimizer optimizer(graph_, initial_guess, params);
            optimizer.optimize();
            result_ = optimizer.values();
        }
    }
}

double GpSmoothing::x(int i) const {
    return result_.at<gtsam::Pose2>(i).x();
}
double GpSmoothing::y(int i) const {
    return result_.at<gtsam::Pose2>(i).y();
}
#endif
}