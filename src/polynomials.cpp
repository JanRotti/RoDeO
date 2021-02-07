/*
 * RoDeO, a Robust Design Optimization Package
 *
 * Copyright (C) 2015-2020 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (nicolas.gauger@scicomp.uni-kl.de) or Dr. Emre Özkaya (emre.oezkaya@scicomp.uni-kl.de)
 *
 * Lead developer: Emre Özkaya (SciComp, TU Kaiserslautern)
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
 * General Public License along with CoDiPack.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Emre Özkaya, (SciComp, TU Kaiserslautern)
 *
 *
 *
 */

#include "polynomials.hpp"
#include "auxiliary_functions.hpp"
#include <cassert>


Polynomial::Polynomial(){

	degree = 0;

}
Polynomial::Polynomial(unsigned int deg){

	degree = deg;
	coefficients = zeros<vec>(degree+1);

}

double Polynomial::evaluate(double x) const{

	double sum = 0.0;

	for(unsigned int i=0; i<degree+1; i++){

		sum += coefficients(i)*pow(x,i);
	}

	return sum;

}

double Polynomial::differentiate(double x) const{

	double sum = 0.0;

	for(unsigned int i=1; i<degree+1; i++){

		sum += i*coefficients(i)*pow(x,i-1);
	}

	return sum;


}

void Polynomial::initializeRandomCoefficients(void){

	coefficients = randn<vec>(degree+1);

}


void Polynomial::print(void) const{

	cout<<"P(x):"<<coefficients(0)<<" + ";

	for(unsigned int i=1; i<degree; i++){

		cout<<coefficients(i)<<"*x^"<<i<<" + ";

	}
	cout<<coefficients(degree)<<"*x^"<<degree<<"\n";

}


PolynomialProduct::PolynomialProduct(){}

PolynomialProduct::PolynomialProduct(unsigned int size, unsigned int degree){

	for(unsigned int i=0; i<size; i++){

		Polynomial p(degree);
		polynomials.push_back(p);

	}


}

double PolynomialProduct::evaluate(const rowvec &x) const{

	assert(polynomials.size() == x.size());

	double prod = 1.0;
	for(unsigned int j=0; j<x.size(); j++){

		double fEval = polynomials[j].evaluate(x(j));
		prod = prod * fEval;
	}

	return prod;

}

double PolynomialProduct::differentiate(const rowvec &x, unsigned int indx) const{

	assert(polynomials.size() == x.size());

	double prod = 1.0;
	for(unsigned int j=0; j<x.size(); j++){
		double fEval = 0.0;
		if(indx == j){

			fEval = polynomials[j].differentiate(x(j));
		}
		else{

			fEval = polynomials[j].evaluate(x(j));
		}

		prod = prod * fEval;

	}

	return prod;

}


void PolynomialProduct::print(void) const{

	for(auto it=polynomials.begin(); it != polynomials.end(); ++it){

		it->print();
	}

}

void PolynomialProduct::initializeRandomCoefficients(void){

	for(auto it=polynomials.begin(); it != polynomials.end(); ++it){

		it->initializeRandomCoefficients();
	}


}

/* This function generates a test data for the aggregation model using N data points with "dim" dimensional random
 * data. Functional values are generated by a randomized polynomial of degree "dim".
 */

void generateRandomTestAndValidationDataForGradientModels(int dim, int N){


	cout<<"Generating a test problem with gradients (N="<<N<<", dim="<<dim<<")\n";

	/* 20 extra samples are for validation purposes */
	int Nval = 20;

	int degree = generateRandomInt(2,5);

	mat testData(N,2*dim+1,fill::zeros);
	mat validationData(Nval,dim+1,fill::zeros);


	mat X(N,dim,fill::randu);
	vec y(N, fill::zeros);
	mat derivatives(N,dim, fill::zeros);

	PolynomialProduct randomPolynomialsProduct(dim,degree);
	randomPolynomialsProduct.initializeRandomCoefficients();



	/* first primal values are calculated
	 *
	 * Product = P0(x0)*P1(x1)*P2(x2)* ... *Pdim(xdim)
	 *
	 * */
	for(int i=0; i<N; i++){

		rowvec x = X.row(i);

		y(i) = randomPolynomialsProduct.evaluate(x);
	}

	/* gradients are calculated
	 *
	 * e.g., d Product/dx1 = dP(x0)*dP1(x1)*P2(x2)* ... *Pdim(xdim)
	 *
	 * */

	for(int i=0; i<N; i++){

		rowvec x = X.row(i);


		for(int j=0; j<dim; j++){

			derivatives(i,j) = randomPolynomialsProduct.differentiate(x,j);
		}

	}

	for(int i=0; i<N; i++){

		for(int j=0; j<dim; j++){

			testData(i,j) = X(i,j);
		}

		testData(i,dim) = y(i);

		for(int j=dim+1; j<2*dim+1; j++){

			testData(i,j) = derivatives(i,j-dim-1);
		}

	}

	mat Xval(Nval,dim,fill::randu);
	vec yval(Nval, fill::zeros);

	for(int i=0; i<Nval; i++){

		rowvec x = Xval.row(i);

		yval(i) = randomPolynomialsProduct.evaluate(x);
	}

	for(int i=0; i<Nval; i++){

		for(int j=0; j<dim; j++){

			validationData(i,j) = Xval(i,j);
		}

		validationData(i,dim) = yval(i);

	}

	validationData.save("testDataValidationPoints.csv",arma::csv_ascii);
	testData.save("testData.csv",arma::csv_ascii);

}




