
#include <stdio.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <stack>
#include "Rodeo_macros.hpp"
#include "Rodeo_globals.hpp"
#include "auxilliary_functions.hpp"
#include "test_functions.hpp"
#include "kriging_training.hpp"
#include "rbf.hpp"
#include "trust_region_gek.hpp"
#include "kernel_regression.hpp"
#include "kernel_regression_cuda.h"
#include <codi.hpp>

#define ARMA_DONT_PRINT_ERRORS
#include <armadillo>
using namespace arma;


void classification_test1(double *x, double *label, double *y){
	/* y = sin(x)+x-1+sin(2x) * (separating curve) */

	*y = sin(x[0])+x[0]-1+sin(2.0*x[0]);

	if (*y > x[1]) {

		*label=1.0;
	}
	else{

		*label=-1.0;
	}


}

/* lineary seperable simple test problem */

void classification_test2(double *x, double *label, double *y){
	/* y = sin(x)+x-1+sin(2x) * (separating curve) */

	*y = 0.5*x[0]+0.2;

	if (*y > x[1]) {

		*label=1.0;
	}
	else{

		*label=-1.0;
	}


}

double empty(double *x){

	return 0;

}

double empty(double *x, double *xb){

	return 0;

}


double test_function1KernelReg(double *x){

	return 0.001*sin(x[0])+ 1.0*cos(x[1]);

}

double test_function1KernelRegAdj(double *xin, double *xb){

	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();
	codi::RealReverse *x = new codi::RealReverse[2];
	x[0]= xin[0];
	x[1]= xin[1];
	tape.registerInput(x[0]);
	tape.registerInput(x[1]);

	codi::RealReverse result = 0.001*sin(x[0])+ 1.0*cos(x[1]);

	tape.registerOutput(result);

	tape.setPassive();
	result.setGradient(1.0);
	tape.evaluate();

	xb[0]=x[0].getGradient();
	xb[1]=x[1].getGradient();

	tape.reset();
	delete[] x;
	return result.getValue();

}


double test_function2KernelReg(double *x){

	return sin(x[1]+x[2])+ pow( (x[1]-x[2]),2.0)-1.5*x[1]+2.5*x[2]+1.0;

}

double test_function2KernelRegAdj(double *xin,double *xb){
	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();
	codi::RealReverse *x = new codi::RealReverse[5];

	for(int i=0; i<5;i++){

		x[i]= xin[i];
		tape.registerInput(x[i]);
	}
	codi::RealReverse result = sin(x[1]+x[2])+ pow( (x[1]-x[2]),2.0)-1.5*x[1]+2.5*x[2]+1.0;
	tape.registerOutput(result);

	tape.setPassive();
	result.setGradient(1.0);
	tape.evaluate();

	for(int i=0; i<5;i++){
		xb[i]=x[i].getGradient();

	}
	delete[] x;
	tape.reset();
	return result.getValue();

}



double test_function1D(double *x){

	return sin(2*x[0])+ 0.5* sin(10*x[0]) + x[0]*x[0] ;

}


double Griewank2D(double *x){

	return (x[0]*x[0])/4000.0 + (x[1]*x[1])/4000.0 - cos(x[0]) * cos(x[1]/sqrt(2.0));



}

double Griewank2D_adj(double *xin, double *xb){

	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();
	codi::RealReverse *x = new codi::RealReverse[2];

	for(int i=0; i<2;i++){

		x[i]= xin[i];
		tape.registerInput(x[i]);
	}

	codi::RealReverse result = (x[0]*x[0])/4000.0 + (x[1]*x[1])/4000.0 - cos(x[0]) * cos(x[1]/sqrt(2.0));
	tape.registerOutput(result);

	tape.setPassive();
	result.setGradient(1.0);
	tape.evaluate();

	for(int i=0; i<2;i++){
		xb[i]=x[i].getGradient();

	}
	delete[] x;
	tape.reset();
	return result.getValue();





}

double Waves2D(double *x){

	double coef = 0.001;
	double term1 = coef*sin(100*x[0]);
	double term2 = coef*cos(100*x[1]);

	if(x[0] > 1 || x[0] < -1 || x[1] > 1 || x[1] < -1){

		return sin(x[0])+ cos(x[1]);

	}
	else{

		return sin(x[0])+ cos(x[1])+term1+term2;
	}

}

double Waves2D_adj(double *x,double *xb){
	double coef = 0.001;
	double term1 = coef*sin(100*x[0]);
	double term2 = coef*cos(100*x[1]);

	if(x[0] > 1 || x[0] < -1 || x[1] > 1 || x[1] < -1){
		xb[0] = cos(x[0]);
		xb[1] = -sin(x[1]);
		return sin(x[0])+ cos(x[1]);
	}
	else{

		xb[0] = cos(x[0])+coef*100*cos(100*x[0]);
		xb[1] = -sin(x[1])-coef*100*sin(100*x[1]);

		return sin(x[0])+ cos(x[1])+term1+term2;
	}



}


double Herbie2D(double *x){

	return exp(-pow((x[0]-1),2))+exp(-0.8*pow((x[0]+1),2))-0.05*sin(8*(x[0]+0.1))
	+ exp(-pow((x[1]-1),2))+exp(-0.8*pow((x[1]+1),2))-0.05*sin(8*(x[1]+0.1));

}

double Herbie2D_adj(double *x, double *xb) {

	double Herbie2Db=1.0;
	xb[0] = 0.0;
	xb[1] = 0.0;

	xb[0] = xb[0] + (-8*(cos(8*(x[0]+0.1))*0.05)-0.8*exp(-(0.8*pow(x[0]+1, 2))
	)*2*pow(x[0]+1, 2-1)-exp(-pow(x[0]-1, 2))*2*pow(x[0]-1, 2-1))*
	Herbie2Db;
	xb[1] = xb[1] + (-8*(cos(8*(x[1]+0.1))*0.05)-0.8*exp(-(0.8*pow(x[1]+1, 2))
	)*2*pow(x[1]+1, 2-1)-exp(-pow(x[1]-1, 2))*2*pow(x[1]-1, 2-1))*
	Herbie2Db;
	return exp(-pow((x[0]-1),2))+exp(-0.8*pow((x[0]+1),2))-0.05*sin(8*(x[0]+0.1))
	+ exp(-pow((x[1]-1),2))+exp(-0.8*pow((x[1]+1),2))-0.05*sin(8*(x[1]+0.1));
}



double test_function1D_adj(double *x, double *xb){


	xb[0] = (2*x[0]+10*(cos(10*x[0])*0.5)+2*cos(2*x[0]));
	return sin(2*x[0])+ 0.5* sin(10*x[0]) + x[0]*x[0] ;

}




double simple_2D_linear_test_function1(double *x){

	return 2*x[0]+3*x[1]+1.5;



}

double simple_2D_linear_test_function1_adj(double *x, double *xb) {
	double simple_2D_linear_test_function1;
	xb[0] = xb[0] + 2.0;
	xb[1] = xb[1] + 3.0;
	return 2*x[0]+3*x[1]+1.5;
}



/* Eggholder test function -512<= (x_1,x_2) <= 512
 *
 * global min f(512, 404.2319) = -959.6407
 *
 *
 * */


double Eggholder(double *x){
	return -(x[1]+47.0)*sin(sqrt(fabs(x[1]+0.5*x[0]+47.0)))-x[0]*sin(sqrt(fabs(x[0]-(x[1]+47.0) )));

}


double EggholderAdj(double *xin, double *xb){

	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();
	codi::RealReverse *x = new codi::RealReverse[2];
	x[0]= xin[0];
	x[1]= xin[1];
	tape.registerInput(x[0]);
	tape.registerInput(x[1]);

	codi::RealReverse result = -(x[1]+47.0)*sin(sqrt(fabs(x[1]+0.5*x[0]+47.0)))-x[0]*sin(sqrt(fabs(x[0]-(x[1]+47.0) )));

	tape.registerOutput(result);

	tape.setPassive();
	result.setGradient(1.0);
	tape.evaluate();

	xb[0]=x[0].getGradient();
	xb[1]=x[1].getGradient();

#if 0
	double fdres[2];
	double epsilon = 0.0001;
	double xsave;
	double f0 = Eggholder(xin);
	xsave = xin[0];
	xin[0]+=epsilon;
	double fp = Eggholder(xin);
	xin[0] = xsave;
	fdres[0] = (fp-f0)/epsilon;
	xsave = xin[1];
	xin[1]+=epsilon;
	fp = Eggholder(xin);
	xin[1] = xsave;
	fdres[1] = (fp-f0)/epsilon;
	printf("fd results = \n");
	printf("%10.7f %10.7f\n",fdres[0],fdres[1]);

	printf("ad results = \n");
	printf("%10.7f %10.7f\n",xb[0],xb[1]);
#endif
	delete[] x;
	tape.reset();
	return result.getValue();

}



/* McCormick test function -1.5<= x_1 <= 4  -3<= x_2 <= 4
 *
 * global min f(-0.54719, -1.54719) = -1.9133
 *
 *
 * */

double McCormick(double *x){

	return sin(x[0]+x[1])+ pow( (x[0]-x[1]),2.0)-1.5*x[0]+2.5*x[1]+1.0;

}

double McCormick_adj(double *x, double *xb) {
	double tempb;
	double tempb0;

	tempb = cos(x[0]+x[1]);
	tempb0 = 2.0*pow(x[0]-x[1], 2.0-1);
	xb[0] = xb[0] + tempb0 - 1.5 + tempb;
	xb[1] = xb[1] + 2.5 - tempb0 + tempb;

	return sin(x[0]+x[1])+ pow( (x[0]-x[1]),2.0)-1.5*x[0]+2.5*x[1]+1.0;
}




/* Goldstein-Price test function
 *
 *
 * global min f(0, -1) = 3
 *
 *
 * */

double Goldstein_Price(double *x){
	double x_sqr  =  x[0]*x[0]; // x^2
	double y_sqr  =  x[1]*x[1]; // y^2
	double xy     =  x[0]*x[1]; // xy
	double temp1  = pow( (x[0]+x[1]+1), 2.0);
	double temp2  = (19.0-14.0*x[0]+3.0*x_sqr-14.0*x[1]+6.0*xy+ 3.0*y_sqr);
	double temp3  = pow( (2*x[0]-3.0*x[1]), 2.0);
	double temp4  = (18.0 - 32.0 *x[0] + 12.0* x_sqr +48.0*x[1]-36.0*xy+27.0*y_sqr);

	return (1.0+temp1*temp2)*(30.0+temp3*temp4);

}


double Goldstein_Price_adj(double *x, double *xb) {
	double x_sqr = x[0]*x[0];
	double x_sqrb;
	// x^2
	double y_sqr = x[1]*x[1];
	double y_sqrb;
	// y^2
	double xy = x[0]*x[1];
	double xyb;
	// xy
	double temp1;
	double temp1b;
	double tempb;
	double tempb0;
	double tempb1;
	double tempb2;
	temp1 = pow(x[0] + x[1] + 1, 2.0);
	double temp2 = 19.0 - 14.0*x[0] + 3.0*x_sqr - 14.0*x[1] + 6.0*xy + 3.0*
			y_sqr;
	double temp2b;
	double temp3;
	double temp3b;
	temp3 = pow(2*x[0] - 3.0*x[1], 2.0);
	double temp4 = 18.0 - 32.0*x[0] + 12.0*x_sqr + 48.0*x[1] - 36.0*xy + 27.0*
			y_sqr;
	double temp4b;
	tempb = (temp3*temp4+30.0);
	tempb0 = (temp1*temp2+1.0);
	temp1b = temp2*tempb;
	temp2b = temp1*tempb;
	temp3b = temp4*tempb0;
	temp4b = temp3*tempb0;
	x_sqrb = 3.0*temp2b + 12.0*temp4b;
	xb[0] = xb[0] - 32.0*temp4b;
	xb[1] = xb[1] + 48.0*temp4b;
	y_sqrb = 3.0*temp2b + 27.0*temp4b;
	xyb = 6.0*temp2b - 36.0*temp4b;
	tempb1 = 2.0*pow(2*x[0]-3.0*x[1], 2.0-1)*temp3b;
	xb[0] = xb[0] + 2*tempb1;
	xb[1] = xb[1] - 3.0*tempb1;
	xb[0] = xb[0] - 14.0*temp2b;
	xb[1] = xb[1] - 14.0*temp2b;
	tempb2 = 2.0*pow(x[0]+x[1]+1, 2.0-1)*temp1b;
	xb[0] = xb[0] + tempb2;
	xb[1] = xb[1] + tempb2;
	xb[0] = xb[0] + x[1]*xyb;
	xb[1] = xb[1] + x[0]*xyb;
	xb[1] = xb[1] + 2*x[1]*y_sqrb;
	xb[0] = xb[0] + 2*x[0]*x_sqrb;


	return (1.0+temp1*temp2)*(30.0+temp3*temp4);
}


/* Rosenbrock test function with a = 1 and b= 100
 *
 * f(x,y) = (a-x)^2 + b(y-x^2)^2
 * global min f(x,y) = (a,a^2)
 *
 *
 * */

double Rosenbrock(double *x){

	return (1.0-x[0])* (1.0-x[0]) + 100.0 *(x[1]-x[0]*x[0])*(x[1]-x[0]*x[0]);


}

double Rosenbrock_adj(double *x, double *xb) {
	double tempb;
	double Rosenbrockb = 1.0;

	xb[0] = 0.0;
	xb[1] = 0.0;
	tempb = 100.0*2*(x[1]-x[0]*x[0])*Rosenbrockb;
	xb[0] = xb[0] - 2*x[0]*tempb - 2*(1.0-x[0])*Rosenbrockb;
	xb[1] = xb[1] + tempb;

	return ( (1.0-x[0])* (1.0-x[0]) + 100.0 *(x[1]-x[0]*x[0])*(x[1]-x[0]*x[0]));

}


double Rosenbrock3D(double *x){

	double term1 = (1.0-x[0])* (1.0-x[0]) + 100.0 *(x[1]-x[0]*x[0])*(x[1]-x[0]*x[0]);
	double term2 = (1.0-x[1])* (1.0-x[1]) + 100.0 *(x[2]-x[1]*x[1])*(x[2]-x[1]*x[1]);
	return term1+term2;


}

double Rosenbrock3D_adj(double *x, double *xb) {

	double Rosenbrock3Db = 1.0;
	xb[0] = 0.0; xb[1] = 0.0; xb[2] = 0.0;

	double term1 = (1.0-x[0])*(1.0-x[0]) + 100.0*(x[1]-x[0]*x[0])*(x[1]-x[0]*x[0]);
	double term1b = 0.0;
	double term2 = (1.0-x[1])*(1.0-x[1]) + 100.0*(x[2]-x[1]*x[1])*(x[2]-x[1]*x[1]);
	double term2b = 0.0;
	double tempb;
	double tempb0;

	term1b = Rosenbrock3Db;
	term2b = Rosenbrock3Db;
	tempb = 100.0*2*(x[2]-x[1]*x[1])*term2b;
	xb[1] = xb[1] - 2*x[1]*tempb - 2*(1.0-x[1])*term2b;
	xb[2] = xb[2] + tempb;
	tempb0 = 100.0*2*(x[1]-x[0]*x[0])*term1b;
	xb[0] = xb[0] - 2*x[0]*tempb0 - 2*(1.0-x[0])*term1b;
	xb[1] = xb[1] + tempb0;
	return term1+term2;
}

double Rosenbrock4D(double *x){

	double term1 = (1.0-x[0])* (1.0-x[0]) + 100.0 *(x[1]-x[0]*x[0])*(x[1]-x[0]*x[0]);
	double term2 = (1.0-x[1])* (1.0-x[1]) + 100.0 *(x[2]-x[1]*x[1])*(x[2]-x[1]*x[1]);
	double term3 = (1.0-x[2])* (1.0-x[2]) + 100.0 *(x[3]-x[2]*x[2])*(x[3]-x[2]*x[2]);
	return term1+term2+term3;


}

double Rosenbrock4D_adj(double *x, double *xb) {
	double Rosenbrock4Db = 1.0;
	xb[0] = 0.0; xb[1] = 0.0; xb[2] = 0.0; xb[3] = 0.0;
	double term1 = (1.0-x[0])*(1.0-x[0]) + 100.0*(x[1]-x[0]*x[0])*(x[1]-x[0]*x
			[0]);
	double term1b = 0.0;
	double term2 = (1.0-x[1])*(1.0-x[1]) + 100.0*(x[2]-x[1]*x[1])*(x[2]-x[1]*x
			[1]);
	double term2b = 0.0;
	double term3 = (1.0-x[2])*(1.0-x[2]) + 100.0*(x[3]-x[2]*x[2])*(x[3]-x[2]*x
			[2]);
	double term3b = 0.0;
	double tempb;
	double tempb0;
	double tempb1;
	double Rosenbrock4D;
	term1b = Rosenbrock4Db;
	term2b = Rosenbrock4Db;
	term3b = Rosenbrock4Db;
	tempb = 100.0*2*(x[3]-x[2]*x[2])*term3b;
	xb[2] = xb[2] - 2*x[2]*tempb - 2*(1.0-x[2])*term3b;
	xb[3] = xb[3] + tempb;
	tempb0 = 100.0*2*(x[2]-x[1]*x[1])*term2b;
	xb[1] = xb[1] - 2*x[1]*tempb0 - 2*(1.0-x[1])*term2b;
	xb[2] = xb[2] + tempb0;
	tempb1 = 100.0*2*(x[1]-x[0]*x[0])*term1b;
	xb[0] = xb[0] - 2*x[0]*tempb1 - 2*(1.0-x[0])*term1b;
	xb[1] = xb[1] + tempb1;
	return term1+term2+term3;
}

double Rosenbrock8D(double *x){


	double temp=0.0;

	for(int i=0; i<7; i++){

		temp+= 100.0*(x[i+1]-x[i]*x[i])*(x[i+1]-x[i]*x[i]) + (1-x[i])*(1-x[i]);
	}

	return temp;

}


double Rosenbrock8D_adj(double *x, double *xb) {
	double temp = 0.0;
	double tempb0 = 0.0;


	for(int i=0; i<8; i++){
		xb[i]=0.0;

	}

	for(int i=0; i<7; i++){

		temp+= 100.0*(x[i+1]-x[i]*x[i])*(x[i+1]-x[i]*x[i]) + (1-x[i])*(1-x[i]);
	}


	tempb0 = 1.0;
	{
		double tempb;
		for (int i = 6; i > -1; --i) {
			tempb = 100.0*2*(x[i+1]-x[i]*x[i])*tempb0;
			xb[i + 1] = xb[i + 1] + tempb;
			xb[i] = xb[i] - 2*(1-x[i])*tempb0 - 2*x[i]*tempb;
		}
	}

	return temp;
}


double Shubert(double *x){

	double term1 =0.0;
	double term2 =0.0;

	for(int i=0; i<5; i++){

		term1+= i*cos( (i+1)*x[0]+i );

	}

	for(int j=0; j<5; j++){

		term2+= j*cos((j+1)*x[1]+j);

	}


	return term1*term2;

}


double Shubert_adj(double *x, double *xb) {
	double term1 = 0.0;
	double term1b = 0.0;
	double term2 = 0.0;
	double term2b = 0.0;
	double Shubertb=1.0;

	xb[0]=0.0;
	xb[1]=0.0;

	for (int i = 0; i < 5; ++i)
		term1 = term1 + i*cos((i+1)*x[0]+i);
	for (int j = 0; j < 5; ++j)
		term2 = term2 + j*cos((j+1)*x[1]+j);
	term1b = term2*Shubertb;
	term2b = term1*Shubertb;
	for (int j = 4; j > -1; --j)
		xb[1] = xb[1] - sin(j+(j+1)*x[1])*j*(j+1)*term2b;
	for (int i = 4; i > -1; --i)
		xb[0] = xb[0] - sin(i+(i+1)*x[0])*i*(i+1)*term1b;

	return term1*term2;

}



/* Himmelblau test function
 *
 * f(x,y) = (x^2+y-11)^2 + (x+y^2-7)^2
 * four local minima :
 * f(3.0,2.0)=0.0
 * f(2.805118, 3.131312) = 0.0
 * f(-3.779310, -3.283186) = 0.0
 * f(3.584428, -1.848126)  = 0.0
 *
 *
 * */

double Himmelblau(double *x){

	return pow( (x[0]*x[0]+x[1]-11.0), 2.0 ) + pow( (x[0]+x[1]*x[1]-7.0), 2.0 );


}

double Himmelblau_adj(double *x, double *xb) {
	double tempb;
	double tempb0;
	tempb = 2.0*pow(x[0]*x[0]+x[1]-11.0, 2.0-1);
	tempb0 = 2.0*pow(x[0]+x[1]*x[1]-7.0, 2.0-1);
	xb[0] = xb[0] + tempb0 + 2*x[0]*tempb;
	xb[1] = xb[1] + 2*x[1]*tempb0 + tempb;

	return pow( (x[0]*x[0]+x[1]-11.0), 2.0 ) + pow( (x[0]+x[1]*x[1]-7.0), 2.0 );

}


/*
 *
 * The Borehole function models water flow through a borehole.
 * Its simplicity and quick evaluation makes it a commonly used function
 * for testing a wide variety of methods in computer experiments.
 * The response is water flow rate, in m3/yr.
 *
 *
 * f(x) = 2*pi* Tu (Hu-Hl) / ( ln(r/rw)* ( 1+ 2* L* Tu/( ln(r/rw)*rw^2* Kw ) + Tu/Tl ) )
 * rw (x[0]) : radius of borehole (m) [0.05, 0.15]
 * r   (x[1]) : radius of influence(m) [100, 50000]
 * Tu (x[2]) : transmissivity of upper aquifer (m2/yr) [63500, 115600]
 * Hu (x[3]) : potentiometric head of upper aquifer (m)  [990, 1110]
 * Tl (x[4]) : transmissivity of lower aquifer (m2/yr) [63.1, 116]
 * Hl (x[5]) : potentiometric head of lower aquifer (m) [700, 820]
 * L   (x[6]) : length of borehole (m)   [1120, 1680]
 * Kw (x[7]) : hydraulic conductivity of borehole (m/yr) [9855, 12045]
 */


double Borehole(double *x){

	double pi = 3.14159265359;

	double ln_r_div_rw = log(x[1]/x[0]);  // ln(r/rw)
	double num = 2*pi* x[2]*(x[3]- x[5]); // 2pi*Tu*(Hu-Hl)
	double two_L_Tu = 2.0* x[6]*x[2];
	double den = ln_r_div_rw * (1.0 + two_L_Tu /( ( ln_r_div_rw )* x[0]*x[0]*x[7])+ x[2]/x[4] );
	return num/den;

}


double BoreholeAdj(double *xin, double *xb){


	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();
	codi::RealReverse *x = new codi::RealReverse[10];

	for(int i=0; i<10; i++) {

		x[i] = xin[i];
		tape.registerInput(x[i]);
	}

	codi::RealReverse ln_r_div_rw = log(x[1]/x[0]);  // ln(r/rw)
	codi::RealReverse num = 2.0*datum::pi* x[2]*(x[3]- x[5]); // 2pi*Tu*(Hu-Hl)
	codi::RealReverse two_L_Tu = 2.0* x[6]*x[2];
	codi::RealReverse den = ln_r_div_rw * (1.0 + two_L_Tu /( ( ln_r_div_rw )* x[0]*x[0]*x[7])+ x[2]/x[4] );
	codi::RealReverse result = num/den;

	tape.registerOutput(result);

	tape.setPassive();
	result.setGradient(1.0);
	tape.evaluate();

	for(int i=0; i<8; i++) {

		xb[i]=x[i].getGradient();
	}


#if 0
	double fdres[8];
	double epsilon = 0.0;
	double xsave;
	double fp = 0.0;

	double f0 = Borehole(xin);
	for(int i=0; i<8; i++){

		epsilon = xin[i]*0.001;
		xsave = xin[i];
		//		printf("xin[%d] = %20.15f\n",i,xin[i]);
		xin[i]+=epsilon;
		//		printf("xin[%d] = %20.15f\n",i,xin[i]);
		fp =  Wingweight(xin);
		xin[i] = xsave;
		printf("fp = %20.15f f0 = %20.15f\n",fp,f0);

		fdres[i] = (fp-f0)/epsilon;
	}

	printf("fd results = \n");
	for(int i=0; i<10; i++) {
		printf("%10.7f ",fdres[i]);
	}
	printf("\n");
	printf("ad results = \n");
	for(int i=0; i<10; i++) {
		printf("%10.7f ",xb[i]);
	}
	printf("\n");

#endif
	delete[] x;
	tape.reset();

	return result.getValue();



}

/*
 *  Sw: Wing Area (ft^2) (150,200)
 *  Wfw: Weight of fuel in the wing (lb) (220,300)
 *  A: Aspect ratio (6,10)
 *  Lambda: quarter chord sweep (deg) (-10,10)
 *  q: dynamic pressure at cruise (lb/ft^2)  (16,45)
 *  lambda: taper ratio (0.5,1)
 *  tc: aerofoil thickness to chord ratio (0.08,0.18)
 *  Nz: ultimate load factor (2.5,6)
 *  Wdg: flight design gross weight (lb)  (1700,2500)
 *  Wp: paint weight (lb/ft^2) (0.025, 0.08)
 *
 */
double Wingweight(double *x){
#if 0
	printf("x = \n");
	for(int i=0; i<10; i++) {

		printf("%10.7f ",x[i]);

	}
	printf("\n");
#endif

	double Sw=x[0];
	double Wfw=x[1];
	double A=x[2];
	double Lambda=x[3];
	double q=x[4];
	double lambda=x[5];
	double tc=x[6];
	double Nz=x[7];
	double Wdg=x[8];
	double Wp=x[9];


	double deg = (Lambda*datum::pi)/180.0;

	double W = 0.036*pow(Sw,0.758)*pow(Wfw,0.0035)*pow((A/(cos(deg)*cos(deg))),0.6) *
			pow(q,0.006)*pow(lambda,0.04)*pow( (100.0*tc/cos(deg)), -0.3) *pow( (Nz*Wdg),0.49) + Sw*Wp;

	return(W);
}

double WingweightAdj(double *xin, double *xb){


	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();
	codi::RealReverse *x = new codi::RealReverse[10];

	for(int i=0; i<10; i++) {

		x[i] = xin[i];
		tape.registerInput(x[i]);
	}


	codi::RealReverse Sw=x[0];
	codi::RealReverse Wfw=x[1];
	codi::RealReverse A=x[2];
	codi::RealReverse Lambda=x[3];
	codi::RealReverse q=x[4];
	codi::RealReverse lambda=x[5];
	codi::RealReverse tc=x[6];
	codi::RealReverse Nz=x[7];
	codi::RealReverse Wdg=x[8];
	codi::RealReverse Wp=x[9];


	codi::RealReverse deg = (Lambda*datum::pi)/180.0;

	codi::RealReverse result = 0.036*pow(Sw,0.758)*pow(Wfw,0.0035)*pow((A/(cos(deg)*cos(deg))),0.6) *
			pow(q,0.006)*pow(lambda,0.04)*pow( (100.0*tc/cos(deg)), -0.3) *pow( (Nz*Wdg),0.49) + Sw*Wp;

	tape.registerOutput(result);

	tape.setPassive();
	result.setGradient(1.0);
	tape.evaluate();

	for(int i=0; i<10; i++) {

		xb[i]=x[i].getGradient();
	}


#if 0
	double fdres[10];
	double epsilon = 0.0;
	double xsave;
	double fp = 0.0;

	double f0 = Wingweight(xin);
	for(int i=0; i<10; i++){

		epsilon = xin[i]*0.001;
		xsave = xin[i];
		//		printf("xin[%d] = %20.15f\n",i,xin[i]);
		xin[i]+=epsilon;
		//		printf("xin[%d] = %20.15f\n",i,xin[i]);
		fp =  Wingweight(xin);
		xin[i] = xsave;
		printf("fp = %20.15f f0 = %20.15f\n",fp,f0);

		fdres[i] = (fp-f0)/epsilon;
	}

	printf("fd results = \n");
	for(int i=0; i<10; i++) {
		printf("%10.7f ",fdres[i]);
	}
	printf("\n");
	printf("ad results = \n");
	for(int i=0; i<10; i++) {
		printf("%10.7f ",xb[i]);
	}
	printf("\n");

#endif
	delete[] x;
	tape.reset();

	return result.getValue();



}




/*
 *
 * Generate test data for a given 2D function with random distribution
 *
 */


/* generate the contour plot with the given function within specified bounds and resolution */
void generate_contour_plot_2D_function(double (*test_function)(double *), double *bounds, std::string function_name, int resolution=100){
	/* first generate output data file */

	std::string filename= function_name;
	filename += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".dat";


	std::string file_name_for_plot = function_name;
	file_name_for_plot += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";



	printf("opening file %s for output...\n",filename.c_str());
	FILE *test_function_data=fopen(filename.c_str(),"w");



	double dx,dy; /* step sizes in x and y directions */
	double x[2];
	double func_val;
	dx = (bounds[1]-bounds[0])/(resolution-1);
	dy = (bounds[3]-bounds[2])/(resolution-1);

	x[0] = bounds[0];
	for(int i=0;i<resolution;i++){
		x[1] = bounds[2];
		for(int j=0;j<resolution;j++){
			func_val = test_function(x);
			fprintf(test_function_data,"%10.7f %10.7f %10.7f\n",x[0],x[1],func_val);


			x[1]+=dy;
		}
		x[0]+= dx;

	}

	fclose(test_function_data);

	std::string python_command = "python -W ignore plot_2d_surface.py "+ filename+ " "+ file_name_for_plot ;



	FILE* in = popen(python_command.c_str(), "r");


	fprintf(in, "\n");



}


/* generate the contour plot with the given function
 * and its gradient sensitivities within specified bounds and resolution
 *
 *   */
void generate_contour_plot_2D_function_with_gradient(double (*test_function)(double *, double *),
		double *bounds,
		std::string function_name,
		std::string python_dir,
		int resolution=100){
	/* first generate output data file */

	std::string filename= function_name;
	filename += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".dat";

	std::string filename_g1= function_name;
	filename_g1 += "_g1_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".dat";


	std::string filename_g2= function_name;
	filename_g2 += "_g2_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".dat";



	std::string file_name_for_plot = function_name;
	file_name_for_plot += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";


	std::string file_name_for_plot_g1 = function_name;
	file_name_for_plot_g1 += "_g1_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";

	std::string file_name_for_plot_g2 = function_name;
	file_name_for_plot_g2 += "_g2_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";




	printf("opening file %s for output...\n",filename.c_str());



	FILE *test_function_data=fopen(filename.c_str(),"w");
	FILE *test_function_data_g1=fopen(filename_g1.c_str(),"w");
	FILE *test_function_data_g2=fopen(filename_g2.c_str(),"w");



	double dx,dy; /* step sizes in x and y directions */
	double x[2];
	double xb[2];
	double func_val;
	dx = (bounds[1]-bounds[0])/(resolution-1);
	dy = (bounds[3]-bounds[2])/(resolution-1);

	x[0] = bounds[0];
	for(int i=0;i<resolution;i++){
		x[1] = bounds[2];
		for(int j=0;j<resolution;j++){
			xb[0]=0.0; xb[1]=0.0;
			func_val = test_function(x,xb);
			fprintf(test_function_data,"%10.7f %10.7f %10.7f\n",x[0],x[1],func_val);
			fprintf(test_function_data_g1,"%10.7f %10.7f %10.7f\n",x[0],x[1],xb[0]);
			fprintf(test_function_data_g2,"%10.7f %10.7f %10.7f\n",x[0],x[1],xb[1]);

			x[1]+=dy;
		}
		x[0]+= dx;

	}

	fclose(test_function_data);
	fclose(test_function_data_g1);
	fclose(test_function_data_g2);

	std::string title = "function";
	std::string python_command = "python -W ignore "+python_dir+"/plot_2d_surface.py "+ filename+ " "+ file_name_for_plot + " "+title;


	FILE* in = popen(python_command.c_str(), "r");
	fprintf(in, "\n");

	title = "df/dx1";
	std::string python_command_g1 = "python -W ignore "+python_dir+"/plot_2d_surface.py "+ filename_g1+ " "+ file_name_for_plot_g1+ " "+title ;
	FILE* in_g1 = popen(python_command_g1.c_str(), "r");
	fprintf(in_g1, "\n");

	title = "df/dx2";
	std::string python_command_g2 = "python -W ignore "+python_dir+"/plot_2d_surface.py "+ filename_g2+ " "+ file_name_for_plot_g2 + " "+title;
	FILE* in_g2 = popen(python_command_g2.c_str(), "r");
	fprintf(in_g2, "\n");




}



void generate_plot_1D_function(double (*test_function)(double *), double *bounds, std::string function_name, int resolution=1000){
	/* first generate output data file */

	std::string filename= function_name;
	filename += "_"+std::to_string(resolution)+".dat";


	std::string file_name_for_plot = function_name;
	file_name_for_plot += "_"+std::to_string(resolution)+".png";



	printf("opening file %s for output...\n",filename.c_str());
	FILE *test_function_data=fopen(filename.c_str(),"w");



	double dx; /* step size in x*/
	double x;
	double func_val;
	dx = (bounds[1]-bounds[0])/(resolution-1);


	x= bounds[0];
	for(int i=0;i<resolution;i++){

		func_val = test_function(&x);
		fprintf(test_function_data,"%10.7f %10.7f\n",x,func_val);
		x+= dx;

	}




	fclose(test_function_data);

	std::string python_command = "python -W ignore plot_1d_function.py "+ filename+ " "+ file_name_for_plot ;



	FILE* in = popen(python_command.c_str(), "r");


	fprintf(in, "\n");



}


void perform_kriging_test(double (*test_function)(double *),
		double *bounds,
		std::string function_name ,
		int  number_of_samples,
		int sampling_method,
		int problem_dimension,
		int linear_regression){


	const int number_of_function_validation_points = 100;

	vec kriging_weights;
	vec regression_weights;

	double reg_param = pow(10.0, -8.0);

	int number_of_max_function_evals_for_training = 10000;


	/* file name for data points in csv (comma separated values) format */
	std::string input_file_name = function_name+"_"+ std::to_string(number_of_samples )+".csv";

	printf("input file name : %s\n",input_file_name.c_str());



	/* generate the contour_plot for 2D data*/
	if(problem_dimension==2){
		generate_contour_plot_2D_function(test_function, bounds, function_name);
	}


	/* generate the function plot for 1D data*/
	if(problem_dimension==1){
		generate_plot_1D_function(test_function, bounds, function_name);

	}
	printf("Generating inputs using %d sample points...\n",number_of_samples );
	/* generate the input data for test	*/
	generate_test_function_data(test_function,
			input_file_name,
			bounds,
			number_of_samples,
			sampling_method,
			problem_dimension);


	/* train response surface with maximum likelihood principle */

	printf("training kriging hyperparameters...\n");


	train_kriging_response_surface(input_file_name,
			"None",
			linear_regression,
			regression_weights,
			kriging_weights,
			reg_param,
			number_of_max_function_evals_for_training,
			RAW_ASCII);


#if 0

	printf("kriging weights = \n");
	kriging_weights.print();
	printf("regression weights = \n");
	regression_weights.print();


	printf("reg_param = %20.15f\n", reg_param);
#endif

	mat data; /* data matrix */
	data.load(input_file_name.c_str(), raw_ascii); /* force loading in raw_ascii format */

	//	data.print();

	int nrows = data.n_rows;
	int ncols = data.n_cols;
	int dim = ncols - 1;


	int dimension_of_R = nrows;

	double beta0=0.0;


	mat X = data.submat(0, 0, nrows - 1, ncols - 2);
#if 0
	X.print();
#endif
	vec ys = data.col(dim);

#if 0
	ys.print();
#endif
	vec x_max(dim);
	x_max.fill(0.0);

	vec x_min(dim);
	x_min.fill(0.0);

	for (int i = 0; i < dim; i++) {
		x_max(i) = data.col(i).max();
		x_min(i) = data.col(i).min();

	}

#if 0
	printf("maximum = \n");
	x_max.print();

	printf("minimum = \n");
	x_min.print();
#endif


	/* normalize data */
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < dim; j++) {
			X(i, j) = (1.0/dim)*(X(i, j) - x_min(j)) / (x_max(j) - x_min(j));
		}
	}


	if (linear_regression == LINEAR_REGRESSION_ON) { /* if linear regression is on */

		mat augmented_X(X.n_rows, dim + 1);

		for (unsigned int i = 0; i < X.n_rows; i++) {
			for (int j = 0; j <= dim; j++) {
				if (j == 0)
					augmented_X(i, j) = 1.0;
				else
					augmented_X(i, j) = X(i, j - 1);

			}
		}



		/* now update the ys_func vector */

		ys = ys - augmented_X * regression_weights;

		//		printf("Updated ys vector = \n");

		//		ys.print();

	} /* end of linear regression */







	/* allocate the correlation matrix */
	mat R =zeros(dimension_of_R, dimension_of_R);


	//	R.print();

	vec theta = kriging_weights.head(dim);
	vec gamma = kriging_weights.tail(dim);




	/* evaluate the correlation matrix for the given theta and gamma */
	compute_R_matrix(theta,gamma, reg_param, R, X);



	mat Rinv = inv(R);
	/* vector of ones */
	vec I = ones(dimension_of_R);

	beta0 = (1.0/dot(I,Rinv*I)) * (dot(I,Rinv*ys));
	vec R_inv_ys_min_beta = Rinv* (ys-beta0* I);


	printf("beta0 = %10.7f\n",beta0);


#if 0
	printf("R_inv_ys_min_beta =\n");
	R_inv_ys_min_beta.print();
#endif



	/* check in-sample error */

	double in_sample_error = 0.0;
	for(unsigned int i=0;i<X.n_rows;i++){

		rowvec x = X.row(i);


		double func_val = calculate_f_tilde(x,
				X,
				beta0,
				regression_weights,
				R_inv_ys_min_beta,
				kriging_weights);

		for(int j=0; j<dim;j++) {

			x(j) = dim* x(j)* (x_max(j) - x_min(j))+x_min(j);
		}

		double func_val_exact = test_function(x.memptr());

		printf("\n");
		x.print();
		printf("\n");
		printf("ftilde = %10.7f fexact= %10.7f\n",func_val,func_val_exact );

		in_sample_error+= (func_val_exact-func_val)*(func_val_exact-func_val);


	}

	in_sample_error = sqrt(in_sample_error/X.n_rows);

	printf("in sample error = %10.7f\n",in_sample_error);






	/* visualize with contour plot if the problem is 2D */
	if (problem_dimension == 2){
		int resolution =100;

		std::string kriging_response_surface_file_name = function_name+"_"+"kriging_response_surface_"+ std::to_string(number_of_samples )+".dat";

		FILE *kriging_response_surface_file = fopen(kriging_response_surface_file_name.c_str(),"w");

		double dx,dy; /* step sizes in x and y directions */
		rowvec x(2);
		rowvec xnorm(2);

		double func_val;
		dx = (bounds[1]-bounds[0])/(resolution-1);
		dy = (bounds[3]-bounds[2])/(resolution-1);

		double out_sample_error=0.0;

		x[0] = bounds[0];
		for(int i=0;i<resolution;i++){
			x[1] = bounds[2];
			for(int j=0;j<resolution;j++){

				/* normalize x */
				xnorm(0)= (1.0/dim)*(x(0)- x_min(0)) / (x_max(0) - x_min(0));
				xnorm(1)= (1.0/dim)*(x(1)- x_min(1)) / (x_max(1) - x_min(1));

				func_val = calculate_f_tilde(xnorm, X, beta0, regression_weights, R_inv_ys_min_beta, kriging_weights);
				fprintf(kriging_response_surface_file,"%10.7f %10.7f %10.7f\n",x(0),x(1),func_val);

				double func_val_exact = test_function(x.memptr());

				out_sample_error+= (func_val_exact-func_val)*(func_val_exact-func_val);



				x[1]+=dy;
			}
			x[0]+= dx;

		}
		fclose(kriging_response_surface_file);

		out_sample_error = sqrt(out_sample_error/(resolution*resolution));

		printf("out of sample error = %10.7f\n",out_sample_error);



		/* plot the kriging response surface */

		std::string file_name_for_plot = function_name+"_"+"kriging_response_surface_";
		file_name_for_plot += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";

		std::string python_command = "python -W ignore plot_2d_surface.py "+ kriging_response_surface_file_name+ " "+ file_name_for_plot ;



		FILE* in = popen(python_command.c_str(), "r");


		fprintf(in, "\n");


	}


	/* visualize with line plot if the problem is 1D */
	if (problem_dimension == 1){
		int resolution =1000;


		std::string kriging_response_surface_file_name =
				function_name+"_"+"kriging_response_surface_"
				+ std::to_string(number_of_samples )
		+".dat";

		FILE *kriging_response_surface_file = fopen(kriging_response_surface_file_name.c_str(),"w");

		double dx; /* step sizes in x */
		rowvec x(1);
		rowvec xnorm(1);

		double func_val;
		double func_val_exact;
		double final_validation_error=0.0;

		dx = (bounds[1]-bounds[0])/(resolution-1);


		x(0) = bounds[0];
		for(int i=0;i<resolution;i++){


			/* normalize x */
			xnorm(0)= (1.0/dim)*(x(0)- x_min(0)) / (x_max(0) - x_min(0));
			func_val = calculate_f_tilde(xnorm, X, beta0, regression_weights,
					R_inv_ys_min_beta, kriging_weights);

			func_val_exact = test_function(x.memptr());

			printf("%10.7f %10.7f %10.7f\n",x(0),func_val,func_val_exact);
			fprintf(kriging_response_surface_file,"%10.7f %10.7f\n",x(0),func_val);
			final_validation_error+=sqrt(pow((func_val_exact-func_val),2.0));

			x(0)+= dx;

		}
		fclose(kriging_response_surface_file);


		printf("final validation error(L2) : %10.7f\n",final_validation_error );



		/* plot the kriging response surface */

		std::string file_name_for_plot = function_name+"_"+"kriging_response_surface_";
		file_name_for_plot += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";

		std::string python_command = "python -W ignore plot_1d_function.py "+ kriging_response_surface_file_name+ " "+ file_name_for_plot ;


		FILE* in_python = popen(python_command.c_str(), "r");


		fprintf(in_python, "\n");
	}



	if (problem_dimension > 2){

		rowvec xs(problem_dimension);
		rowvec xe(problem_dimension);
		rowvec x(problem_dimension);
		rowvec xnorm(problem_dimension);

		double func_val;
		double func_val_exact;
		double final_validation_error=0.0;


		int count=0;
		for(int i=0;i<problem_dimension;i++){
			xs(i) = bounds[count];
			count++;
			xe(i) = bounds[count];
			count++;

		}


		for(int i=0; i<number_of_function_validation_points;i++ ){

			for(int j=0;j<problem_dimension;j++) {

				x(j) = RandomDouble(xs(j), xe(j));
			}
#if 1
			printf("x = \n");
			x.print();
#endif
			/* normalize x */
			for(int j=0;j<problem_dimension;j++) {

				xnorm(j)= (1.0/dim)*(x(j)- x_min(j)) / (x_max(j) - x_min(j));
			}


			func_val = calculate_f_tilde(xnorm, X, beta0, regression_weights,
					R_inv_ys_min_beta, kriging_weights);

			func_val_exact = test_function(x.memptr());

			printf("f_exact = %10.7f, f_tilde =  %10.7f\n",func_val_exact,func_val);

			final_validation_error+=pow((func_val_exact-func_val),2.0);




		} /* end of validation loop */

		final_validation_error = final_validation_error/number_of_function_validation_points;

		printf("final validation error(L2) : %10.7f\n",final_validation_error );




	}


}





void perform_NNregression_test(double (*test_function)(double *),
		double *bounds,
		std::string function_name ,
		int  number_of_samples,
		int sampling_method,
		int problem_dimension,
		int number_of_trials){

	FILE *NNinput;

	vec meanError(number_of_trials);
	vec stdError(number_of_trials);


	for(int trial=0; trial <number_of_trials; trial ++ ){

		/* file name for data points in csv (comma separated values) format */
		std::string input_file_name = function_name+"_"
				+ std::to_string(number_of_samples )
		+".csv";

		printf("input file name : %s\n",input_file_name.c_str());
		printf("Generating inputs using %d sample points...\n",number_of_samples );


		/* generate the input data for test	*/
		generate_test_function_data(test_function,
				input_file_name,
				bounds,
				number_of_samples,
				sampling_method,
				problem_dimension);


		/* run Neural Network Regression */

		std::string python_command = "python -W ignore "+settings.python_dir+"/NeuralNetworkReg.py "+ input_file_name+ " > python.out";
#if 0
		printf("python_command : %s\n",python_command.c_str());
#endif

		system(python_command.c_str());

		NNinput = fopen("NNoutput.dat", "r");

		fscanf(NNinput,"%lf",&stdError(trial));
		fscanf(NNinput,"%lf",&meanError(trial));

		fclose(NNinput);

		printf("it = %d, squared error = %10.7f\n",trial,meanError(trial));

	}


	printf("mean squared error = %10.7f\n",mean(meanError));


}





void perform_GEK_test(double (*test_function)(double *),
		double (*test_function_adj)(double *, double *),
		double *bounds,
		std::string function_name ,
		int  number_of_samples_with_only_f_eval,
		int number_of_samples_with_g_eval,
		int sampling_method,
		int method_for_solving_lin_eq,
		int dim,
		int linear_regression,
		std::string python_dir){

	if(dim <= 0){
		printf("Error: problem dimension must be greater than zero\n");
		exit(-1);
	}


	vec kriging_weights;
	vec regression_weights;
	double reg_param;


	int number_of_max_function_evals_for_training = 10000;

	std::string input_file_name = function_name+"_"+ std::to_string(number_of_samples_with_only_f_eval)+"_"+std::to_string(number_of_samples_with_g_eval)+ ".csv";

	printf("input file name : %s\n",input_file_name.c_str());



	if(dim ==2){
		/* generate the contour_plot */
		generate_contour_plot_2D_function(test_function, bounds, function_name);

	}


	if(dim ==2){
		printf("Generating inputs using %d points (%d gradient computations)...\n",number_of_samples_with_only_f_eval+ number_of_samples_with_g_eval,number_of_samples_with_g_eval);


		/* generate the input data for test	*/
		generate_2D_test_function_data_GEK(test_function,
				test_function_adj,
				input_file_name,
				bounds,
				number_of_samples_with_only_f_eval,
				number_of_samples_with_g_eval,
				sampling_method,
				python_dir);

	}

	/* train the response surface */
	train_GEK_response_surface(input_file_name,
			linear_regression,
			regression_weights,
			kriging_weights,
			reg_param,
			number_of_max_function_evals_for_training,
			dim,
			method_for_solving_lin_eq);



	printf("kriging weights = \n");
	kriging_weights.print();




	mat data_functional_values; // data matrix for only functional values
	mat data_gradients;         // data matrix for only functional values + gradient sensitivities


	std::ifstream in(input_file_name);

	if(!in) {
		cout << "Cannot open input file...\n";
		exit(-1);
	}


	std::vector<double> temp;



	std::string str;
	int count_f=0;
	int count_g=0;

	while (std::getline(in, str)) {
		// output the line
		//				std::cout << "line = "<<str << std::endl;


		std::string delimiter = ",";

		size_t pos = 0;
		std::string token;
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			//			std::cout << "token = "<<token << std::endl;


			temp.push_back(atof(token.c_str()));


			str.erase(0, pos + delimiter.length());
		}
		//				std::cout << "str = "<<str << std::endl;
		temp.push_back(atof(str.c_str()));

		//		std::cout<<"temp= \n";
		for (std::vector<double>::iterator it = temp.begin() ; it != temp.end(); ++it){

			//			std::cout<<*it<<std::endl;


		}


		if(temp.size() == dim+1){ // function values

			rowvec newrow(dim+1);
			int count=0;
			for (std::vector<double>::iterator it = temp.begin() ; it != temp.end(); ++it){

				//				std::cout<<*it<<std::endl;
				newrow(count)=*it;
				count++;

			}
			//			newrow.print();
			count_f++;
			data_functional_values.resize(count_f, dim+1);

			data_functional_values.row(count_f-1)=newrow;



		}
		else{ // function+gradient information

			rowvec newrow(2*dim+1);
			int count=0;
			for (std::vector<double>::iterator it = temp.begin() ; it != temp.end(); ++it){

				//				std::cout<<*it<<std::endl;
				newrow(count)=*it;
				count++;

			}
			//			newrow.print();
			count_g++;
			data_gradients.resize(count_g, 2*dim+1);

			data_gradients.row(count_g-1)=newrow;


		}




		temp.clear();

		// now we loop back and get the next line in 'str'
	}


	printf("data functional values = \n");
	data_functional_values.print();
	printf("data gradient sensitivities = \n");
	data_gradients.print();
	printf("\n");

	int n_f_evals = data_functional_values.n_rows;
	int n_g_evals = data_gradients.n_rows;


	mat X; /* data matrix */
	vec ys_func;

	if(n_f_evals>0){
		X = data_functional_values.submat(0, 0, n_f_evals - 1, data_functional_values.n_cols - 2);
	}
	//		X.print();

	if(n_f_evals>0){
		ys_func = data_functional_values.col(dim);
	}

	//	ys.print();


	if( n_g_evals > 0){
		X.insert_rows( n_f_evals , data_gradients.submat(0, 0, n_g_evals - 1, dim-1));

		//	X.print();

	}

	ys_func.print();

	vec x_max(dim);
	x_max.fill(0.0);

	vec x_min(dim);
	x_min.fill(0.0);

	for (int i = 0; i < dim; i++) {
		x_max(i) = X.col(i).max();
		x_min(i) = X.col(i).min();

	}

	printf("maximum = \n");
	x_max.print();

	printf("minimum = \n");
	x_min.print();


	/* normalize the data matrix */
	for (unsigned int i = 0; i < X.n_rows; i++) {
		for (int j = 0; j < dim; j++) {
			X(i, j) = (X(i, j) - x_min(j)) / (x_max(j) - x_min(j));
		}
	}


	/* normalize the data sensitivity data matrix */
	for (unsigned int i = 0; i < data_gradients.n_rows; i++) {
		for (int j = 0; j < dim; j++) {
			data_gradients(i, j) = (data_gradients(i, j) - x_min(j)) / (x_max(j) - x_min(j));
		}

		for (int j = dim+1; j < 2*dim+1; j++) {
			data_gradients(i, j) = data_gradients(i, j) * (x_max(j-dim-1) - x_min(j-dim-1));
		}
	}


	if(n_g_evals > 0){

		ys_func.insert_rows(n_f_evals, data_gradients.col(dim));


	}


	printf("Normalized data = \n");
	X.print();

	printf("Normalized sensitivity data = \n");
	data_gradients.print();





	/* matrix that holds the gradient sensitivities */
	mat grad(n_g_evals, dim);


	/* copy only gradient sensitivities */
	if( n_g_evals > 0) {
		grad = data_gradients.submat(0, dim+1, n_g_evals - 1, 2*dim);
	}


	/* dimension of the correlation matrix R */
	int dimension_of_R = n_f_evals+n_g_evals+ n_g_evals*dim;





	if (linear_regression == 1) { /* if linear regression is on */

		mat augmented_X(X.n_rows, dim + 1);

		for (unsigned int i = 0; i < X.n_rows; i++) {
			for (int j = 0; j <= dim; j++) {
				if (j == 0)
					augmented_X(i, j) = 1.0;
				else
					augmented_X(i, j) = X(i, j - 1);

			}
		}


		/* now update the ys_func vector */

		ys_func = ys_func - augmented_X * regression_weights;

		//		printf("Updated ys vector = \n");

		//		ys.print();

	} /* end of linear regression */



	vec ys(dimension_of_R);

	/* copy n_f_evals+n_g_evals functional values from ys_func */
	for(int i=0; i<n_f_evals+n_g_evals; i++ ) {
		ys(i)=ys_func(i);
	}


	/* for each data point with gradient information*/
	int pos = n_f_evals+n_g_evals;

	for(int j=0; j<dim; j++){ /* for each design variable */
		for(int i=0; i<n_g_evals; i++ ) {


			ys(pos + n_g_evals*j+i) = grad(i,j);

		}

	}


	printf("ys = \n");
	ys.print();




	/* allocate the correlation matrix */
	mat R= eye(dimension_of_R, dimension_of_R);
	//	R.fill(0.0);




	/* initialize the diagonal to 1.0 plus a small regularization term*/
	//	for (int i = 0; i < dimension_of_R; i++) {
	//		R(i, i) = 1.0 + REGULARIZATION_TERM;
	//	}




	vec theta = kriging_weights.head(dim);


	compute_R_matrix_GEK(theta, reg_param, R, X,grad);

	//	R.print();

	//	printf("R = \n");
	//	R.print();



	printf("condition number of R = %10.7e\n", cond(R));


	//	mat Rinv = inv_sympd(R);






	//		Rinv.print();







	mat U(dimension_of_R, dimension_of_R);
	mat D(dimension_of_R, dimension_of_R);
	mat L(dimension_of_R, dimension_of_R);
	mat V(dimension_of_R, dimension_of_R);
	mat Ut(dimension_of_R, dimension_of_R);

	vec s(dimension_of_R);
	vec sinv(dimension_of_R);

	/* allocate the F vector */
	vec F(dimension_of_R);
	F.fill(0.0);

	/* set first n_f_evals entries to one */
	for(int i=0; i<n_f_evals+n_g_evals ;i++) F(i)=1.0;




	//	printf("F = \n");
	//	F.print();
	//	printf("\n");


	if( method_for_solving_lin_eq == SVD){
		int flag_svd = svd( U, s, V, R );

		if (flag_svd == 0) {
			printf("SVD could not be performed\n");

			exit(-1);
		}

		sinv = 1.0/s;

		sinv.print();

		double threshold = 10E-08;
		for(int i=0; i< dimension_of_R; i++){
			if(s(i)  < threshold){
				sinv(i) = 0.0;
			}

		}

		printf("sinv = \n");
		sinv.print();
		printf("\n");


		Ut = trans(U);

		D.fill(0.0);
		for(int i=0; i< dimension_of_R;i++){

			D(i,i) = sinv(i);


		}

	}


	if( method_for_solving_lin_eq == CHOLESKY){

		/* compute Cholesky decomposition */
		int flag = chol(U, R);

		if (flag == 0) {
			printf("Ill conditioned correlation matrix in Cholesky decomposition\n");

			exit(-1);
		}

		L = trans(U);



	}


	vec R_inv_ys(dimension_of_R); /* R^-1* ys */
	vec R_inv_F (dimension_of_R); /* R^-1* F */
	vec R_inv_ys_min_beta(dimension_of_R); /* R^-1* (ys-beta0*F) */

	if( method_for_solving_lin_eq == SVD){
		R_inv_ys =V*D*Ut*ys;
		R_inv_F  =V*D*Ut*F;
	}

	if( method_for_solving_lin_eq == CHOLESKY){

		solve_linear_system_by_Cholesky(U, L, R_inv_ys, ys);
		solve_linear_system_by_Cholesky(U, L, R_inv_F, F);
	}


	/*
		vec res1 = R*R_inv_F-F;

		printf("check = R*R_inv_F-F = \n");
		res1.print();
		printf("\n");

		vec res2 = R*R_inv_ys-ys;

		printf("check = R*R_inv_ys-ys = \n");
		res2.print();
		printf("\n");

	 */

	double beta0 = (1.0/dot(F,R_inv_F)) * (dot(F,R_inv_ys));


	vec ys_min_betaF = ys-beta0*F;

	if( method_for_solving_lin_eq == CHOLESKY){

		solve_linear_system_by_Cholesky(U, L, R_inv_ys_min_beta, ys_min_betaF);

	}

	if( method_for_solving_lin_eq == SVD){
		R_inv_ys_min_beta = V*D*Ut*ys_min_betaF;

	}


	/*

		vec res3 = R*R_inv_ys_min_beta-ys_min_betaF;

		printf("check R*R_inv_ys_min_beta-(ys-beta0*F) = \n");
		res3.print();
		printf("\n");

	 */



	printf("beta0 = %10.7f\n",beta0);



	double in_sample_error = 0.0;
	for(int i=0;i<X.n_rows;i++){

		rowvec x = X.row(i);
		rowvec xp = X.row(i);


		printf("\nData point = %d\n", i+1);
		printf("calling f_tilde at x = %10.7f y = %10.7f\n",
				x(0)* (x_max(0) - x_min(0))+x_min(0),
				x(1)* (x_max(1) - x_min(1))+x_min(1));
		double func_val = calculate_f_tilde_GEK(x,
				X,
				beta0,
				regression_weights,
				R_inv_ys_min_beta,
				theta,
				n_g_evals );

		/* perturb the first design point */
		xp(0)+= 0.000001;

		double func_valp = calculate_f_tilde_GEK(xp,
				X,
				beta0,
				regression_weights,
				R_inv_ys_min_beta,
				theta,
				n_g_evals );

		double der_val1 = (func_valp-func_val)/0.000001;
		xp(0)-= 0.000001;

		/* perturb the second design point */
		xp(1)+= 0.000001;

		func_valp = calculate_f_tilde_GEK(xp,
				X,
				beta0,
				regression_weights,
				R_inv_ys_min_beta,
				theta,
				n_g_evals );

		double der_val2 = (func_valp-func_val)/0.000001;


		/* change to original coordinates before calling the original function */
		x(0) = x(0)* (x_max(0) - x_min(0))+x_min(0);
		x(1) = x(1)* (x_max(1) - x_min(1))+x_min(1);

		double func_val_exact = test_function(x.memptr());

		x(0) += 0.000001;

		double func_val_exactp = test_function(x.memptr());

		double der_val_f1 = (func_val_exactp-func_val_exact)/0.000001;
		x(0) -= 0.000001;

		x(1) += 0.000001;

		func_val_exactp = test_function(x.memptr());

		double der_val_f2 = (func_val_exactp-func_val_exact)/0.000001;


		printf("func_val (approx) = %10.7f\n", func_val);
		printf("func_val (exact) = %10.7f\n", func_val_exact);
		printf("der_val1  (approx) = %10.7f\n", der_val1/(x_max(0) - x_min(0)));
		printf("der_valf1  (approx) = %10.7f\n", der_val_f1);
		printf("der_val2  (approx) = %10.7f\n", der_val2/(x_max(1) - x_min(1)));
		printf("der_valf2  (approx) = %10.7f\n\n", der_val_f2);


		in_sample_error+= (func_val_exact-func_val)*(func_val_exact-func_val);

		printf("in sample error = %10.7f\n", in_sample_error);

	}

	in_sample_error = sqrt(in_sample_error/X.n_rows);

	printf("in sample error = %10.7f\n",in_sample_error);




	if(dim ==2){

		int resolution =100;


		std::string kriging_response_surface_file_name = function_name+"_"+"kriging_response_surface_"+ std::to_string(number_of_samples_with_only_f_eval)+"_"+std::to_string(number_of_samples_with_g_eval)+".dat";

		FILE *kriging_response_surface_file = fopen(kriging_response_surface_file_name.c_str(),"w");

		double dx,dy; /* step sizes in x and y directions */
		rowvec x(2);
		rowvec xnorm(2);

		double func_val;
		dx = (x_max(0) - x_min(0))/(resolution-1);
		dy = (x_max(1) - x_min(1))/(resolution-1);

		dx = (80.0 - 20.0)/(resolution-1);
		dy = (80.0 - 20.0)/(resolution-1);


		double out_sample_error = 0.0;

		x[0] = x_min(0);
		x[0] = 20.0;
		for(int i=0;i<resolution;i++){
			x[1] = x_min(1);
			x[1] = 20.0;
			for(int j=0;j<resolution;j++){

				/* normalize x */
				xnorm(0)= (x(0)- x_min(0)) / (x_max(0) - x_min(0));
				xnorm(1)= (x(1)- x_min(1)) / (x_max(1) - x_min(1));

				//				printf("calling f_tilde at x = %10.7f y = %10.7f\n", x(0),x(1));
				func_val = calculate_f_tilde_GEK(xnorm,
						X,
						beta0,
						regression_weights,
						R_inv_ys_min_beta,
						kriging_weights,
						n_g_evals );

				double func_val_exact = test_function(x.memptr());

				out_sample_error+= (func_val_exact-func_val)*(func_val_exact-func_val);
				//				printf("out of sample error = %10.7f\n",out_sample_error);
				//				printf("x = %10.7f y = %10.7f ftilde = %10.7f fexact = %10.7f\n",x(0),x(1),func_val,func_val_exact);
				fprintf(kriging_response_surface_file,"%10.7f %10.7f %10.7f\n",x(0),x(1),func_val);

				x[1]+=dy;
			}
			x[0]+= dx;

		}
		fclose(kriging_response_surface_file);

		out_sample_error = sqrt(out_sample_error/(resolution*resolution));

		printf("out of sample error = %10.7f\n",out_sample_error);


		/* plot the kriging response surface */

		std::string file_name_for_plot = function_name+"_"+"kriging_response_surface_";
		file_name_for_plot += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";

		std::string python_command = "python -W ignore plot_2d_surface.py "+ kriging_response_surface_file_name+ " "+ file_name_for_plot ;



		FILE *figure_out = popen(python_command.c_str(), "r");


		fprintf(figure_out, "\n");


	}


}









void generate_2D_test_function_data(double (*test_function)(double *),
		std::string filename,
		double bounds[4],
		int number_of_function_evals,
		int sampling_method){

	if(sampling_method == EXISTING_FILE){
		/* do nothing */


	}


	if(sampling_method == RANDOM_SAMPLING){

		FILE *outp;

		//	printf("opening file %s for input...\n",filename.c_str() );
		outp = fopen(filename.c_str(), "w");



		double xs = bounds[0];
		double xe = bounds[1];

		double ys = bounds[2];
		double ye = bounds[3];

		double x[2];

		for(int i=0; i<number_of_function_evals;i++ ){

			x[0] = RandomDouble(xs, xe);
			x[1] = RandomDouble(ys, ye);
			double f_val = test_function(x);

			fprintf(outp,"%10.7f %10.7f %10.7f\n",x[0],x[1],f_val);

		}

		fclose(outp);
	}


	if(sampling_method == LHS_CENTER){

		double x[2];
		double xs = bounds[0];
		double xe = bounds[1];

		double ys = bounds[2];
		double ye = bounds[3];

		std::string lhs_filename = "lhs_points.dat";
		std::string python_command = "python -W ignore lhs.py "+ lhs_filename+ " "+ "2" + " "+ std::to_string(number_of_function_evals)+ " center" ;

		system(python_command.c_str());

		FILE *inp= fopen("lhs_points.dat","r");
		FILE *outp = fopen(filename.c_str(), "w");

		for(int i=0; i<number_of_function_evals;i++ ){

			fscanf(inp,"%lf %lf",&x[0],&x[1]);
			x[0] = x[0]*(xe-xs)+xs;
			x[1] = x[1]*(ye-ys)+ys;
			double f_val = test_function(x);

			fprintf(outp,"%10.7f %10.7f %10.7f\n",x[0],x[1],f_val);

		}

		fclose(inp);
		fclose(outp);


	}

	if(sampling_method == LHS_RANDOM){

		double x[2];
		double xs = bounds[0];
		double xe = bounds[1];

		double ys = bounds[2];
		double ye = bounds[3];

		std::string lhs_filename = "lhs_points.dat";
		std::string python_command = "python -W ignore lhs.py "+ lhs_filename+ " "+ "2" + " "+ std::to_string(number_of_function_evals)+ " None" ;

		system(python_command.c_str());

		FILE *inp= fopen("lhs_points.dat","r");
		FILE *outp = fopen(filename.c_str(), "w");

		for(int i=0; i<number_of_function_evals;i++ ){

			fscanf(inp,"%lf %lf",&x[0],&x[1]);
			x[0] = x[0]*(xe-xs)+xs;
			x[1] = x[1]*(ye-ys)+ys;
			double f_val = test_function(x);

			fprintf(outp,"%10.7f %10.7f %10.7f\n",x[0],x[1],f_val);

		}

		fclose(inp);
		fclose(outp);


	}

}


void generate_test_function_data(double (*test_function)(double *),
		std::string filename,
		double * bounds,
		int number_of_function_evals,
		int sampling_method,
		int problem_dimension){


	double *xs = new double[problem_dimension];
	double *xe = new double[problem_dimension];
	double *x  = new double[problem_dimension];

	int count=0;
	for(int i=0;i<problem_dimension;i++){
		xs[i] = bounds[count];
		count++;
		xe[i] = bounds[count];
		count++;

	}


	if(sampling_method == RANDOM_SAMPLING){

		FILE *outp;
#if 0
		printf("opening file %s for input...\n",filename.c_str() );
#endif
		outp = fopen(filename.c_str(), "w");



		for(int i=0; i<number_of_function_evals;i++ ){

			for(int j=0;j<problem_dimension;j++) x[j] = RandomDouble(xs[j], xe[j]);

			double f_val = test_function(x);

			for(int j=0;j<problem_dimension;j++) fprintf(outp,"%10.7f, ",x[j]);

			fprintf(outp,"%10.7f\n",f_val);

		}

		fclose(outp);
	}


	if(sampling_method == LHS_CENTER){


		std::string str_problem_dim = std::to_string(problem_dimension);
		std::string lhs_filename = "lhs_points.dat";
		std::string python_command = "python -W ignore lhs.py "+ lhs_filename+ " "+ str_problem_dim + " "+ std::to_string(number_of_function_evals)+ " center" ;

		system(python_command.c_str());

		FILE *inp= fopen("lhs_points.dat","r");
		FILE *outp = fopen(filename.c_str(), "w");

		for(int i=0; i<number_of_function_evals;i++ ){

			for(int j=0;j<problem_dimension;j++) fscanf(inp,"%lf",&x[j]);

			for(int j=0;j<problem_dimension;j++) x[j] = x[j]*(xe[j]-xs[j])+xs[j];


			double f_val = test_function(x);

			for(int j=0;j<problem_dimension;j++) fprintf(outp,"%10.7f, ",x[j]);

			fprintf(outp,"%10.7f\n",f_val);

		}

		fclose(inp);
		fclose(outp);




	}

	if(sampling_method == LHS_RANDOM){


		std::string str_problem_dim = std::to_string(problem_dimension);
		std::string lhs_filename = "lhs_points.dat";
		std::string python_command = "python -W ignore lhs.py "+ lhs_filename+ " "+ str_problem_dim + " "+ std::to_string(number_of_function_evals)+ " None" ;

		system(python_command.c_str());

		FILE *inp= fopen("lhs_points.dat","r");
		FILE *outp = fopen(filename.c_str(), "w");

		for(int i=0; i<number_of_function_evals;i++ ){

			for(int j=0;j<problem_dimension;j++) fscanf(inp,"%lf",&x[j]);

			for(int j=0;j<problem_dimension;j++) x[j] = x[j]*(xe[j]-xs[j])+xs[j];


			double f_val = test_function(x);

			for(int j=0;j<problem_dimension;j++) fprintf(outp,"%10.7f, ",x[i]);

			fprintf(outp,"%10.7f\n",f_val);

		}

		fclose(inp);
		fclose(outp);




	}

	if(sampling_method == EXISTING_FILE){
		/* do nothing */


	}

}




void generate_highdim_test_function_data_GEK(double (*test_function)(double *),
		double (*test_function_adj)(double *, double *),
		std::string filename,
		double *bounds,
		int dim,
		int number_of_samples_with_only_f_eval,
		int number_of_samples_with_g_eval,
		int sampling_method){

#if 0
	printf("generate_highdim_test_function_data_GEK...\n");
	printf("dim = %d\n",dim);
#endif

	int number_of_function_evals  =  number_of_samples_with_only_f_eval+ number_of_samples_with_g_eval;


	if(sampling_method == EXISTING_FILE){

		/* do nothing */

	}

	if(sampling_method == RANDOM_SAMPLING){

		FILE *outp;

		printf("opening file %s for input...\n",filename.c_str() );
		outp = fopen(filename.c_str(), "w");

		double *x = new double[dim];
		double *xb = new double[dim];

		/* write functional values to the output file */
		for(int i=0; i<number_of_samples_with_only_f_eval;i++ ){

			for(int j=0; j<dim;j++){
#if 0
				printf("bounds[%d] = %10.7f\n",j*2,bounds[j*2]);
				printf("bounds[%d] = %10.7f\n",j*2+1,bounds[j*2+1]);
#endif
				x[i] = RandomDouble(bounds[j*2], bounds[j*2+1]);
			}

			double f_val = test_function(x);

			for(int j=0; j<dim;j++){
				printf("%10.7f, ",x[j]);
				fprintf(outp,"%10.7f, ",x[j]);
			}
			printf("%10.7f\n",f_val);
			fprintf(outp,"%10.7f\n",f_val);


		}


		/* write functional values and the gradient sensitivities to the output file */

		for(int i=number_of_samples_with_only_f_eval; i<number_of_function_evals;i++ ){

			for(int j=0; j<dim;j++){
#if 0
				printf("bounds[%d] = %10.7f\n",j*2,bounds[j*2]);
				printf("bounds[%d] = %10.7f\n",j*2+1,bounds[j*2+1]);
#endif

				x[j] = RandomDouble(bounds[j*2], bounds[j*2+1]);
				xb[j]=0.0;
			}

			double f_val = test_function_adj(x,xb);

#if 0
			double f_val_original = test_function(x);

			printf("fval = %10.7f\n",f_val);
			printf("fval(original) = %10.7f\n",f_val_original);

			for(int j=0; j<dim;j++){
				double eps = x[j]*0.0001;
				x[j]+= eps;
				double fplus = test_function(x);
				x[j]-= eps;

				double fdval = (fplus-f_val_original)/eps;
				printf("fd = %10.7f\n",fdval);
				printf("adj = %10.7f\n",xb[j]);


			}
#endif



			for(int j=0; j<dim;j++){
				printf("%10.7f, ",x[j]);
				fprintf(outp,"%10.7f, ",x[j]);
			}
			printf("%10.7f, ",f_val);
			fprintf(outp,"%10.7f, ",f_val);

			for(int j=0; j<dim-1;j++){
				printf("%10.7f, ",xb[j]);
				fprintf(outp,"%10.7f, ",xb[j]);
			}
			printf("%10.7f\n",xb[dim-1]);
			fprintf(outp,"%10.7f\n",xb[dim-1]);


		}

		fclose(outp);

		delete[] x;
		delete[] xb;

	}


}


void generate_highdim_test_function_data_cuda(double (*test_function)(double *),
		std::string filename,
		double *bounds,
		int number_of_samples,
		int dim){

#if 0
	printf("generate_highdim_test_function_data...\n");
	printf("dim = %d\n",dim);
#endif

	FILE *outp;

	printf("opening file %s for input...\n",filename.c_str() );
	outp = fopen(filename.c_str(), "w");

	double *x = new double[dim];


	/* write functional values to the output file */
	for(int i=0; i<number_of_samples;i++ ){

		for(int j=0; j<dim;j++){
#if 0
			printf("bounds[%d] = %10.7f\n",j*2,bounds[j*2]);
			printf("bounds[%d] = %10.7f\n",j*2+1,bounds[j*2+1]);
#endif
			x[j] = RandomDouble(bounds[j*2], bounds[j*2+1]);
		}

		double f_val = test_function(x);

		for(int j=0; j<dim;j++){
			printf("%10.7f, ",x[j]);
			fprintf(outp,"%10.7f, ",x[j]);
		}
		printf("%10.7f\n",f_val);
		fprintf(outp,"%10.7f\n",f_val);


	}

	fclose(outp);

	delete[] x;

}




void generate_2D_test_function_data_GEK(double (*test_function)(double *),
		double (*test_function_adj)(double *, double *),
		std::string filename,
		double bounds[4],
		int number_of_samples_with_only_f_eval,
		int number_of_samples_with_g_eval,
		int sampling_method,
		std::string python_dir){


	int number_of_function_evals  =  number_of_samples_with_only_f_eval
			+ number_of_samples_with_g_eval;


	if(sampling_method == EXISTING_FILE){

		/* do nothing */

	}


	if(sampling_method == LHS_CENTER){
		printf("Generating LHS_CENTER samples ...\n");

		vec x(number_of_function_evals);
		vec y(number_of_function_evals);
		double xs = bounds[0];
		double xe = bounds[1];

		double ys = bounds[2];
		double ye = bounds[3];

		std::string lhs_filename = "lhs_points.dat";
		std::string python_command = "python -W ignore "+ python_dir+"/lhs.py "
				+ lhs_filename+ " "+ "2" + " "
				+ std::to_string(number_of_function_evals)
		+ " center" ;

#if 0
		printf("python_command = %s\n",python_command.c_str());
#endif

		system(python_command.c_str());

		FILE *inp= fopen("lhs_points.dat","r");
		FILE *outp = fopen(filename.c_str(), "w");


		for(int i=0; i<number_of_function_evals;i++ ){

			fscanf(inp,"%lf %lf\n",&x(i),&y(i));


		}
		fclose(inp);


		/* write functional values to the output file */
		for(int i=0; i<number_of_samples_with_only_f_eval;i++ ){

			double xin[2];
			xin[0]=x[i]*(xe-xs)+xs;
			xin[1]=y[i]*(ye-ys)+ys;
			double f_val = test_function(xin);
#if 0
			printf("%10.7f, %10.7f, %10.7f \n",x[i],y[i],f_val);
#endif
			fprintf(outp,"%10.7f, %10.7f, %10.7f \n",x[i],y[i],f_val);

		}

		/* write functional values and the gradient sensitivities to the output file */

		for(int i=number_of_samples_with_only_f_eval; i<number_of_function_evals;i++ ){

			double xin[2];
			double xb[2];
			xb[0]=0.0;
			xb[1]=0.0;
			xin[0]=x[i]*(xe-xs)+xs;
			xin[1]=y[i]*(ye-ys)+ys;
			double f_val = test_function_adj(xin,xb);

#if 0
			printf("%10.7f, %10.7f, %10.7f, %10.7f, %10.7f\n",xin[0],xin[1],f_val,xb[0],xb[1]);
#endif
			fprintf(outp,"%10.7f, %10.7f, %10.7f, %10.7f, %10.7f\n",xin[0],xin[1],f_val,xb[0],xb[1]);

		}




		fclose(outp);




	}





	if(sampling_method == RANDOM_SAMPLING){
#if 0
		printf("Generating random samples ...\n");
#endif
		FILE *outp;
#if 0
		printf("opening file %s for input...\n",filename.c_str() );
#endif
		outp = fopen(filename.c_str(), "w");



		/* parameter bounds */
		double xs = bounds[0];
		double xe = bounds[1];

		double ys = bounds[2];
		double ye = bounds[3];


		vec x(number_of_function_evals);
		vec y(number_of_function_evals);

		/* generate random points */
		for(int i=0; i<number_of_function_evals;i++ ){

			x[i] = RandomDouble(xs, xe);
			y[i] = RandomDouble(ys, ye);


		}




		/* write functional values to the output file */
		for(int i=0; i<number_of_samples_with_only_f_eval;i++ ){

			double xin[2];
			xin[0]=x[i];
			xin[1]=y[i];
			double f_val = test_function(xin);
#if 0
			printf("%10.7f, %10.7f, %10.7f \n",x[i],y[i],f_val);
#endif
			fprintf(outp,"%10.7f, %10.7f, %10.7f \n",x[i],y[i],f_val);

		}


		/* write functional values and the gradient sensitivities to the output file */

		for(int i=number_of_samples_with_only_f_eval; i<number_of_function_evals;i++ ){

			double xin[2];
			double xb[2];
			xb[0]=0.0;
			xb[1]=0.0;
			xin[0]=x[i];
			xin[1]=y[i];
			double f_val = test_function_adj(xin,xb);

#if 0
			printf("%10.7f, %10.7f, %10.7f, %10.7f, %10.7f\n",x[i],y[i],f_val,xb[0],xb[1]);
#endif
			fprintf(outp,"%10.7f, %10.7f, %10.7f, %10.7f, %10.7f\n",x[i],y[i],f_val,xb[0],xb[1]);

		}



		fclose(outp);



	} /* end of random sampling */





}


void generate_1D_test_function_data_GEK(double (*test_function)(double *),
		double (*test_function_adj)(double *, double *),
		std::string filename,
		double *bounds,
		int number_of_samples_with_only_f_eval,
		int number_of_samples_with_g_eval,
		int sampling_method,
		double *locations_func,
		double *locations_grad){


	int number_of_function_evals  =  number_of_samples_with_only_f_eval+ number_of_samples_with_g_eval;


	if(sampling_method == PREDEFINED_LOCATIONS){


		double max = -LARGE;
		double min =  LARGE;

		for(int i=0; i<number_of_samples_with_only_f_eval;i++ ){

			if(locations_func[i] > max) max = locations_func[i];
			if(locations_func[i] < min) min = locations_func[i];
		}



		for(int i=0; i<number_of_samples_with_g_eval;i++ ){

			if(locations_grad[i] > max) max = locations_grad[i];
			if(locations_grad[i] < min) min = locations_grad[i];
		}




		FILE *outp;

		printf("opening file %s for input...\n",filename.c_str() );
		outp = fopen(filename.c_str(), "w");



		double x;

		/* write functional values to the output file */
		for(int i=0; i<number_of_samples_with_only_f_eval;i++ ){

			x = locations_func[i];

			double f_val = test_function(&x);

			fprintf(outp,"%10.7f, %10.7f\n",x,f_val);
			printf("%d %10.7f %10.7f\n",i,x,f_val);

		}

		double xb;

		/* write functional values and the gradient sensitivites to the output file */
		for(int i=0; i<number_of_samples_with_g_eval;i++ ){

			xb=0.0;

			x = locations_grad[i];
			double f_val = test_function_adj(&x,&xb);




			fprintf(outp,"%10.7f, %10.7f, %10.7f\n",x,f_val,xb);
			printf("%d %10.7f, %10.7f, %10.7f\n",i,x,f_val,xb);

		}



		fclose(outp);


	}



	if(sampling_method == RANDOM_SAMPLING){

		FILE *outp;

		printf("opening file %s for input...\n",filename.c_str() );
		outp = fopen(filename.c_str(), "w");



		double xs = bounds[0];
		double xe = bounds[1];


		vec x(number_of_samples_with_only_f_eval+number_of_samples_with_g_eval);

		for(int i=0; i<number_of_samples_with_only_f_eval+number_of_samples_with_g_eval;i++ ){

			x(i) = RandomDouble(xs, xe);


		}

		/* write functional values to the output file */
		for(int i=0; i<number_of_samples_with_only_f_eval;i++ ){

			double f_val = test_function(&x(i));

			fprintf(outp,"%10.7f, %10.7f\n",x(i),f_val);

		}

		double xb;

		/* write functional values and the gradient sensitivites to the output file */
		for(int i=number_of_samples_with_only_f_eval; i<number_of_samples_with_g_eval+number_of_samples_with_only_f_eval;i++ ){

			xb=0.0;

			double f_val = test_function_adj(&x(i),&xb);

			fprintf(outp,"%10.7f, %10.7f, %10.7f\n",x(i),f_val,xb);

		}

		fclose(outp);


	} /* end of random sampling */


}


void perform_trust_region_GEK_test(double (*test_function)(double *),
		double (*test_function_adj)(double *, double *),
		double *bounds,
		std::string function_name ,
		int  number_of_samples_with_only_f_eval,
		int number_of_samples_with_g_eval,
		int sampling_method,
		int method_for_solving_lin_eq,
		int dim,
		int linear_regression,
		std::string python_dir){

	if(dim <= 0){
		printf("Error: problem dimension must be greater than zero\n");
		exit(-1);
	}

	mat kriging_weights;
	mat regression_weights;
	mat R_inv_ys_min_beta;

	vec beta0(dim+1);

	double reg_param;


	int number_of_max_function_evals_for_training = 10000;

	std::string input_file_name = function_name+"_"+ std::to_string(number_of_samples_with_only_f_eval)+"_"+std::to_string(number_of_samples_with_g_eval)+ ".csv";

	printf("input file name : %s\n",input_file_name.c_str());



	if(dim ==2){
		/* generate the contour_plot */
		generate_contour_plot_2D_function_with_gradient(test_function_adj, bounds, function_name, python_dir);


	}


	if(dim == 2){
		printf("Generating inputs using %d points (%d gradient computations)...\n",number_of_samples_with_only_f_eval+ number_of_samples_with_g_eval,number_of_samples_with_g_eval);


		/* generate the input data for test	*/
		generate_2D_test_function_data_GEK(test_function,
				test_function_adj,
				input_file_name,
				bounds,
				number_of_samples_with_only_f_eval,
				number_of_samples_with_g_eval,
				sampling_method,
				python_dir);

	}
	else{

		/* generate the input data for test	*/
		generate_highdim_test_function_data_GEK(test_function,test_function_adj, input_file_name, bounds,dim,
				number_of_samples_with_only_f_eval,
				number_of_samples_with_g_eval,
				sampling_method );

	}




	double radius;

#if 0
	printf("Training the hyperparameters of the model...\n");
#endif
	/* train the response surfaces */
	train_TRGEK_response_surface(input_file_name,
			"None",
			linear_regression,
			regression_weights,
			kriging_weights,
			R_inv_ys_min_beta,
			radius,
			beta0,
			number_of_max_function_evals_for_training,
			dim,1);


}


void perform_kernel_regression_test_highdim_cuda(double (*test_function)(double *),
		double (*test_function_adj)(double *, double *),
		double *bounds,
		std::string function_name,
		int number_of_samples_with_only_f_eval,
		int number_of_samples_with_g_eval,
		int sampling_method,
		int dim){

	std::string datafilename;

	if (sampling_method == EXISTING_FILE){

		datafilename = function_name + ".csv";


	}
	else{

		datafilename = function_name+"_"+ std::to_string(number_of_samples_with_only_f_eval)+"_"
				+ std::to_string(number_of_samples_with_g_eval)+ ".csv";


	}

	float sigma = 0.01;
	float wSvd = 1.0;
	float w12 = 1.0;
	int max_cv_iter = 40;




	float partition[2] = {0.4,0.6};

	srand (time(NULL));
	printf("GPU test for kernel regression\n");

#if 0	
	cudaDeviceProp prop;
	int count;
	cudaGetDeviceCount( &count ) ;
	for (int i=0; i< count; i++) {
		cudaGetDeviceProperties( &prop, i);
		printf( "--- General Information for device %d ---\n", i );
		printf( "Name:%s\n", prop.name );
		printf( "Compute capability:%d.%d\n", prop.major, prop.minor );
		printf( "Total global mem:%ld\n", prop.totalGlobalMem );
		printf( "Total amount of constant memory: %lu bytes\n",
				prop.totalConstMem);


		printf( "Threads in warp:%d\n", prop.warpSize );
		printf( "Max threads per block:%d\n",prop.maxThreadsPerBlock );
		printf( "Max thread dimensions:(%d, %d, %d)\n",prop.maxThreadsDim[0], prop.maxThreadsDim[1],prop.maxThreadsDim[2] );
		printf( "Max grid dimensions:(%d, %d, %d)\n",prop.maxGridSize[0], prop.maxGridSize[1],prop.maxGridSize[2] );		

	}
#endif






	if(numVar!=dim) {

		printf("Mismatch between numVar and dim");
		exit(-1);
	}


	if ( sampling_method == EXISTING_FILE ){

		// do nothing

	}
	else{


		/* generate data only with functional values */

		if(number_of_samples_with_g_eval == 0) {

			int number_of_samples_to_generate = number_of_samples_with_only_f_eval;

			printf("Generating data with %d samples...\n", number_of_samples_to_generate);
			generate_highdim_test_function_data_cuda(test_function,
					datafilename,
					bounds,
					number_of_samples_to_generate,
					numVar);

		}

		else{

			/* generate data with gradients */

			generate_highdim_test_function_data_GEK(test_function,
					test_function_adj,
					datafilename,
					bounds,
					dim,
					number_of_samples_with_only_f_eval,
					number_of_samples_with_g_eval,
					sampling_method);



		}

		exit(1);



	} /* end of else */



	fmat data;
	bool load_ok = data.load(datafilename);
	data.print();


	if(load_ok == false)
	{
		printf("problem with loading the file %s\n",datafilename.c_str());
		exit(-1);
	}


	int number_of_data_points = data.n_rows;


	if(numVar != data.n_cols-1){

		printf("Number of columns in the input file does not match with numVar = %d\n",numVar);
		exit(-1);

	}

	printf("Kernel regression with the input data: %s\n",datafilename.c_str());
	printf("Data has %d samples with %d variables\n",number_of_data_points,numVar);


	printf("Original data:\n");
	data.print();


	data = shuffle(data);
#if 0
	printf("Original data (shuffled):\n");
	data.print();
#endif		

	int number_of_training_samples = number_of_data_points*partition[0];
	int number_of_test_samples = number_of_data_points - number_of_training_samples;


	printf("number of training samples = %d\n",number_of_training_samples);
	printf("number of test samples = %d\n",number_of_test_samples);



	fmat dataTraining = data.submat( 0, 0, number_of_training_samples-1, numVar );
	fmat dataTest     = data.submat( number_of_training_samples, 0, number_of_data_points-1, numVar );

	data.reset();

#if 1
	printf("Training data:\n");
	dataTraining.print();

	printf("Test data:\n");
	dataTest.print();	
#endif	



	fvec x_maxTraining(numVar);
	x_maxTraining.fill(0.0);

	fvec x_minTraining(numVar);
	x_minTraining.fill(0.0);

	for (int i = 0; i < numVar; i++) {

		x_maxTraining(i) = dataTraining.col(i).max();
		x_minTraining(i) = dataTraining.col(i).min();

	}

#if 1
	printf("maximum = \n");
	x_maxTraining.print();

	printf("minimum = \n");
	x_minTraining.print();
#endif
	/* normalize training data */
	for (int i = 0; i < number_of_training_samples; i++) {

		for (int j = 0; j < numVar; j++) {

			dataTraining(i, j) = (1.0/numVar)*(dataTraining(i, j) - x_minTraining(j)) / (x_maxTraining(j) - x_minTraining(j));
		}
	}


	float yTrainingMax = dataTraining.col(numVar).max();

	for (int i = 0; i < number_of_training_samples; i++) {

		dataTraining(i, numVar) = dataTraining(i, numVar)/yTrainingMax ;

	}



#if 1
	printf("Training data (normalized) = \n");
	dataTraining.print();
#endif	

	fvec x_maxTest(numVar);
	x_maxTest.fill(0.0);

	fvec x_minTest(numVar);
	x_minTest.fill(0.0);

	for (int i = 0; i < numVar; i++) {

		x_maxTest(i) = dataTest.col(i).max();
		x_minTest(i) = dataTest.col(i).min();

	}

#if 1
	printf("maximum = \n");
	x_maxTest.print();

	printf("minimum = \n");
	x_minTest.print();
#endif
	/* normalize test data */
	for (int i = 0; i < number_of_test_samples; i++) {

		for (int j = 0; j < numVar; j++) {

			dataTest(i, j) = (1.0/numVar)*(dataTest(i, j) - x_minTest(j)) / (x_maxTest(j) - x_minTest(j));
		}
	}

#if 1
	printf("Test data (normalized) = \n");
	dataTest.print();
#endif	



	fmat L(numVar,numVar);
	L.fill(0.0);






	trainMahalanobisDistance(L, dataTraining, sigma, wSvd, w12, max_cv_iter,L2_LOSS_FUNCTION);


	fmat M = L*trans(L);

	fmat U;
	fvec s;
	fmat V;

	svd(U,s,V,M);

#if 1

	printf("M = \n");
	M.print();
	printf("sigma = %10.7f\n",sigma);
	printf("singular values of M = \n");
	s.print();
#endif	

	/* compute generalization error */



	fmat XTest = dataTest.submat(0,0,number_of_test_samples-1,numVar-1);
	fvec yTest = dataTest.col(numVar);
	fmat XTraining = dataTraining.submat(0,0,number_of_training_samples-1,numVar-1);
	fvec yTraining = dataTraining.col(numVar);

	float genError = 0.0;

	for(int i=0;i <number_of_test_samples; i++){

		frowvec xp = XTest.row(i);
		float ytilde = kernelRegressor(XTraining, yTraining, xp, M, sigma)*yTrainingMax ;
		float yexact = yTest(i);

#if 1
		printf("x:\n");
		xp.print();
		printf("ytilde = %10.7f, yexact = %10.7f\n",ytilde,yexact);
#endif


		genError += (yexact-ytilde)*(yexact-ytilde);


	}

	genError = genError/number_of_test_samples;

	printf("genError = %10.7f\n",genError );

#if 0

	wSvd = 0.0;
	w12 = 1.0;


	trainMahalanobisDistance(L, dataTraining, sigma, wSvd, w12,max_cv_iter);


	M = L*trans(L);



	genError = 0.0;

	for(int i=0;i <number_of_test_samples; i++){

		rowvec xp = XTest.row(i);
		float ytilde = kernelRegressor(XTraining, yTraining, xp, M, sigma)*yTrainingMax;
		float yexact = yTest(i);

#if 0
		printf("x:\n");
		xp.print();
		printf("ytilde = %10.7f, yexact = %10.7f\n",ytilde,yexact);
#endif


		genError += (yexact-ytilde)*(yexact-ytilde);


	}

	genError = genError/number_of_test_samples;

	printf("genError = %10.7f\n",genError );

#endif	

	/* generalization error for M = I */

	M = eye<fmat>(numVar,numVar);

	genError = 0.0;

	for(int i=0;i <number_of_test_samples; i++){

		frowvec xp = XTest.row(i);
		float ytilde = kernelRegressor(XTraining, yTraining, xp, M, sigma)*yTrainingMax;
		float yexact = yTest(i);

#if 0
		printf("x:\n");
		xp.print();
		printf("ytilde = %10.7f, yexact = %10.7f\n",ytilde,yexact);
#endif


		genError += (yexact-ytilde)*(yexact-ytilde);


	}

	genError = genError/number_of_test_samples;

	printf("genError = %10.7f\n",genError );





	//	delete[] bounds;

}





void perform_rbf_test(double (*test_function)(double *),
		double *bounds,
		std::string function_name ,
		int  number_of_samples,
		int sampling_method,
		int problem_dimension,
		RBF_TYPE rbf_type)
{

	/* file name for data points in csv (comma separated values) format */
	std::string input_file_name = function_name+"_"
			+ std::to_string(number_of_samples )
	+".csv";

	printf("input file name : %s\n",input_file_name.c_str());



	/* generate the contour_plot for 2D data*/
	if(problem_dimension==2){
		generate_contour_plot_2D_function(test_function, bounds, function_name);
	}


	/* generate the function plot for 1D data*/
	if(problem_dimension==1){
		generate_plot_1D_function(test_function, bounds, function_name);

	}


	printf("Generating inputs using %d sample points...\n",number_of_samples );
	/* generate the input data for testing */
	generate_test_function_data(test_function,
			input_file_name,
			bounds,
			number_of_samples,
			sampling_method,
			problem_dimension);



	mat data; /* data matrix */
	data.load(input_file_name.c_str(), raw_ascii); /* force loading in raw_ascii format */

#if 0
	printf("data matrix = \n");
	data.print();
#endif


	int nrows = data.n_rows;
	int ncols = data.n_cols;

	/* number of input parameters in the problem */
	int dim = ncols - 1;

	mat X = data.submat(0, 0, nrows - 1, ncols - 2);

#if 0
	printf("X = \n");
	X.print();
#endif

	/* last column of the data matrix is the output vector */
	vec ys = data.col(dim);

#if 0
	printf("ys = \n");
	ys.print();
#endif


	vec x_max(dim);
	x_max.fill(0.0);

	vec x_min(dim);
	x_min.fill(0.0);

	for (int i = 0; i < dim; i++) {
		x_max(i) = data.col(i).max();
		x_min(i) = data.col(i).min();

	}

#if 0
	printf("maximum = \n");
	x_max.print();

	printf("minimum = \n");
	x_min.print();

#endif

	/* normalize data */
	for (int i = 0; i < nrows; i++) {

		for (int j = 0; j < dim; j++) {
			X(i, j) = (X(i, j) - x_min(j)) / (x_max(j) - x_min(j));
		}
	}


	/* weights of rbfs */
	vec w(nrows+1);


	/* train rbf weights */

#if 0
	printf("Training rbf...\n");
#endif

	double sigma=0.0;

	RBF_param model_parameters;
	model_parameters.type = rbf_type;

#if 0
	model_parameters.print();
#endif

	train_rbf(X, ys, w, sigma,model_parameters);

#if 0
	printf("Rbf weights =\n");
	w.print();
#endif



	double in_sample_error = 0.0;

	for(unsigned int i=0;i<X.n_rows;i++){

		rowvec xp = X.row(i);




		double func_val = calc_ftilde_rbf(X, xp, w, rbf_type, sigma);

		/* convert to original input vector */
		for(int j=0; j<problem_dimension;j++) {

			xp(j) = xp(j)* (x_max(j) - x_min(j))+x_min(j);
		}

		double func_val_exact = test_function(xp.memptr());

#if 0
		printf("xp = \n");
		xp.print();
		printf("\n");
		printf("ftilde = %10.7f fexact= %10.7f\n",func_val,func_val_exact );
#endif

		in_sample_error+= (func_val_exact-func_val)*(func_val_exact-func_val);

	}

	in_sample_error = in_sample_error/X.n_rows;

	printf("in sample error = %10.7f\n",in_sample_error);


	/* visualize with contour plot if the problem is 2D */
	if (problem_dimension == 2){
		int resolution =100;

		std::string rbf_response_surface_file_name = function_name+"_"+"rbf_response_surface_"+ std::to_string(number_of_samples )+".dat";

		FILE *rbf_response_surface_file = fopen(rbf_response_surface_file_name.c_str(),"w");

		double dx,dy; /* step sizes in x and y directions */
		rowvec x(2);
		rowvec xnorm(2);

		double func_val;
		dx = (bounds[1]-bounds[0])/(resolution-1);
		dy = (bounds[3]-bounds[2])/(resolution-1);

		double out_sample_error=0.0;

		x[0] = bounds[0];
		for(int i=0;i<resolution;i++){

			x[1] = bounds[2];
			for(int j=0;j<resolution;j++){

				/* normalize x */
				xnorm(0)= (x(0)- x_min(0)) / (x_max(0) - x_min(0));
				xnorm(1)= (x(1)- x_min(1)) / (x_max(1) - x_min(1));


				func_val = calc_ftilde_rbf(X, xnorm, w, rbf_type, sigma);

				fprintf(rbf_response_surface_file,"%10.7f %10.7f %10.7f\n",x(0),x(1),func_val);

				double func_val_exact = test_function(x.memptr());

				out_sample_error+= (func_val_exact-func_val)*(func_val_exact-func_val);

				x[1]+=dy;
			}
			x[0]+= dx;

		}
		fclose(rbf_response_surface_file);

		out_sample_error = out_sample_error/(resolution*resolution);

		printf("out of sample error = %10.7f\n",out_sample_error);

		/* plot the kriging response surface */

		std::string file_name_for_plot = function_name+"_"+"rbf_response_surface_";
		file_name_for_plot += "_"+std::to_string(resolution)+ "_"+std::to_string(resolution)+".png";

		std::string python_command = "python -W ignore plot_2d_surface.py "+ rbf_response_surface_file_name+ " "+ file_name_for_plot ;

		FILE* in = popen(python_command.c_str(), "r");

		fprintf(in, "\n");


	}

}


void test_norms(int dim){

	int number_of_points = 1000000;
	vec dist1(number_of_points);
	vec dist2(number_of_points);

	for(int i=0; i<number_of_points; i++){

		rowvec p1(dim);
		rowvec p2(dim);

		for(int j=0; j<dim;j++) {

			p1(j)= RandomDouble(0.0, 1.0);
			p2(j)= RandomDouble(0.0, 1.0);

		}
		rowvec diff = p1-p2;

		dist2(i) = L2norm(diff,dim);
		dist1(i) = L1norm(diff,dim);

		//		printf("%d L2 = %10.7f L1 = %10.7f\n",i,dist2(i),dist1(i));


	}


	double max2= max(dist2);
	double max1= max(dist1);

	dist2 = dist2/max2;
	dist1 = dist1/max1;

	double meanL2 = mean(dist2);
	double meanL1 = mean(dist1);
	double stddevL2 = stddev(dist2);
	double stddevL1 = stddev(dist1);

	printf("mean L2 = %10.7f\n",meanL2);
	printf("mean L1 = %10.7f\n",meanL1);
	printf("stddev L2 = %10.7f\n",stddevL2);
	printf("stddev L1 = %10.7f\n",stddevL1);






}


