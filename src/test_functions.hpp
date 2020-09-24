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
#ifndef TEST_FUNCTIONS_HPP
#define TEST_FUNCTIONS_HPP
#include "Rodeo_macros.hpp"
#include "surrogate_model.hpp"

#include <string>

#include <armadillo>



using namespace arma;


class TestFunction {

private:

	unsigned int numberOfInputParams;
	std::string function_name;

	vec lb; /*lower bounds */
	vec ub; /*upper bounds */

	SurrogateModel *surrogateModel;

	bool ifFunctionIsNoisy;

	bool ifVisualize;
	double noiseLevel;

public:

	short int numberOfSamplesUsedForVisualization;

    double (*func_ptr)(double *);
    double (*adj_ptr)(double *, double *);

    TestFunction(std::string name,int dimension);
    void plot(int resolution = 100) const;


    void addNoise(double);
    void setVisualizationOn(void);
    void setVisualizationOff(void);

    void visualizeSurrogate1D(SurrogateModel *TestFunSurrogate, unsigned int resolution=1000) const;
    void visualizeSurrogate2D(SurrogateModel *TestFunSurrogate, unsigned int resolution=100) const;

    void print(void);
    mat  generateRandomSamples(unsigned int howManySamples);
    mat  generateRandomSamplesWithGradients(unsigned int nsamples);

    mat  generateUniformSamples(unsigned int howManySamples) const;
    double testSurrogateModel(SURROGATE_MODEL modelId, unsigned int nsamples, bool warmStart = false);
    void testEfficientGlobalOptimization(int nsamplesTrainingData, int maxnsamples, bool ifVisualize = false, bool ifWarmStart = false, bool ifMinimize = true);

    void evaluateGlobalExtrema(void) const;
    void setBoxConstraints(double lowerBound, double upperBound);
    void setBoxConstraints(vec lowerBound, vec upperBound);


} ;







/* regression test functions */

double LinearTF1(double *x);
double LinearTF1Adj(double *x, double *xb);


double Eggholder(double *x);
double Eggholder_adj(double *x, double *xb);



double Rastrigin6D(double *x);
double Rastrigin6D_adj(double *xin, double *xb);



double Waves2D(double *x);
double Waves2D_adj(double *x,double *xb);

double Waves2Dperturbed(double *x);
double Waves2Dperturbed_adj(double *x,double *xb);

double Herbie2D(double *x);
double Herbie2DAdj(double *x, double *xb);


double McCormick(double *x);
double McCormick_adj(double *x, double *xb);

double Goldstein_Price(double *x);
double Goldstein_Price_adj(double *x, double *xb);

double Rosenbrock(double *x);
double Rosenbrock_adj(double *x, double *xb);


double Rosenbrock3D(double *x);
double Rosenbrock3D_adj(double *x, double *xb);

double Rosenbrock4D(double *x);
double Rosenbrock4D_adj(double *x, double *xb);



double Rosenbrock8D(double *x);
double Rosenbrock8D_adj(double *x, double *xb) ;

double Shubert(double *x);
double Shubert_adj(double *x, double *xb);

double Himmelblau(double *x);
double HimmelblauAdj(double *x, double *xb);



double Borehole(double *x);
double BoreholeAdj(double *x, double *xb);


double Wingweight(double *x);
double WingweightAdj(double *xin, double *xb);


double empty(double *x);
double emptyAdj(double *x, double *xb);







#endif
