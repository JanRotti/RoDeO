/*
 * RoDeO, a Robust Design Optimization Package
 *
 * Copyright (C) 2015-2023 Chair for Scientific Computing (SciComp), RPTU
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (nicolas.gauger@scicomp.uni-kl.de) or Dr. Emre Özkaya (emre.oezkaya@scicomp.uni-kl.de)
 *
 * Lead developer: Emre Özkaya (SciComp, RPTU)
 *
 * This file is part of RoDeO
 *
 * RoDeO is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * RoDeO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with RoDeO.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Emre Özkaya, (SciComp, RPTU)
 *
 *
 *
 */

#include <cassert>

#include "./INCLUDE/random_functions.hpp"
#include "../Auxiliary/INCLUDE/auxiliary_functions.hpp"

#define ARMA_DONT_PRINT_ERRORS
#include <armadillo>

using namespace arma;

int generateRandomInt(int a, int b){
	assert(b>a);
	return(rand() % (b-a)+a);

}


int generateRandomInt(uvec indices){
	unsigned int size = indices.size();
	assert(size>0);
	int randomIndex = generateRandomInt(0, size);

	return(indices(randomIndex));

}



double generateRandomDouble(double a, double b) {


	double random = ((double) rand()) / (double) RAND_MAX;
	double diff = b - a;
	double r = random * diff;
	return a + r;
}

mat generateRandomMatrix(unsigned int Nrows, vec lb, vec ub){

	assert(lb.size() == ub.size());
	unsigned int dim = lb.size();

	mat X(Nrows, dim, fill::zeros);

	for(unsigned int i=0; i<Nrows; i++) {

		X.row(i) = generateRandomVector<rowvec>(lb,ub);

	}

	return X;

}





/** generate a random number between xs and xe using the normal distribution
 *
 */
double generateRandomDoubleFromNormalDist(double xs, double xe, double sigma_factor){

	double sigma=fabs((xe-xs))/sigma_factor;
	double mu=(xe+xs)/2.0;

	if (sigma == 0.0) sigma=1.0;

	/* construct a trivial random generator engine from a time-based seed */
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);
	std::normal_distribution<double> distribution (mu,sigma);
	return distribution(generator);
}



