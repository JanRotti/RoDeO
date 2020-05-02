#ifndef TRAIN_LINREG_HPP
#define TRAIN_LINREG_HPP
#include <armadillo>
using namespace arma;

void train_linear_regression(mat &X, vec &ys, vec &w, double lambda);

#endif
