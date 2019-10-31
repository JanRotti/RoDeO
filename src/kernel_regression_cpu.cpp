#include <codi.hpp>
#include <math.h>
#include <stdio.h>
#include <armadillo>
#include <iostream>
#include <stack>

#include "auxilliary_functions.hpp"
#include "Rodeo_macros.hpp"

using namespace std;
using namespace arma;


float MAX(float a, float b){

	if(a >=b) return a;
	else return b;


}

codi::RealReverse MAX(codi::RealReverse a, codi::RealReverse b){

	if(a >=b) return a;
	else return b;


}


codi::RealForward MAX(codi::RealForward a, codi::RealForward b){

	if(a >=b) return a;
	else return b;


}


/*
 * calculates the generalized Mahalanobis distance between two points
 *
 * @param[in] x_i : first vector
 * @param[in] X_j : second vector
 * @param[in] M : dim x dim matrix
 * @param[in] dim
 * @return distance
 *
 * */

float calcMetric(float *xi, float *xj, float *M, int dim) {

#if 0
	printf("calling calcMetric (primal)...\n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", M[i*dim+j]);

		}
		printf("\n");
	}

#endif



	float *diff = new float[dim];

	for (int i = 0; i < dim; i++) {

		diff[i] = xi[i] - xj[i];
	}
#if 0
	rowvec xi_val(dim);
	rowvec xj_val(dim);
	rowvec diff_val(dim);
	mat M_val(dim, dim);

	for (int i = 0; i < dim; i++) {
		xi_val(i) = xi[i];
		xj_val(i) = xj[i];
	}

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++)
			M_val(i, j) = M[i][j];

	diff_val = xi_val - xj_val;

	printf("diff_val=\n");
	diff_val.print();

	colvec diffT = trans(diff_val);

	vec matVecProd = M_val * diffT;
	printf("M * xdiff = \n");
	matVecProd.print();

	float metric_val = dot(diff_val, M_val * diffT);

	printf("metric_val = %10.7f\n", metric_val);
#endif

	float *tempVec = new float[dim];

	float sum = 0.0;

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			sum = sum + M[i*dim+j] * diff[j];
		}

		tempVec[i] = sum;
		sum = 0.0;

	}
#if 0
	printf("tempVec = \n");
	for(int i=0; i<dim; i++) {
		printf("%10.7f \n",tempVec[i] );

	}
#endif

	sum = 0.0;

	for (int i = 0; i < dim; i++) {

		sum = sum + tempVec[i] * diff[i];
	}
#if 0
	printf("sum = %10.7f\n",sum);
#endif

	delete[] diff;
	delete[] tempVec;


	if (sum < 0.0) {

		fprintf(stderr, "Error: metric is negative! at FILE = %s, LINE = %d.\n",__FILE__, __LINE__);
		exit(-1);
	}

	return sum;

}



float gaussianKernel(frowvec &xi, frowvec &xj, float sigma, fmat &M) {
#if 0
	printf("calling gaussianKernel...\n");
	xi.print();
	xj.print();
#endif

	/* calculate distance between xi and xj with the matrix M */
	float metricVal = calcMetric(xi, xj, M);
#if 0
	printf("metricVal = %10.7f\n",metricVal);
#endif

	float sqr_two_pi = sqrt(2.0 * datum::pi);

	float kernelVal = (1.0 / (sigma * sqr_two_pi))* exp(-metricVal / (2 * sigma * sigma));

	kernelVal += 10E-14;

#if 0
	printf("kernelVal = %10.7f\n",kernelVal);

#endif
	return kernelVal;

}


double gaussianKernel(rowvec &xi, rowvec &xj, double sigma, mat &M) {

	/* calculate distance between xi and xj with the matrix M */
	double metricVal = calcMetric(xi, xj, M);

	double sqr_two_pi = sqrt(2.0 * datum::pi);

	double kernelVal = (1.0 / (sigma * sqr_two_pi))* exp(-metricVal / (2 * sigma * sigma));

	kernelVal += 10E-14;

	return kernelVal;

}




float SIGN(float a, float b) {

	if (b >= 0.0) {
		return fabs(a);
	} else {
		return -fabs(a);
	}
}

codi::RealReverse SIGN(codi::RealReverse a, codi::RealReverse b) {

	if (b >= 0.0) {
		return fabs(a);
	} else {

		return -fabs(a);
	}
}

codi::RealForward SIGN(codi::RealForward a, codi::RealForward b) {

	if (b >= 0.0) {
		return fabs(a);
	} else {

		return -fabs(a);
	}
}

float PYTHAG(float a, float b) {
	float at = fabs(a), bt = fabs(b), ct, result;

	if (at > bt) {
		ct = bt / at;
		result = at * sqrt(1.0 + ct * ct);
	} else if (bt > 0.0) {
		ct = at / bt;
		result = bt * sqrt(1.0 + ct * ct);
	} else
		result = 0.0;
	return (result);
}

codi::RealReverse PYTHAG(codi::RealReverse a, codi::RealReverse b) {
	codi::RealReverse at = fabs(a), bt = fabs(b), ct, result;

	if (at > bt) {
		ct = bt / at;
		result = at * sqrt(1.0 + ct * ct);
	} else if (bt > 0.0) {
		ct = at / bt;
		result = bt * sqrt(1.0 + ct * ct);
	} else
		result = 0.0;
	return (result);
}
codi::RealForward PYTHAG(codi::RealForward a, codi::RealForward b) {
	codi::RealForward at = fabs(a), bt = fabs(b), ct, result;

	if (at > bt) {
		ct = bt / at;
		result = at * sqrt(1.0 + ct * ct);
	} else if (bt > 0.0) {
		ct = at / bt;
		result = bt * sqrt(1.0 + ct * ct);
	} else
		result = 0.0;
	return (result);
}



/** calculate regularization terms for the given matrix L
 *
 * @param[in]  L:  lower diagonal matrix
 * @param[in]  wSvd: weight for the svd regularization part
 * @param[in]  w12: weight for the mixed 12 regularization part
 * @param[out] regTerm
 *
 */

int calcRegTerms(float *L, float *regTerm, float wSvd, float w12, int dim) {
	int flag, i, its, j, jj, k, l = 0, nm;
	float c, f, h, s, x, y, z;
	float anorm = 0.0, g = 0.0, scale = 0.0;


	int m = dim;
	int n = dim;


	float **a;
	a = new float*[dim];

	for (i = 0; i < dim; i++) {

		a[i] = new float[dim];
	}

	float **M;
	M= new float*[dim];

	for (i = 0; i < dim; i++) {

		M[i] = new float[dim];
	}


	float **LT;
	LT = new float*[dim];
	for (int i = 0; i < dim; i++) {
		LT[i] = new float[dim];

	}

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {

			LT[i][j]=0.0;
		}




	for (int i = 0; i < dim; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = L[i*dim+j];
		}


	}

#if 0
	printf("L = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", L[i*dim+j]);

		}
		printf("\n");
	}
#endif


#if 0
	printf("LT = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", LT[i][j]);

		}
		printf("\n");
	}

#endif

	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{
			a[i][j]=0;
			M[i][j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
			for(int k = 0; k < dim; ++k)
			{
				M[i][j] += L[i*dim+k] * LT[k][j];

			}

	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{
			a[i][j]=M[i][j];

		}



#if 0
	printf("a = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", a[i][j]);

		}
		printf("\n");
	}

#endif

#if 0
	/* only for validation */
	mat Lval(dim,dim);
	mat LTval(dim,dim);
	mat aval(dim,dim);
	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {
			Lval(i,j) = Lin(i,j);
		}
	LTval = trans(Lval);
	aval = Lval*LTval;

	printf("aval = \n");
	aval.print();

#endif


	/* SVD part */

	float **v;
	v = new float*[n];

	for (i = 0; i < n; i++) {

		v[i] = new float[n];
	}
	float *w = new float[n];

	float *rv1 = new float[n];

	/* Householder reduction to bidiagonal form */
	for (i = 0; i < n; i++) {
		/* left-hand reduction */
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m) {
			for (k = i; k < m; k++)
				scale += fabs(a[k][i]);
			if (scale) {
				for (k = i; k < m; k++) {
					a[k][i] = (a[k][i] / scale);
					s += (a[k][i] * a[k][i]);
				}
				f = a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][i] = (f - g);
				if (i != n - 1) {
					for (j = l; j < n; j++) {
						for (s = 0.0, k = i; k < m; k++)
							s += (a[k][i] * a[k][j]);
						f = s / h;
						for (k = i; k < m; k++)
							a[k][j] += (f * a[k][i]);
					}
				}
				for (k = i; k < m; k++)
					a[k][i] = (a[k][i] * scale);
			}
		}
		w[i] = (scale * g);

		/* right-hand reduction */
		g = s = scale = 0.0;
		if (i < m && i != n - 1) {
			for (k = l; k < n; k++)
				scale += fabs(a[i][k]);
			if (scale) {
				for (k = l; k < n; k++) {
					a[i][k] = (a[i][k] / scale);
					s += (a[i][k] * a[i][k]);
				}
				f = a[i][l];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][l] = (f - g);
				for (k = l; k < n; k++)
					rv1[k] = a[i][k] / h;
				if (i != m - 1) {
					for (j = l; j < m; j++) {
						for (s = 0.0, k = l; k < n; k++)
							s += (a[j][k] * a[i][k]);
						for (k = l; k < n; k++)
							a[j][k] += (s * rv1[k]);
					}
				}
				for (k = l; k < n; k++)
					a[i][k] = (a[i][k] * scale);
			}
		}
		anorm = MAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	}

	/* accumulate the right-hand transformation */
	for (i = n - 1; i >= 0; i--) {
		if (i < n - 1) {
			if (g) {
				for (j = l; j < n; j++)
					v[j][i] = ((a[i][j] / a[i][l]) / g);
				/* float division to avoid underflow */
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < n; k++)
						s += (a[i][k] * v[k][j]);
					for (k = l; k < n; k++)
						v[k][j] += (s * v[k][i]);
				}
			}
			for (j = l; j < n; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}

	/* accumulate the left-hand transformation */
	for (i = n - 1; i >= 0; i--) {
		l = i + 1;
		g = w[i];
		if (i < n - 1)
			for (j = l; j < n; j++)
				a[i][j] = 0.0;
		if (g) {
			g = 1.0 / g;
			if (i != n - 1) {
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < m; k++)
						s += (a[k][i] * a[k][j]);
					f = (s / a[i][i]) * g;
					for (k = i; k < m; k++)
						a[k][j] += (f * a[k][i]);
				}
			}
			for (j = i; j < m; j++)
				a[j][i] = (a[j][i] * g);
		} else {
			for (j = i; j < m; j++)
				a[j][i] = 0.0;
		}
		++a[i][i];
	}

	/* diagonalize the bidiagonal form */
	for (k = n - 1; k >= 0; k--) { /* loop over singular values */
		for (its = 0; its < 30000; its++) { /* loop over allowed iterations */
			flag = 1;
			for (l = k; l >= 0; l--) { /* test for splitting */
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm) {
					flag = 0;
					break;
				}
				if (fabs(w[nm]) + anorm == anorm)
					break;
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm) {
						g = w[i];
						h = PYTHAG(f, g);
						w[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 0; j < m; j++) {
							y = a[j][nm];
							z = a[j][i];
							a[j][nm] = (y * c + z * s);
							a[j][i] = (z * c - y * s);
						}
					}
				}
			}
			z = w[k];
			if (l == k) { /* convergence */
				if (z < 0.0) { /* make singular value nonnegative */
					w[k] = (-z);
					for (j = 0; j < n; j++)
						v[j][k] = (-v[j][k]);
				}
				break;
			}
			if (its >= 30000) {
				delete[] rv1;
				fprintf(stderr, "No convergence after 30,000! iterations \n");
				return 1;
			}

			/* shift from bottom 2 x 2 minor */
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = PYTHAG(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;

			/* next QR transformation */
			c = s = 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = PYTHAG(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 0; jj < n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = (x * c + z * s);
					v[jj][i] = (z * c - x * s);
				}
				z = PYTHAG(f, h);
				w[j] = z;
				if (z) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 0; jj < m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = (y * c + z * s);
					a[jj][i] = (z * c - y * s);
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	delete[] rv1;

#if 0
	printf("singular values of a=\n");

	for (i = 0; i < n; i++) {

		printf("%10.7f\n",w[i]);
	}
#endif

	/* sort the singular values */

	float temp;
	for (i = 0; i < n; ++i) {
		for (j = i + 1; j < n; ++j) {

			if (w[i] < w[j])

			{
				temp = w[i];
				w[i] = w[j];
				w[j] = temp;
			}
		}
	}

#if 0
	printf("singular values of a=\n");


	for (i = 0; i < n; i++) {

		printf("%10.7f\n",w[i]);
	}
#endif

	/* normalization */
	float wsum = 0.0;
	for (i = 0; i < n; i++) {

		wsum += w[i];

	}

	for (i = 0; i < n; i++) {

		w[i] = w[i]/wsum;

	}

#if 0
	printf("singular values of a (normalized) with wsum =%10.7f\n",wsum);


	for (i = 0; i < n; i++) {

		printf("%15.10f\n",w[i]);
	}
#endif


	float svd_multiplier = (1.0*n*(1.0*n+1))/2.0;

	svd_multiplier = 1.0/svd_multiplier;
#if 0
	printf("svd_multiplier = %10.7f\n",svd_multiplier);
#endif
	float reg_term_svd = 0.0;

	for (i = 0; i < n; i++) {
#if 0
		printf("%d * %10.7f = %10.7f\n",i+1,w[i],(i+1)*w[i]);
#endif
		reg_term_svd = reg_term_svd + (i + 1) * w[i];
	}
#if 0
	printf("reg_term_svd = %10.7f\n",reg_term_svd);
#endif


	float reg_term_L1 = 0.0;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {

			reg_term_L1 = reg_term_L1 + M[i][j]* M[i][j];
		}
#if 0
	printf("reg_term_L1 = %10.7f\n",reg_term_L1);
#endif



	for (i = 0; i < n; i++) {
		delete[] v[i];
		delete[] a[i];
		delete[] M[i];
		delete[] LT[i];
	}


	delete[] LT;
	delete[] M;
	delete[] a;
	delete[] v;
	delete[] w;



	*regTerm = wSvd * svd_multiplier *reg_term_svd + w12 * reg_term_L1;
#if 0
	printf("result = %10.7f\n",*regTerm);
#endif

	return 0;




}

/* forward mode */

int calcRegTerms(float *L, float *regTerm,float *regTermd, float wSvd, float w12, int dim, int derIndx) {
	int flag, i, its, j, jj, k, l = 0, nm;
	codi::RealForward c, f, h, s, x, y, z;
	codi::RealForward anorm = 0.0, g = 0.0, scale = 0.0;


	int m = dim;
	int n = dim;

	codi::RealForward *Lcodi = new codi::RealForward[dim*dim];
	for (int i = 0; i < dim*dim; i++) {

		Lcodi[i] = L[i];

	}

	Lcodi[derIndx].setGradient(1.0);


	codi::RealForward **a;
	a = new codi::RealForward*[dim];

	for (i = 0; i < dim; i++) {

		a[i] = new codi::RealForward[dim];
	}

	codi::RealForward **M;
	M= new codi::RealForward*[dim];

	for (i = 0; i < dim; i++) {

		M[i] = new codi::RealForward[dim];
	}


	codi::RealForward **LT;
	LT = new codi::RealForward*[dim];
	for (int i = 0; i < dim; i++) {
		LT[i] = new codi::RealForward[dim];

	}

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {

			LT[i][j]=0.0;
		}




	for (int i = 0; i < dim; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = Lcodi[i*dim+j];
		}


	}
#if 0
	printf("LT = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", LT[i][j]);

		}
		printf("\n");
	}

#endif

	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{
			a[i][j]=0;
			M[i][j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
			for(int k = 0; k < dim; ++k)
			{
				M[i][j] += Lcodi[i*dim+k] * LT[k][j];

			}

	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{
			a[i][j]=M[i][j];

		}



#if 0
	printf("a = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", a[i][j]);

		}
		printf("\n");
	}

#endif




	/* SVD part */

	codi::RealForward **v;
	v = new codi::RealForward*[n];

	for (i = 0; i < n; i++) {

		v[i] = new codi::RealForward[n];
	}
	codi::RealForward *w = new codi::RealForward[n];

	codi::RealForward *rv1 = new codi::RealForward[n];

	/* Householder reduction to bidiagonal form */
	for (i = 0; i < n; i++) {
		/* left-hand reduction */
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m) {
			for (k = i; k < m; k++)
				scale += fabs(a[k][i]);
			if (scale!= 0) {
				for (k = i; k < m; k++) {
					a[k][i] = (a[k][i] / scale);
					s += (a[k][i] * a[k][i]);
				}
				f = a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][i] = (f - g);
				if (i != n - 1) {
					for (j = l; j < n; j++) {
						for (s = 0.0, k = i; k < m; k++)
							s += (a[k][i] * a[k][j]);
						f = s / h;
						for (k = i; k < m; k++)
							a[k][j] += (f * a[k][i]);
					}
				}
				for (k = i; k < m; k++)
					a[k][i] = (a[k][i] * scale);
			}
		}
		w[i] = (scale * g);

		/* right-hand reduction */
		g = s = scale = 0.0;
		if (i < m && i != n - 1) {
			for (k = l; k < n; k++)
				scale += fabs(a[i][k]);
			if (scale!=0) {
				for (k = l; k < n; k++) {
					a[i][k] = (a[i][k] / scale);
					s += (a[i][k] * a[i][k]);
				}
				f = a[i][l];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][l] = (f - g);
				for (k = l; k < n; k++)
					rv1[k] = a[i][k] / h;
				if (i != m - 1) {
					for (j = l; j < m; j++) {
						for (s = 0.0, k = l; k < n; k++)
							s += (a[j][k] * a[i][k]);
						for (k = l; k < n; k++)
							a[j][k] += (s * rv1[k]);
					}
				}
				for (k = l; k < n; k++)
					a[i][k] = (a[i][k] * scale);
			}
		}
		anorm = MAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	}

	/* accumulate the right-hand transformation */
	for (i = n - 1; i >= 0; i--) {
		if (i < n - 1) {
			if (g!=0) {
				for (j = l; j < n; j++)
					v[j][i] = ((a[i][j] / a[i][l]) / g);
				/* float division to avoid underflow */
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < n; k++)
						s += (a[i][k] * v[k][j]);
					for (k = l; k < n; k++)
						v[k][j] += (s * v[k][i]);
				}
			}
			for (j = l; j < n; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}

	/* accumulate the left-hand transformation */
	for (i = n - 1; i >= 0; i--) {
		l = i + 1;
		g = w[i];
		if (i < n - 1)
			for (j = l; j < n; j++)
				a[i][j] = 0.0;
		if (g!=0) {
			g = 1.0 / g;
			if (i != n - 1) {
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < m; k++)
						s += (a[k][i] * a[k][j]);
					f = (s / a[i][i]) * g;
					for (k = i; k < m; k++)
						a[k][j] += (f * a[k][i]);
				}
			}
			for (j = i; j < m; j++)
				a[j][i] = (a[j][i] * g);
		} else {
			for (j = i; j < m; j++)
				a[j][i] = 0.0;
		}
		++a[i][i];
	}

	/* diagonalize the bidiagonal form */
	for (k = n - 1; k >= 0; k--) { /* loop over singular values */
		for (its = 0; its < 30000; its++) { /* loop over allowed iterations */
			flag = 1;
			for (l = k; l >= 0; l--) { /* test for splitting */
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm) {
					flag = 0;
					break;
				}
				if (fabs(w[nm]) + anorm == anorm)
					break;
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm) {
						g = w[i];
						h = PYTHAG(f, g);
						w[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 0; j < m; j++) {
							y = a[j][nm];
							z = a[j][i];
							a[j][nm] = (y * c + z * s);
							a[j][i] = (z * c - y * s);
						}
					}
				}
			}
			z = w[k];
			if (l == k) { /* convergence */
				if (z < 0.0) { /* make singular value nonnegative */
					w[k] = (-z);
					for (j = 0; j < n; j++)
						v[j][k] = (-v[j][k]);
				}
				break;
			}
			if (its >= 30000) {
				delete[] rv1;
				fprintf(stderr, "No convergence after 30,000! iterations \n");
				return 1;
			}

			/* shift from bottom 2 x 2 minor */
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = PYTHAG(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;

			/* next QR transformation */
			c = s = 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = PYTHAG(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 0; jj < n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = (x * c + z * s);
					v[jj][i] = (z * c - x * s);
				}
				z = PYTHAG(f, h);
				w[j] = z;
				if (z!=0) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 0; jj < m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = (y * c + z * s);
					a[jj][i] = (z * c - y * s);
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	delete[] rv1;

#if 0
	printf("singular values of a=\n");

	for (i = 0; i < n; i++) {

		printf("%10.7f\n",w[i]);
	}
#endif

	/* sort the singular values */

	codi::RealForward temp;
	for (i = 0; i < n; ++i) {
		for (j = i + 1; j < n; ++j) {

			if (w[i] < w[j])

			{
				temp = w[i];
				w[i] = w[j];
				w[j] = temp;
			}
		}
	}

#if 0
	printf("singular values of a=\n");


	for (i = 0; i < n; i++) {

		printf("%10.7f\n",w[i].getValue());
	}
#endif

	/* normalization */
	codi::RealForward wsum = 0.0;
	for (i = 0; i < n; i++) {

		wsum += w[i];

	}

	for (i = 0; i < n; i++) {

		w[i] = w[i]/wsum;

	}

#if 0
	printf("singular values of a (normalized) with wsum =%10.7f\n",wsum.getValue());


	for (i = 0; i < n; i++) {

		printf("%15.10f\n",w[i].getValue());
	}
#endif


	float svd_multiplier = (1.0*n*(1.0*n+1))/2.0;

	svd_multiplier = 1.0/svd_multiplier;
#if 0
	printf("svd_multiplier = %10.7f\n",svd_multiplier);
#endif
	codi::RealForward reg_term_svd = 0.0;

	for (i = 0; i < n; i++) {
#if 0
		printf("%d * %10.7f = %10.7f\n",i+1,w[i].getValue(),(i+1)*w[i].getValue());
#endif
		reg_term_svd = reg_term_svd + (i + 1) * w[i];
	}
#if 0
	printf("reg_term_svd = %10.7f\n",reg_term_svd.getValue());
#endif


	codi::RealForward reg_term_L1 = 0.0;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {

			reg_term_L1 = reg_term_L1 + M[i][j]* M[i][j];
		}
#if 0
	printf("reg_term_L1 = %10.7f\n",reg_term_L1.getValue());
#endif



	for (i = 0; i < n; i++) {
		delete[] v[i];
		delete[] a[i];
		delete[] M[i];
		delete[] LT[i];
	}


	delete[] LT;
	delete[] M;
	delete[] a;
	delete[] v;
	delete[] w;


	codi::RealForward result = wSvd * svd_multiplier *reg_term_svd + w12 * reg_term_L1;
	*regTerm = result.getValue();

	*regTermd = result.getGradient();

	return 0;


}



int calcRegTerms(float *L, float *Lb,float *result , float wSvd, float w12, int dim) {
	int flag, i, its, j, jj, k, l = 0, nm;


	codi::RealReverse *Lcodi = new codi::RealReverse[dim*dim];
	for (int i = 0; i < dim*dim; i++) {

		Lcodi[i] = L[i];

	}

	/* activate tape and register input */

	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();

	codi::RealReverse regTerm=0.0;



	for (int i = 0; i < dim*dim; i++) {

		tape.registerInput(Lcodi[i]);

	}


	codi::RealReverse c, f, h, s, x, y, z;
	codi::RealReverse anorm = 0.0, g = 0.0, scale = 0.0;


	int m = dim;
	int n = dim;


	codi::RealReverse **a;
	a = new codi::RealReverse*[dim];

	for (i = 0; i < dim; i++) {

		a[i] = new codi::RealReverse[dim];
	}

	codi::RealReverse **M;
	M = new codi::RealReverse*[dim];

	for (i = 0; i < dim; i++) {

		M[i] = new codi::RealReverse[dim];
	}




	codi::RealReverse **LT;
	LT = new codi::RealReverse*[dim];
	for (int i = 0; i < dim; i++) {
		LT[i] = new codi::RealReverse[dim];

	}

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {

			LT[i][j]=0.0;
		}




	for (int i = 0; i < dim; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = Lcodi[i*dim+j];
		}


	}
#if 0
	printf("LT = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", LT[i][j].getValue());

		}
		printf("\n");
	}

#endif

	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{
			a[i][j]=0;
			M[i][j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
			for(int k = 0; k < dim; ++k)
			{
				M[i][j] += Lcodi[i*dim+k] * LT[k][j];

			}

	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{
			a[i][j]=M[i][j];

		}

#if 0
	printf("M = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", M[i][j].getValue());

		}
		printf("\n");
	}

#endif

#if 0
	printf("a = \n");

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {

			printf("%10.7f ", a[i][j].getValue());

		}
		printf("\n");
	}

#endif


	/* SVD part */

	codi::RealReverse **v;
	v = new codi::RealReverse*[n];

	for (i = 0; i < n; i++) {

		v[i] = new codi::RealReverse[n];
	}
	codi::RealReverse *w = new codi::RealReverse[n];

	codi::RealReverse *rv1 = new codi::RealReverse[n];

	/* Householder reduction to bidiagonal form */
	for (i = 0; i < n; i++) {
		/* left-hand reduction */
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m) {
			for (k = i; k < m; k++)
				scale += fabs(a[k][i]);
			if (scale != 0) {
				for (k = i; k < m; k++) {
					a[k][i] = (a[k][i] / scale);
					s += (a[k][i] * a[k][i]);
				}
				f = a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][i] = (f - g);
				if (i != n - 1) {
					for (j = l; j < n; j++) {
						for (s = 0.0, k = i; k < m; k++)
							s += (a[k][i] * a[k][j]);
						f = s / h;
						for (k = i; k < m; k++)
							a[k][j] += (f * a[k][i]);
					}
				}
				for (k = i; k < m; k++)
					a[k][i] = (a[k][i] * scale);
			}
		}
		w[i] = (scale * g);

		/* right-hand reduction */
		g = s = scale = 0.0;
		if (i < m && i != n - 1) {
			for (k = l; k < n; k++)
				scale += fabs(a[i][k]);
			if (scale !=0) {
				for (k = l; k < n; k++) {
					a[i][k] = (a[i][k] / scale);
					s += (a[i][k] * a[i][k]);
				}
				f = a[i][l];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][l] = (f - g);
				for (k = l; k < n; k++)
					rv1[k] = a[i][k] / h;
				if (i != m - 1) {
					for (j = l; j < m; j++) {
						for (s = 0.0, k = l; k < n; k++)
							s += (a[j][k] * a[i][k]);
						for (k = l; k < n; k++)
							a[j][k] += (s * rv1[k]);
					}
				}
				for (k = l; k < n; k++)
					a[i][k] = (a[i][k] * scale);
			}
		}
		anorm = MAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	}

	/* accumulate the right-hand transformation */
	for (i = n - 1; i >= 0; i--) {
		if (i < n - 1) {
			if (g !=0) {
				for (j = l; j < n; j++)
					v[j][i] = ((a[i][j] / a[i][l]) / g);
				/* float division to avoid underflow */
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < n; k++)
						s += (a[i][k] * v[k][j]);
					for (k = l; k < n; k++)
						v[k][j] += (s * v[k][i]);
				}
			}
			for (j = l; j < n; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}

	/* accumulate the left-hand transformation */
	for (i = n - 1; i >= 0; i--) {
		l = i + 1;
		g = w[i];
		if (i < n - 1)
			for (j = l; j < n; j++)
				a[i][j] = 0.0;
		if (g != 0) {
			g = 1.0 / g;
			if (i != n - 1) {
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < m; k++)
						s += (a[k][i] * a[k][j]);
					f = (s / a[i][i]) * g;
					for (k = i; k < m; k++)
						a[k][j] += (f * a[k][i]);
				}
			}
			for (j = i; j < m; j++)
				a[j][i] = (a[j][i] * g);
		} else {
			for (j = i; j < m; j++)
				a[j][i] = 0.0;
		}
		++a[i][i];
	}

	/* diagonalize the bidiagonal form */
	for (k = n - 1; k >= 0; k--) { /* loop over singular values */
		for (its = 0; its < 30000; its++) { /* loop over allowed iterations */
			flag = 1;
			for (l = k; l >= 0; l--) { /* test for splitting */
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm) {
					flag = 0;
					break;
				}
				if (fabs(w[nm]) + anorm == anorm)
					break;
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm) {
						g = w[i];
						h = PYTHAG(f, g);
						w[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 0; j < m; j++) {
							y = a[j][nm];
							z = a[j][i];
							a[j][nm] = (y * c + z * s);
							a[j][i] = (z * c - y * s);
						}
					}
				}
			}
			z = w[k];
			if (l == k) { /* convergence */
				if (z < 0.0) { /* make singular value nonnegative */
					w[k] = (-z);
					for (j = 0; j < n; j++)
						v[j][k] = (-v[j][k]);
				}
				break;
			}
			if (its >= 30000) {
				delete[] rv1;
				fprintf(stderr, "No convergence after 30,000! iterations \n");
				return 1;
			}

			/* shift from bottom 2 x 2 minor */
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = PYTHAG(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;

			/* next QR transformation */
			c = s = 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = PYTHAG(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 0; jj < n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = (x * c + z * s);
					v[jj][i] = (z * c - x * s);
				}
				z = PYTHAG(f, h);
				w[j] = z;
				if (z != 0) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 0; jj < m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = (y * c + z * s);
					a[jj][i] = (z * c - y * s);
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	delete[] rv1;

#if 0
	printf("singular values of a=\n");

	for (i = 0; i < n; i++) {

		printf("%10.7f\n",w[i]);
	}
#endif

	codi::RealReverse temp;
	for (i = 0; i < n; ++i) {
		for (j = i + 1; j < n; ++j) {

			if (w[i] < w[j])

			{
				temp = w[i];
				w[i] = w[j];
				w[j] = temp;
			}
		}
	}

#if 0
	printf("singular values of a=\n");


	for (i = 0; i < n; i++) {

		printf("%10.7f\n",w[i].getValue());
	}
#endif

	codi::RealReverse wsum = 0.0;
	for (i = 0; i < n; i++) {

		wsum += w[i];

	}

	for (i = 0; i < n; i++) {

		w[i] = w[i]/wsum;

	}

#if 0
	printf("singular values of a (normalized) with wsum =%10.7f\n",wsum.getValue());


	for (i = 0; i < n; i++) {

		printf("%15.10f\n",w[i].getValue());
	}
#endif


	codi::RealReverse svd_multiplier = (1.0*n*(1.0*n+1))/2.0;

	svd_multiplier = 1.0/svd_multiplier;
#if 0
	printf("svd_multiplier = %10.7f\n",svd_multiplier);
#endif
	codi::RealReverse reg_term_svd = 0.0;

	for (i = 0; i < n; i++) {
#if 0
		printf("%d * %10.7f = %10.7f\n",i+1,w[i].getValue(),(i+1)*w[i].getValue());
#endif
		reg_term_svd = reg_term_svd + (i + 1) * w[i];
	}
#if 0
	printf("reg_term_svd = %10.7f\n",reg_term_svd.getValue());
#endif


	codi::RealReverse reg_term_L1 = 0.0;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {

			reg_term_L1 = reg_term_L1 + M[i][j]* M[i][j];
		}
#if 0
	printf("reg_term_L1 = %10.7f\n",reg_term_L1.getValue());
#endif







	regTerm = wSvd * svd_multiplier *reg_term_svd + w12 * reg_term_L1;

#if 0
	printf("w12 * reg_term_L1 = %10.7f\n",w12 * reg_term_L1.getValue());
#endif


	tape.registerOutput(regTerm);

	tape.setPassive();
	regTerm.setGradient(1.0);
	tape.evaluate();

	for (int i = 0; i < dim*dim; i++) {


		Lb[i] = Lcodi[i].getGradient();

	}


	tape.reset();

	*result = regTerm.getValue();

	for (i = 0; i < n; i++) {

		delete[] v[i];
		delete[] a[i];
		delete[] M[i];
		delete[] LT[i];
	}


	delete[] LT;
	delete[] M;
	delete[] a;
	delete[] v;
	delete[] w;
	delete[] Lcodi;


	return 0;




}


/** calculate regularization terms for the given matrix L
 *
 * @param[in]  L:  lower diagonal matrix
 * @param[in]  w12: weight for the mixed 12 regularization part
 * @param[out] regTerm
 *
 */

int calcRegTermL12(float *L, float *regTerm, float w12, int dim) {




	float **M;
	M= new float*[dim];

	for (int i = 0; i < dim; i++) {

		M[i] = new float[dim];
	}


	float **LT;
	LT = new float*[dim];
	for (int i = 0; i < dim; i++) {
		LT[i] = new float[dim];

	}

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {

			LT[i][j]=0.0;
		}




	for (int i = 0; i < dim; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = L[i*dim+j];
		}


	}



	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{

			M[i][j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
			for(int k = 0; k < dim; ++k)
			{
				M[i][j] += L[i*dim+k] * LT[k][j];

			}


	float reg_term_L1 = 0.0;

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {

			reg_term_L1 = reg_term_L1 + M[i][j]* M[i][j];
		}
#if 0
	printf("reg_term_L1 = %10.7f\n",reg_term_L1);
#endif



	for (int i = 0; i < dim; i++) {
		delete[] M[i];
		delete[] LT[i];
	}


	delete[] LT;
	delete[] M;


	*regTerm = w12 * reg_term_L1;
#if 0
	printf("result = %10.7f\n",*regTerm);
#endif

	return 0;




}


int calcRegTermL12(float *L, float *Lb,float *result , float w12, int dim) {



	codi::RealReverse *Lcodi = new codi::RealReverse[dim*dim];
	for (int i = 0; i < dim*dim; i++) {

		Lcodi[i] = L[i];

	}

	/* activate tape and register input */

	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();

	codi::RealReverse regTerm=0.0;



	for (int i = 0; i < dim*dim; i++) {

		tape.registerInput(Lcodi[i]);

	}





	codi::RealReverse **M;
	M = new codi::RealReverse*[dim];

	for (int i = 0; i < dim; i++) {

		M[i] = new codi::RealReverse[dim];
	}




	codi::RealReverse **LT;
	LT = new codi::RealReverse*[dim];
	for (int i = 0; i < dim; i++) {
		LT[i] = new codi::RealReverse[dim];

	}

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {

			LT[i][j]=0.0;
		}




	for (int i = 0; i < dim; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = Lcodi[i*dim+j];
		}


	}

	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
		{

			M[i][j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < dim; ++i)
		for(int j = 0; j < dim; ++j)
			for(int k = 0; k < dim; ++k)
			{
				M[i][j] += Lcodi[i*dim+k] * LT[k][j];

			}







	codi::RealReverse reg_term_L1 = 0.0;

	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++) {

			reg_term_L1 = reg_term_L1 + M[i][j]* M[i][j];
		}


	regTerm = w12 * reg_term_L1;

#if 0
	printf("w12 * reg_term_L1 = %10.7f\n",w12 * reg_term_L1.getValue());
#endif


	tape.registerOutput(regTerm);

	tape.setPassive();
	regTerm.setGradient(1.0);
	tape.evaluate();

	for (int i = 0; i < dim*dim; i++) {


		Lb[i] = Lcodi[i].getGradient();

	}


	tape.reset();

	*result = regTerm.getValue();

	for (int i = 0; i < dim; i++) {

		delete[] M[i];
		delete[] LT[i];
	}


	delete[] LT;
	delete[] M;
	delete[] Lcodi;


	return 0;




}



void calculateKernelValues(float *X, float *kernelValTable, float *M, float sigma, int N, int d){
#if 0
	printf("calculateKernelValues...\n");
#endif

	const float sqr_two_pi = sqrt(2.0 * 3.14159265359);

	float diff[d];
	float tempVec[d];

	for(int indx1=0; indx1<N; indx1++){
		for(int indx2=indx1+1; indx2<N; indx2++){

#if 0
			printf("indx1 = %d, indx2 = %d\n",indx1,indx2);
#endif

			int off1 = indx1*(d+1);
			int off2 = indx2*(d+1);



			for (int k = 0; k < d; k++) {

				diff[k] = X[off1+k] - X[off2+k];
#if 0
				printf("diff[%d] = %10.7f\n",k,diff[k]);
#endif


			}





			float sum = 0.0;

			for (int i = 0; i < d; i++) {
				for (int j = 0; j < d; j++) {

					sum = sum + M[i*d+j] * diff[j];
				}

				tempVec[i] = sum;
				sum = 0.0;

			}


			sum = 0.0;

			for (int i = 0; i < d; i++) {

				sum = sum + tempVec[i] * diff[i];
			}




			float kernelVal = (1.0 / (sigma * sqr_two_pi))* exp(-sum / (2 * sigma * sigma)) + 10E-12;

			kernelValTable[indx1*N+indx2]= kernelVal;


		}
	}



}


void calculateKernelValues_b(float *X, float *kernelValTable, float *
		kernelValTableb, float *M, float *Mb, float sigma, float *sigmab, int
		N, int d) {
	const float sqr_two_pi = sqrt(2.0*3.14159265359);
	float diff[d];
	float diffb[d];
	float tempVec[d];
	float tempVecb[d];


	//	stack<float> stack;

	*sigmab=0.0;
	for (int i = 0; i < d*d; i++) {

		Mb[i]=0.0;
	}


	int ii1;
	for (int indx1 = 0; indx1 < N; ++indx1) {

		for (int indx2 = indx1+1; indx2 < N; ++indx2) {
			int off1 = indx1*(d+1);
			int off2 = indx2*(d+1);
			for (int k = 0; k < d; ++k) {
				//				stack.push(diff[k]);
				diff[k] = X[off1 + k] - X[off2 + k];
			}
			float sum = 0.0;
			for (int i = 0; i < d; ++i) {
				for (int j = 0; j < d; ++j)
					sum = sum + M[i*d+j]*diff[j];
				tempVec[i] = sum;
				sum = 0.0;
			}
			sum = 0.0;
			for (int i = 0; i < d; ++i) {
				sum = sum + tempVec[i]*diff[i];
			}

			float temp = 2*(sigma*sigma);
			float temp0 = sum/temp;
			float temp1 = exp(-temp0);
			float kernelVal = 1.0/(sigma*sqr_two_pi)*temp1 + 10E-12;
			//			stack.push(sum);
			//		}

			//	}

			for (ii1 = 0; ii1 < d; ++ii1)
				tempVecb[ii1] = 0.0;


			//	for (int indx1 = N-1; indx1 > -1; --indx1) {

			//		for (int indx2 = N-1; indx2 > indx1; --indx2) {
			//			int off1;
			//			int off2;
			//			float sum;
			float sumb = 0.0;
			//			float kernelVal;
			float kernelValb = 0.0;
			double tempb;

			float tempb0;
			//			sum = stack.top(); stack.pop();
			//             popReal4(&sum);
			kernelValb = kernelValTableb[indx1*N + indx2];
			kernelValTableb[indx1*N + indx2] = 0.0;
			tempb = kernelValb/(sqr_two_pi*sigma);

			tempb0 = -(exp(-temp0)*tempb/temp);
			sumb = tempb0;
			*sigmab = *sigmab - temp1*tempb/sigma - 2*2*temp0*sigma*tempb0;


			for (int i = d-1; i > -1; --i)
				tempVecb[i] = tempVecb[i] + diff[i]*sumb;
			for (int i = d-1; i > -1; --i) {
				sumb = tempVecb[i];
				tempVecb[i] = 0.0;
				for (int j = d-1; j > -1; --j)
					Mb[i*d + j] = Mb[i*d + j] + diff[j]*sumb;
			}
			//			for (int k = d-1; k > -1; --k) {
			//				diff[k] = stack.top();
			//				stack.pop();
			//			}
			//                 popReal4(&(diff[k]));
		}
	}

}


void calculateKernelValues(float *X, codi::RealReverse *kernelValTable, codi::RealReverse *M, codi::RealReverse sigma, int N, int d){

	const float sqr_two_pi = sqrt(2.0 * 3.14159265359);

	codi::RealReverse diff[d];
	codi::RealReverse tempVec[d];

	for(int indx1=0; indx1<N; indx1++){
		for(int indx2=indx1+1; indx2<N; indx2++){



			int off1 = indx1*(d+1);
			int off2 = indx2*(d+1);



			for (int k = 0; k < d; k++) {

				diff[k] = X[off1+k] - X[off2+k];


			}





			codi::RealReverse sum = 0.0;

			for (int i = 0; i < d; i++) {
				for (int j = 0; j < d; j++) {

					sum = sum + M[i*d+j] * diff[j];
				}

				tempVec[i] = sum;
				sum = 0.0;

			}


			sum = 0.0;

			for (int i = 0; i < d; i++) {

				sum = sum + tempVec[i] * diff[i];
			}




			codi::RealReverse kernelVal = (1.0 / (sigma * sqr_two_pi))* exp(-sum / (2 * sigma * sigma)) + 10E-12;

			kernelValTable[indx1*N+indx2]= kernelVal;


		}
	}



}

void calculateLossKernelL1(float* result, float *data,float *kernelValTable, int N, int d){


	float lossFunc = 0.0;

	for(int tid=0; tid<N; tid++){



		float kernelSum = 0.0;

		for(int i=0; i<N; i++){

			if(tid != i){

				int indxKernelValTable;
				if(i<tid) {


					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				kernelSum += kernelValTable[indxKernelValTable];

			}



		}

		float fapprox=0.0;
		for(int i=0; i<N; i++){

			if(tid != i){
				int indxKernelValTable;

				if(i<tid) {

					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				fapprox += (kernelValTable[indxKernelValTable]/kernelSum)* data[i*(d+1)+d];

			}




		}

		lossFunc += fabs(fapprox - data[tid*(d+1)+d]);

	}

	*result = lossFunc;

}


void calculateLossKernelL1_b(float *result, float *resultb, float *data, float
		*kernelValTable, float *kernelValTableb, int N, int d) {
	float lossFunc = 0.0;
	float lossFuncb = 0.0;
	int branch;

	stack<int> stackInt;
	stack<int> stackCont;
	stack<float> stackReal;


	for(int i=0; i<N*N; i++) {
		kernelValTableb[i]=0.0;
	}

	for (int tid = 0; tid < N; ++tid) {
		float kernelSum = 0.0;
		float fabs0;
		for (int i = 0; i < N; ++i){
			if (tid != i) {
				int indxKernelValTable;
				if (i < tid)
					indxKernelValTable = i*N + tid;
				else
					indxKernelValTable = tid*N + i;
				kernelSum = kernelSum + kernelValTable[indxKernelValTable];
				stackInt.push(indxKernelValTable);
				//				pushInteger4(indxKernelValTable);
				stackCont.push(1);
				//				pushControl1b(1);
			} else
				stackCont.push(0);
			//				pushControl1b(0);

		}
		float fapprox = 0.0;
		for (int i = 0; i < N; ++i){
			if (tid != i) {
				int indxKernelValTable;
				if (i < tid)
					indxKernelValTable = i*N + tid;
				else
					indxKernelValTable = tid*N + i;
				fapprox = fapprox + kernelValTable[indxKernelValTable]/
						kernelSum*data[i*(d+1)+d];
				stackInt.push(indxKernelValTable);
				//				pushInteger4(indxKernelValTable);
				//				pushControl1b(1);
				stackCont.push(1);
			} else
				//				pushControl1b(0);
				stackCont.push(0);

		}

		lossFunc += fabs(fapprox - data[tid*(d+1)+d]);

		if (fapprox - data[tid*(d+1) + d] >= 0.0)
			//			pushControl1b(0);
			stackCont.push(0);
		else
			//			pushControl1b(1);
			stackCont.push(1);

		stackReal.push(kernelSum);
		//		pushReal4(kernelSum);
	}
	lossFuncb = *resultb;
	*resultb = 0.0;
	//	*kernelValTableb = 0.0;
	for (int tid = N-1; tid > -1; --tid) {
		float kernelSum;
		float kernelSumb = 0.0;
		float fabs0;
		float fabs0b;
		float fapprox;
		float fapproxb = 0.0;
		kernelSum = stackReal.top(); stackReal.pop();
		//		popReal4(&kernelSum);
		fabs0b = lossFuncb;
		branch = stackCont.top(); stackCont.pop();
		if (branch == 0)
			fapproxb = fabs0b;
		else
			fapproxb = -fabs0b;
		kernelSumb = 0.0;
		for (int i = N-1; i > -1; --i) {
			branch = stackCont.top(); stackCont.pop();
			if (branch != 0) {
				int indxKernelValTable;
				float tempb;
				indxKernelValTable = stackInt.top(); stackInt.pop();
				//				popInteger4(&indxKernelValTable);
				tempb = data[i*(d+1)+d]*fapproxb/kernelSum;
				kernelValTableb[indxKernelValTable] = kernelValTableb[indxKernelValTable] + tempb;
				kernelSumb = kernelSumb - kernelValTable[indxKernelValTable]*tempb/kernelSum;
			}
		}
		for (int i = N-1; i > -1; --i) {
			branch = stackCont.top(); stackCont.pop();
			if (branch != 0) {
				int indxKernelValTable;
				//				popInteger4(&indxKernelValTable);
				indxKernelValTable = stackInt.top(); stackInt.pop();
				kernelValTableb[indxKernelValTable] = kernelValTableb[indxKernelValTable] + kernelSumb;
			}
		}
	}


	*result = lossFunc;

#if 0
	printf("kernelValTableb = \n");
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++) {
			printf("%10.7f ", kernelValTableb[i*N+j]);

		}

		printf("\n");
	}
#endif


}



void calculateLossKernelL1(codi::RealReverse* result, float *data,codi::RealReverse *kernelValTable, int N, int d){


	codi::RealReverse lossFunc = 0.0;

	for(int tid=0; tid<N; tid++){



		codi::RealReverse kernelSum = 0.0;

		for(int i=0; i<N; i++){

			if(tid != i){

				int indxKernelValTable;
				if(i<tid) {


					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				kernelSum += kernelValTable[indxKernelValTable];

			}



		}

		codi::RealReverse fapprox=0.0;
		for(int i=0; i<N; i++){

			if(tid != i){
				int indxKernelValTable;

				if(i<tid) {

					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				fapprox += (kernelValTable[indxKernelValTable]/kernelSum)* data[i*(d+1)+d];

			}




		}

		lossFunc += fabs(fapprox - data[tid*(d+1)+d]);

	}

	*result = lossFunc;

}








void calculateLossKernelL2(float* result, float *X,float *kernelValTable, int N, int d){


	float lossFunc = 0.0;

	for(int tid=0; tid<N; tid++){


		float kernelSum = 0.0;

		for(int i=0; i<N; i++){

			if(tid != i){

				int indxKernelValTable;
				if(i<tid) {


					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				kernelSum += kernelValTable[indxKernelValTable];

			}



		}

		float fapprox=0.0;
		for(int i=0; i<N; i++){

			if(tid != i){
				int indxKernelValTable;

				if(i<tid) {

					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				fapprox += (kernelValTable[indxKernelValTable]/kernelSum)* X[i*(d+1)+d];
#if 0
				printf("weight = %10.7f f = %10.7f\n", (kernelValTable[indxKernelValTable]/kernelSum),X[i*(d+1)+d]);
#endif

			}


		}


		lossFunc += (fapprox - X[tid*(d+1)+d]) * (fapprox - X[tid*(d+1)+d]);
#if 0
		printf("ftilde = %10.7f fexact = %10.7f\n", fapprox,X[tid*(d+1)+d]);
#endif




	}

	*result = lossFunc;

}


void calculateLossKernelL2_b(float *result, float *resultb, float *X, float *
		kernelValTable, float *kernelValTableb, int N, int d) {

	stack<int> stackInt;
	stack<int> stackCont;
	stack<float> stackReal;

	for(int i=0; i<N*N; i++) {
		kernelValTableb[i]=0.0;
	}


	float lossFunc = 0.0;
	float lossFuncb = 0.0;
	int branch;
	for (int tid = 0; tid < N; ++tid) {
		float kernelSum = 0.0;
		for (int i = 0; i < N; ++i){
			if (tid != i) {
				int indxKernelValTable;
				if (i < tid)
					indxKernelValTable = i*N + tid;
				else
					indxKernelValTable = tid*N + i;
				kernelSum = kernelSum + kernelValTable[indxKernelValTable];
				stackInt.push(indxKernelValTable);
				//				pushInteger4(indxKernelValTable);
				//				pushControl1b(1);
				stackCont.push(1);
			} else
				stackCont.push(0);
			//				pushControl1b(0);


		}
		float fapprox = 0.0;
		for (int i = 0; i < N; ++i){
			if (tid != i) {
				int indxKernelValTable;
				if (i < tid)
					indxKernelValTable = i*N + tid;
				else
					indxKernelValTable = tid*N + i;
				fapprox = fapprox + kernelValTable[indxKernelValTable]/
						kernelSum*X[i*(d+1)+d];
				stackInt.push(indxKernelValTable);
				//				pushInteger4(indxKernelValTable);
				//				pushControl1b(1);
				stackCont.push(1);
			} else
				//				pushControl1b(0);
				stackCont.push(0);

		}

		lossFunc += (fapprox - X[tid*(d+1)+d]) * (fapprox - X[tid*(d+1)+d]);

		//		pushReal4(kernelSum);
		//		pushReal4(fapprox);

		stackReal.push(kernelSum);
		stackReal.push(fapprox);

	}


	lossFuncb = *resultb;
	*resultb = 0.0;
	*kernelValTableb = 0.0;
	for (int tid = N-1; tid > -1; --tid) {
		float kernelSum;
		float kernelSumb = 0.0;
		float fapprox;
		float fapproxb = 0.0;

		fapprox = stackReal.top(); stackReal.pop();
		kernelSum = stackReal.top(); stackReal.pop();

		//		popReal4(&fapprox);
		//		popReal4(&kernelSum);
		fapproxb = 2*(fapprox-X[tid*(d+1)+d])*lossFuncb;
		kernelSumb = 0.0;
		for (int i = N-1; i > -1; --i) {
			//			popControl1b(&branch);
			branch = stackCont.top(); stackCont.pop();
			if (branch != 0) {
				int indxKernelValTable;
				float tempb;
				//				popInteger4(&indxKernelValTable);
				indxKernelValTable = stackInt.top(); stackInt.pop();
				tempb = X[i*(d+1)+d]*fapproxb/kernelSum;
				kernelValTableb[indxKernelValTable] = kernelValTableb[indxKernelValTable] + tempb;
				kernelSumb = kernelSumb - kernelValTable[indxKernelValTable]*tempb/kernelSum;
			}
		}
		for (int i = N-1; i > -1; --i) {
			//			popControl1b(&branch);
			branch = stackCont.top(); stackCont.pop();
			if (branch != 0) {
				int indxKernelValTable;
				indxKernelValTable = stackInt.top(); stackInt.pop();
				//				popInteger4(&indxKernelValTable);
				kernelValTableb[indxKernelValTable] = kernelValTableb[indxKernelValTable] + kernelSumb;
			}
		}
	}


	*result = lossFunc;
#if 0
	printf("kernelValTableb = \n");
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++) {
			printf("%10.7f ", kernelValTableb[i*N+j]);

		}

		printf("\n");
	}
#endif



}


void calculateLossKernelL2(codi::RealReverse* result, float *X,codi::RealReverse *kernelValTable, int N, int d){


	codi::RealReverse lossFunc = 0.0;

	for(int tid=0; tid<N; tid++){


		codi::RealReverse kernelSum = 0.0;

		for(int i=0; i<N; i++){

			if(tid != i){

				int indxKernelValTable;
				if(i<tid) {


					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				kernelSum += kernelValTable[indxKernelValTable];

			}



		}

		codi::RealReverse fapprox=0.0;
		for(int i=0; i<N; i++){

			if(tid != i){
				int indxKernelValTable;

				if(i<tid) {

					indxKernelValTable = i*N+tid;

				}
				else{

					indxKernelValTable = tid*N+i;

				}

				fapprox += (kernelValTable[indxKernelValTable]/kernelSum)* X[i*(d+1)+d];
#if 0
				printf("weight = %10.7f f = %10.7f\n", (kernelValTable[indxKernelValTable]/kernelSum),X[i*(d+1)+d]);
#endif

			}


		}


		lossFunc += (fapprox - X[tid*(d+1)+d]) * (fapprox - X[tid*(d+1)+d]);
#if 0
		printf("ftilde = %10.7f fexact = %10.7f\n", fapprox,X[tid*(d+1)+d]);
#endif




	}

	*result = lossFunc;

}



/*
 * calculates the loss function for the given training data
 *
 * @param[in] data : first vector
 * @param[in] input : Matrix M + sigma
 * @param[in] N : size of the training data
 * @param[in] d : problem dimension
 * @param[in] lossFunType :
 * @param[out] result
 *
 * */


void calcLossFunCPU(float *result, float *input, float *data,int N, int d,int lossFunType){


	float LT[d][d];
	float L[d][d];
	float M[d*d];





	for (int i = 0; i < d; i++)
		for (int j = 0; j < d; j++) {
			L[i][j]=input[i*d+j];

		}


	float sigma = input[d*d];



	for (int i = 0; i < d; i++)
		for (int j = 0; j < d; j++) {

			LT[i][j]=0.0;
		}




	for (int i = 0; i < d; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = L[i][j];
		}


	}


	for(int i = 0; i < d; ++i)
		for(int j = 0; j < d; ++j)
		{
			M[i*d+j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < d; ++i)
		for(int j = 0; j < d; ++j)
			for(int k = 0; k < d; ++k)
			{
				M[i*d+j] += L[i][k] * LT[k][j];

			}



	float *kernelValTable = new float[N*N];

	calculateKernelValues(data, kernelValTable, M, sigma, N, d);



	switch ( lossFunType )
	{
	case L1_LOSS_FUNCTION:
		calculateLossKernelL1(result,data,kernelValTable, N, d);
		break;
	case L2_LOSS_FUNCTION:
		calculateLossKernelL2(result, data,kernelValTable, N,d);
		break;
	default:
		printf("Error: Unkown lossFunType");
		exit(-1);
	}





#if 0
	printf("loss = %10.7f\n", *result);
#endif

	delete [] kernelValTable;

}

/*
 * calculates the loss function for the given training data
 *
 * @param[in] data : first vector
 * @param[in] input : Matrix M + sigma
 * @param[out] inputb : sensitivites w.r.t. Matrix M + sigma
 * @param[in] N : size of the training data
 * @param[in] d : problem dimension
 * @param[in] lossFunType :
 * @param[out] result
 *
 * */


void calcLossFunCPU_b(float *result, float *input, float *inputb, float *data,int N, int d,int lossFunType){


	float LT[d][d];
	float L[d][d];
	float LTb[d][d];
	float Lb[d][d];
	float M[d*d];
	float Mb[d*d];

	for (int i = 0; i < d*d+1; i++) {

		inputb[i] = 0.0;
	}



	for (int i = 0; i < d; i++)
		for (int j = 0; j < d; j++) {
			Lb[i][j]=0.0;
			L[i][j]=input[i*d+j];

		}


	float sigma = input[d*d];
	float sigmab = 0.0;


#if 0
	printf("Training data = \n");

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < d+1; j++) {

			printf("%10.7f ", data[i*(d+1)+j]);

		}
		printf("\n");
	}

#endif



#if 0
	printf("L = \n");

	for (int i = 0; i < d; i++) {
		for (int j = 0; j < d; j++) {

			printf("%10.7f ", L[i][j]);

		}
		printf("\n");
	}

#endif




	for (int i = 0; i < d; i++)
		for (int j = 0; j < d; j++) {

			LT[i][j]=0.0;
			LTb[i][j]=0.0;
		}




	for (int i = 0; i < d; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = L[i][j];
		}


	}
#if 0
	printf("LT = \n");

	for (int i = 0; i < d; i++) {
		for (int j = 0; j < d; j++) {

			printf("%10.7f ", LT[i][j]);

		}
		printf("\n");
	}

#endif

	for(int i = 0; i < d; ++i)
		for(int j = 0; j < d; ++j)
		{
			M[i*d+j]=0;
			Mb[i*d+j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < d; ++i)
		for(int j = 0; j < d; ++j)
			for(int k = 0; k < d; ++k)
			{
				M[i*d+j] += L[i][k] * LT[k][j];

			}
#if 0
	printf("M = \n");

	for (int i = 0; i < d; i++) {
		for (int j = 0; j < d; j++) {

			printf("%10.7f ", M[i*d+j]);

		}
		printf("\n");
	}

#endif


	float *kernelValTable = new float[N*N];
	float *kernelValTableb = new float[N*N];

	float resultb = 1.0;

	calculateKernelValues(data, kernelValTable, M, sigma, N, d);



	switch ( lossFunType )
	{
	case L1_LOSS_FUNCTION:
		calculateLossKernelL1_b(result, &resultb, data, kernelValTable, kernelValTableb, N, d);
		break;
	case L2_LOSS_FUNCTION:
		calculateLossKernelL2_b(result, &resultb, data, kernelValTable, kernelValTableb, N, d);
		break;
	default:
		printf("Error: Unkown lossFunType");
		exit(-1);
	}


#if 1
	printf("loss = %10.7f\n", *result);
#endif

	calculateKernelValues_b(data, kernelValTable, kernelValTableb, M, Mb, sigma, &sigmab, N, d);



	for (int i = d-1; i > -1; --i)
		for (int j = d-1; j > -1; --j)
			for (int k = d-1; k > -1; --k) {
				Lb[i][k] = Lb[i][k] + LT[k][j]*Mb[i*d+j];
				LTb[k][j] = LTb[k][j] + L[i][k]*Mb[i*d+j];
			}

	for (int i = d-1; i > -1; --i) {
		for (int j = i; j > -1; --j) {
			Lb[i][j] = Lb[i][j] + LTb[j][i];
			LTb[j][i] = 0.0;
		}
	}


	for (int i = d-1; i > -1; --i)
		for (int j = d-1; j > -1; --j) {
			inputb[i*d + j] = inputb[i*d + j] + Lb[i][j];
			Lb[i][j] = 0.0;
		}


	inputb[d*d] = sigmab;

#if 0
	printf("Lb =\n");
	for (int i = 0; i < d; i++){
		for (int j = 0; j < d; j++) {
			printf("%10.7f ",inputb[i*d+j]);

		}

		printf("\n");
	}

	printf("sigmab = %10.7f\n",sigmab);

#endif

#if 0

	/* validation part */

	float eps = 0.00001;
	float f0 = *result;
	float fp;
	float fdres[d*d+1];

	for(int i=0; i<d*d+1; i++){

		input[i]+=eps;

		calcLossFunCPU(&fp, input, data, N, d, lossFunType);
		fdres[i] = (fp - f0)/eps;
		input[i]-=eps;

	}


	printf("fd results =\n");
	for (int i = 0; i < d; i++){
		for (int j = 0; j < d; j++) {
			printf("%10.7f ",fdres[i*d+j]);

		}

		printf("\n");
	}

	printf("sigmab (fd) = %10.7f\n",fdres[d*d]);

#endif

	delete [] kernelValTable;
	delete [] kernelValTableb;


}

void calcLossFunCPUCodi(float *result, float *input, float *inputb, float *data,int N, int d,int lossFunType){


	codi::RealReverse *inputcodi = new codi::RealReverse[d*d+1];

	codi::RealReverse LT[d][d];
	codi::RealReverse L[d][d];
	codi::RealReverse M[d*d];

	codi::RealReverse *kernelValTable = new codi::RealReverse[N*N];


	/* activate tape and register input */

	codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
	tape.setActive();


	for (int i = 0; i < d*d+1; i++) {
		inputcodi[i] = input[i];
		tape.registerInput(inputcodi[i]);

	}



	codi::RealReverse resultCodi;


	for (int i = 0; i < d; i++)
		for (int j = 0; j < d; j++) {
			L[i][j]=inputcodi[i*d+j];

		}


	codi::RealReverse sigma = inputcodi[d*d];



	for (int i = 0; i < d; i++)
		for (int j = 0; j < d; j++) {

			LT[i][j]=0.0;
		}




	for (int i = 0; i < d; i++) {
		for (int j = 0; j <= i; j++){

			LT[j][i] = L[i][j];
		}


	}


	for(int i = 0; i < d; ++i)
		for(int j = 0; j < d; ++j)
		{
			M[i*d+j]=0;
		}

	/* Multiplying matrix L and LT and storing in M */
	for(int i = 0; i < d; ++i)
		for(int j = 0; j < d; ++j)
			for(int k = 0; k < d; ++k)
			{
				M[i*d+j] += L[i][k] * LT[k][j];

			}




	calculateKernelValues(data, kernelValTable, M, sigma, N, d);








	if(lossFunType == L1_LOSS_FUNCTION){

		calculateLossKernelL1(&resultCodi,data,kernelValTable, N, d);

	}

	if(lossFunType == L2_LOSS_FUNCTION){

		calculateLossKernelL2(&resultCodi, data,kernelValTable, N,d);

	}



	tape.registerOutput(resultCodi);

	tape.setPassive();
	resultCodi.setGradient(1.0);
	tape.evaluate();

	for (int i = 0; i < d*d+1; i++) {


		inputb[i] = inputcodi[i].getGradient();

	}




	*result = resultCodi.getValue();

#if 1
	printf("loss = %10.7f\n", *result);
#endif

#if 1


	printf("Lb =\n");
	for (int i = 0; i < d; i++){
		for (int j = 0; j < d; j++) {
			printf("%10.7f ",inputb[i*d+j]);

		}

		printf("\n");
	}

	printf("sigmab = %10.7f\n",inputb[d*d]);

#endif

	tape.reset();


	delete [] kernelValTable;

}



float kernelRegressorNotNormalized(fmat &X, fvec &y, frowvec &xp, vec& xmax, vec &xmin, fmat &M, float sigma) {

	int N = y.size();

	fvec kernelVal(N);
	fvec weight(N);

	frowvec xpNormalized;

	/* first normalize xp */

	for (int j = 0; j < xp.size(); j++) {

		xpNormalized(j) = (1.0/xp.size())*(xp(j) - xmin(j)) / (xmax(j) - xmin(j));
	}

	/* calculate the kernel values */

	float kernelSum = 0.0;
	for (int i = 0; i < N; i++) {

		frowvec xi = X.row(i);
		kernelVal(i) = gaussianKernel(xi, xpNormalized, sigma, M);
		kernelSum += kernelVal(i);
	}

	float yhat = 0.0;
	for (int i = 0; i < N; i++) {

		weight(i) = kernelVal(i) / kernelSum;
		yhat += y(i) * weight(i);
#if 0
		printf("y(%d) * weight(%d) = %10.7f * %10.7f\n",i,i,y(i),weight(i) );
#endif
	}

	return yhat;

}


/*
 * return kernel regression estimate
 * @param[in] X: sample input values (normalized)
 * @param[in] xp: point to be estimated
 * @param[in] M : Mahalanobis matrix
 * @param[in] sigma:  bandwidth parameter
 * @param[in] y: functional values at sample locations(normalized)
 *
 * */

float kernelRegressor(fmat &X, fvec &y, frowvec &xp, fmat &M, float sigma) {

	int N = y.size();

	fvec kernelVal(N);
	fvec weight(N);


	float kernelSum = 0.0;
	for (int i = 0; i < N; i++) {

		frowvec xi = X.row(i);
		kernelVal(i) = gaussianKernel(xi, xp, sigma, M);
		kernelSum += kernelVal(i);
	}



	float yhat = 0.0;
	for (int i = 0; i < N; i++) {

		weight(i) = kernelVal(i) / kernelSum;
		yhat += y(i) * weight(i);
#if 0
		printf("y(%d) * weight(%d) = %10.7f * %10.7f\n",i,i,y(i),weight(i) );
#endif
	}

	return yhat;

}





/*
 * return kernel regression estimate with gradient data
 * @param[in] X: sample input values (normalized)
 * @param[in] grad: sample gradient values (normalized)
 * @param[in] xp: point to be estimated
 * @param[in] M : Mahalanobis matrix
 * @param[in] sigma:  bandwidth parameter
 * @param[in] y: functional values at sample locations(normalized)
 *
 * */


float kernelRegressor(fmat &X, fvec &y, fmat &grad, frowvec &xp, fmat &M, float sigma) {

	int N = y.size();

	fvec kernelVal(N);
	fvec weight(N);
	float kernelSum = 0.0;
	float yhat = 0.0;




	/* first evaluate the kernel sum */
	for (int i = 0; i < N; i++) {

		frowvec xi = X.row(i);
		kernelVal(i) = gaussianKernel(xi, xp, sigma, M);
		kernelSum += kernelVal(i);


	}

#if 1

	int above99flag=0;
	int above99index = -1;
	for (int i = 0; i < N; i++) {

		weight(i) = kernelVal(i) / kernelSum;

		if(weight(i) > 0.5) {

			above99index = i;
			above99flag=1;
		}

	}

	if(above99flag == 1){
		printf("above99flag is 1 with index = %d!\n",above99index);
		for (int i = 0; i < N; i++) {

			if(i!=above99index) {

				weight(i) = 0.0;
			}
			else weight(i) = 1.0;
		}


	}


	//	weight.print();

#endif

	frowvec xdiff(xp.size());

	for (int i = 0; i < N; i++) {

		frowvec xi = X.row(i);

		for(int j=0; j<xp.size(); j++) xdiff(j) = xp(j) -xi(j);

		xdiff.print();

#if 0
		printf("xp =\n");
		xp.print();
		printf("xi =\n");
		xi.print();
		printf("xdiff =\n");
		xdiff.print();
		printf("grad =\n");
		grad.row(i).print();
#endif


		float gradTerm = dot(xdiff,grad.row(i));
#if 0
		printf("gradTerm = %10.7f\n",gradTerm);
		printf("y = %10.7f\n",y(i));
#endif



		//		weight(i) = kernelVal(i) / kernelSum;
		yhat += (y(i) + gradTerm) * weight(i);
		//		yhat += (y(i) ) * weight(i);
#if 0
		printf("y(%d) * weight(%d) = %10.7f * %10.7f\n",i,i,y(i),weight(i) );
#endif
	}


	printf("yhat = %10.7f\n",yhat);
	return yhat;

}


/*
 * return kernel regression estimate with gradient data
 * @param[in] X: sample input values (normalized)
 * @param[in] XnotNormalized: sample input values
 * @param[in] grad: sample gradient values (not normalized)
 * @param[in] xp: point to be estimated
 * @param[in] M : Mahalanobis matrix
 * @param[in] sigma:  bandwidth parameter
 * @param[in] y: functional values at sample locations
 *
 * */


float kernelRegressorNotNormalized(fmat &X,
		fmat &XnotNormalized,
		fvec &y,
		fmat &grad,
		frowvec &xp,
		fvec &xmin,
		fvec &xmax,
		fmat &M,
		float sigma) {


	/* number of samples */
	int N = y.size();
	int d = xp.size();

	fvec kernelVal(N);
	fvec weight(N);

	frowvec xpNormalized(d);

	/* first normalize xp */

	for (int j = 0; j < d; j++) {

		xpNormalized(j) = (1.0/d)*(xp(j) - xmin(j)) / (xmax(j) - xmin(j));
	}


	float kernelSum = 0.0;


	frowvec xi(d);
	frowvec xdiff(d);

	/* first evaluate the kernel sum */
	for (int i = 0; i < N; i++) {

		xi = X.row(i);

		kernelVal(i) = gaussianKernel(xi, xpNormalized, sigma, M);
		kernelSum += kernelVal(i);
	}



	float yhat = 0.0;

	for (int i = 0; i < N; i++) {


		xi = XnotNormalized.row(i);
		for(int j=0; j<d; j++) {

			xdiff(j) = xp(j) -xi(j);
		}


		float gradTerm = dot(xdiff,grad.row(i));

		weight(i) = kernelVal(i) / kernelSum;
		yhat += (y(i) + gradTerm) * weight(i);
#if 0
		printf("y(%d) * weight(%d) = %10.7f * %10.7f\n",i,i,y(i),weight(i) );
#endif
	}

	return yhat;

}


/*
 * return kernel regression estimate with gradient data
 * @param[in] X: sample input values (normalized)
 * @param[in] XnotNormalized: sample input values
 * @param[in] grad: sample gradient values (not normalized)
 * @param[in] xp: point to be estimated
 * @param[in] M : Mahalanobis matrix
 * @param[in] sigma:  bandwidth parameter
 * @param[in] y: functional values at sample locations
 *
 * */


double kernelRegressorNotNormalized(mat &X,
		mat &XnotNormalized,
		vec &y,
		mat &grad,
		rowvec &xp,
		vec &xmin,
		vec &xmax,
		mat &M,
		double sigma) {


	/* number of samples */
	int N = y.size();
	int d = xp.size();

	vec kernelVal(N);
	vec weight(N);

	rowvec xpNormalized(d);

	/* first normalize xp */

	for (int j = 0; j < d; j++) {

		xpNormalized(j) = (1.0/d)*(xp(j) - xmin(j)) / (xmax(j) - xmin(j));
	}


	double kernelSum = 0.0;


	rowvec xi(d);
	rowvec xdiff(d);

	/* first evaluate the kernel sum */
	for (int i = 0; i < N; i++) {

		xi = X.row(i);

		kernelVal(i) = gaussianKernel(xi, xpNormalized, sigma, M);
		kernelSum += kernelVal(i);
	}



	double yhat = 0.0;

	for (int i = 0; i < N; i++) {


		xi = XnotNormalized.row(i);
		for(int j=0; j<d; j++) {

			xdiff(j) = xp(j) -xi(j);
		}


		double gradTerm = dot(xdiff,grad.row(i));

		weight(i) = kernelVal(i) / kernelSum;
		yhat += (y(i) + gradTerm) * weight(i);
#if 0
		printf("y(%d) * weight(%d) = %10.7f * %10.7f\n",i,i,y(i),weight(i) );
#endif
	}

	return yhat;

}

int trainMahalanobisDistance(fmat &L, fmat &data, float &sigma, float &wSvd, float &w12,
		int max_cv_iter, int lossFunType, int batchsize, int nepochs) {


	bool trainWithSvdFlag = false;

	if(wSvd > 0.0) trainWithSvdFlag = true;


	int max_opt_iter = 40000;

	const float alpha = 0.9;

	unsigned int d = L.n_cols;

	unsigned int Ldim = d*d;

	/* lower diagonal matrix Lbest to keep the best L*/
	fmat bestL(d,d);
	bestL.fill(0.0);

	float bestsigma = 0.0;


	/* divide the data set into training and validation sets */

	unsigned int N = data.n_rows;


	/* size of the validation set, default to one fifth */
	unsigned int NvalidationSet = N/5;
	unsigned int Ntraining = N - NvalidationSet;


#if 1

	printf("number of training samples (core) = %d\n",Ntraining);
	printf("number of validation samples      = %d\n",NvalidationSet);

#endif


	/* divide the data into two sets */

	fmat dataTraining      = data.submat( 0, 0, Ntraining-1, d );
	fmat dataValidation    = data.submat( Ntraining, 0, N-1, d );


	fmat XValidation = dataValidation.submat(0,0,NvalidationSet-1,d-1);
	fvec yValidation = dataValidation.col(d);

	fmat XTraining = dataTraining.submat(0,0,Ntraining-1,d-1);
	fvec yTraining = dataTraining.col(d);




#if 0

	printf("Training data set = \n");
	dataTraining.print();

	printf("Validation data set = \n");
	dataValidation.print();
#endif


#if 0
	printf("XTraining = \n");
	XTraining.print();
	printf("yTraining = \n");
	yTraining.print();
#endif

#if 0
	printf("XValidation = \n");
	XValidation.print();
	printf("yValidation = \n");
	yValidation.print();
#endif

	fvec wSvdtrial(max_cv_iter);
	fvec w12trial(max_cv_iter);


	if(max_cv_iter !=1){



		for(int i=0; i<max_cv_iter; i++){

			wSvdtrial(i) = pow(10.0,RandomFloat(-2,0.0));
			w12trial(i) = pow(10.0,RandomFloat(-5,0.0));
		}


#if 1
		printf("wSvdtrial = \n");
		wSvdtrial.print();
		printf("w12trial = \n");
		w12trial.print();
#endif


	}


	float *inputVec = new float[Ldim+1]();
	float *inputVecVel = new float[Ldim+1]();
	float *inputVecLocalBest = new float[Ldim+1]();
	float *inputVecb = new float[Ldim+1]();
	float *inputVecRegb = new float[Ldim]();
	float *gradientVec = new float[Ldim+1]();
	float *dataVecTraining = new float[Ntraining*(d+1)]();




#if 0
	printf("L = \n");
	for (int i = 0; i < d; i++){
		for (int j = 0; j < d; j++) {

			printf("%10.7f ",inputVec[i*numVar+j]);
		}
		printf("\n");
	}

	printf("sigma = %10.7f\n",inputVec[Ldim]);
#endif


#if 1
	printf("copying training data...\n");
#endif
	for (int i = 0; i < Ntraining; i++) {

		for (int j = 0; j < d+1; j++) {

			dataVecTraining[i*(d+1)+j ] = dataTraining(i, j);
		}
	}
#if 0
	printf("data copied = \n");

	for (int i = 0; i < Ntraining; i++) {

		for (int j = 0; j < d+1; j++) {

			printf("%10.7f ",dataVecTraining[i*(n+1)+j ]);
		}
		printf("\n");
	}

#endif


	float optGenError = 10E14;

	/* cross validation loop to tune the weights for the regularization parameters */
	for(int iter_cv=0; iter_cv< max_cv_iter; iter_cv++){


		float learning_rateM = 0.0001;
		float learning_rateSigma = learning_rateM * 0.01;



		if(max_cv_iter !=1){

			if(trainWithSvdFlag){
				wSvd = wSvdtrial(iter_cv);
			}
			else{

				wSvd = 0.0;
			}
			w12 =  w12trial(iter_cv);
		}
#if 1
		printf("Outer iteration = %d\n",iter_cv);
		printf("wSvd = %10.7f, w12 = %10.7f\n",wSvd,w12);
#endif

		/* initialize the L matrix and sigma => everything is saved in the vector "inputVec" */

		for (int i = 0; i < d; i++)
			for (int j = 0; j < d; j++) {

				inputVec[i*d+j] = 0.0;
			}

		for (int i = 0; i < d; i++) {

			for (int j = 0; j <= i; j++) {

				if(i ==j) { /* main diagonal */

					inputVec[i*d+j] = 1.0+ RandomFloat(-0.1,0.1);
				}
				else {

					inputVec[i*d+j] = RandomFloat(0.0,0.1);
				}
			}
		}

		/* assign sigma to a small value between 0 and 0.1 */
		inputVec[Ldim] = RandomFloat(0.0,0.1);

		float lossVal,lossValb, regTerm;
		float objFunVal;
		lossVal = 0.0;
		lossValb = 1.0;


		/* init sensitivities to zero */
		for(int i=0;i<Ldim+1;i++) {

			inputVecb[i] = 0.0;
		}

		/* calculate the first gradient vector */

		printf("Evaluating the first gradient...\n");



		calcLossFunCPUCodi(&lossVal,inputVec,inputVecb, dataVecTraining,Ntraining,d, lossFunType);



		calcLossFunCPU_b(&lossVal,inputVec,inputVecb, dataVecTraining,Ntraining,d, lossFunType);

		exit(1);

		calcLossFunCPU(&lossVal, inputVec, dataVecTraining,Ntraining, d,lossFunType);





		//		calcLossFunCPU_b(&lossVal, &lossValb, inputVec,inputVecb, dataVecTraining,Ntraining, lossFunType);

		printf("initial Loss (CPU Version)= %10.7f\n", lossVal);


#if 0
		printf("gradient of the loss term = \n");

		for (int i = 0; i < d; i++) {
			for (int j = 0; j < d; j++) {

				printf("%10.7f ", inputVecb[i*d+j]);

			}
			printf("\n");
		}
		printf("sigma sensitivity = %10.7f\n", inputVecb[Ldim]);
#endif





		for(int i=0;i<Ldim+1;i++) {

			gradientVec[i]=inputVecb[i];
		}






#if 0
		printf("calculating regularization term...\n");
#endif
		for(int i=0;i<Ldim;i++) {

			inputVecRegb[i] = 0.0;
		}

		/* call the adjoint mode of the function to compute the regularization term */
		if(trainWithSvdFlag){

			calcRegTerms(inputVec, inputVecRegb, &regTerm, wSvd, w12, d);
		}
		else{

			calcRegTermL12(inputVec, inputVecRegb,&regTerm, w12, d);

		}

#if 0
		printf("gradient of the regularization term = \n");

		for (int i = 0; i < d; i++) {
			for (int j = 0; j < d; j++) {

				printf("%10.7f ", inputVecRegb[i*d+j]);

			}
			printf("\n");
		}
#endif

		objFunVal = lossVal + regTerm;

		printf("initial value of the objective function = %10.7f\n",objFunVal);

		/* add the regularization sensitivities to the gradient vector */

		for(int i=0;i<Ldim;i++) {

			gradientVec[i]+=inputVecRegb[i];
		}


#if 0

		/* validation loop for the regularization term */


		float f0 = 0.0;
		float tempSave;
		calcRegTerms(inputVec, &f0, wSvd, w12, n);
		printf("f0 = %10.7f\n",f0);
		float epsValReg= 0.001;


		for (int i = 0; i < n; i++) {
			for (int j = 0; j <= i; j++) {

				printf("validating the (%d,%d) th element of M\n",i,j);
				tempSave = inputVec[i*n+j];
				inputVec[i*n+j]+=epsValReg;

				float f1 = 0.0;

				calcRegTerms(inputVec, &f1, wSvd, w12, n);
				printf("f1 = %10.7f, f0 = %10.7f\n",f1,f0);
				inputVec[i*n+j]= tempSave;

				float fdVal = (f1-f0)/epsValReg;

				printf("fd value = %10.7f, ad value = %10.7f\n",fdVal,inputVecRegb[i*n+j]);

				float f2,f2d;

				/* call forward mode */
				calcRegTerms(inputVec, &f2,&f2d, wSvd, w12, n, i*n+j);

				printf("primal value = %10.7f, forward ad value = %10.7f, ad value = %10.7f\n",f2,f2d,inputVecRegb[i*n+j]);


			}

		}




#endif

		/* optimization loop */


		/* check gradient */
		for(int i=0;i<Ldim;i++) {

			if( gradientVec[i] != gradientVec[i]){

				printf("gradientVec[%d] is NaN!\n",i);
				exit(1);

			}
		}


		float objectiveFunLocalBest = 10E14;



		for(int opt_iter=0 ; opt_iter < max_opt_iter; opt_iter++){



			/* update M */

			for (int i = 0; i < d; i++){
				for (int j = 0; j <= i; j++) {

					inputVec[i*d+j]= inputVec[i*d+j] + inputVecVel[i*d+j];


				}

			}


			for (int i = 0; i < d; i++){
				for (int j = 0; j <= i; j++) {

					if ( inputVec[i*d+j] < 0) {

						inputVec[i*d+j] = 10E-6;

					}


				}

			}

			/* update sigma */
			inputVec[Ldim]= inputVec[Ldim] + inputVecVel[Ldim];

			if(inputVec[Ldim] <= 0) {

				inputVec[Ldim] = 10E-06;
			}


			for(int i=0;i<Ldim+1;i++) {

				inputVecb[i] = 0.0;
			}

			/* calculate the gradient vector */
#if 0
			printf("evaluating gradient vector...\n");
#endif



			//ceo			calcLossFunCPU_b(&lossVal, &lossValb, inputVec,inputVecb, dataVecTraining,Ntraining,L1_LOSS_FUNCTION);




#if 0
			printf("Loss (GPU Version)= %10.7f\n", lossVal);
#endif
			for(int i=0;i<Ldim+1;i++) {

				gradientVec[i]=inputVecb[i];
			}
#if 0
			printf("calculating the regularization term...\n");
#endif
			for(int i=0;i<Ldim;i++) {

				inputVecRegb[i] = 0.0;
			}

			/* call the adjoint mode of the function to compute the regularization term */

			if(trainWithSvdFlag){

				calcRegTerms(inputVec, inputVecRegb, &regTerm, wSvd, w12, d);
			}
			else{

				calcRegTermL12(inputVec, inputVecRegb,&regTerm, w12, d);

			}


#if 0
			printf("gradient of the regularization term = \n");

			for (int i = 0; i < numVar; i++) {
				for (int j = 0; j < numVar; j++) {

					printf("%10.7f ", inputVecRegb[i*numVar+j]);

				}
				printf("\n");
			}
#endif


			/* add the regularization sensitivities to the gradient vector */

			for(int i=0;i<Ldim;i++) {

				gradientVec[i]+=inputVecRegb[i];
			}


			objFunVal = lossVal + regTerm;

			if(objFunVal < objectiveFunLocalBest){

				objectiveFunLocalBest = objFunVal;

				for(int i=0;i<Ldim+1;i++) {

					inputVecLocalBest[i]=inputVec[i];

				}

			}


			if(opt_iter % 100 == 0){


				printf("iter = %d, objective function = %10.7f, Leave One Out Error = %10.7f, Regularization term = %10.7f\n",opt_iter,objFunVal,lossVal, regTerm);

#if 0
				printf("L = \n");

				for (int i = 0; i < d; i++) {
					for (int j = 0; j < d; j++) {

						printf("%10.7f ", inputVec[i*d+j]);

					}
					printf("\n");
				}

				printf("sigma = %10.7f\n",inputVec[Ldim]);
#endif



			}


			/* update velocity vector */
			for(int i=0;i<Ldim;i++) {

				inputVecVel[i]=alpha* inputVecVel[i] - learning_rateM*gradientVec[i];

			}
			inputVecVel[Ldim]=alpha* inputVecVel[Ldim] - learning_rateSigma*gradientVec[Ldim];




		} /* end of local optimization loop */



		for (int i = 0; i < d; i++)
			for (int j = 0; j < d; j++) {

				L(i,j)= inputVecLocalBest[i*d+j];
			}

#if 1
		printf("local optimization result:\n");
		printf("L = \n");
		L.print();
		printf("sigma = %10.7f\n", inputVecLocalBest[Ldim]);

#endif
		sigma = inputVecLocalBest[Ldim];


		fmat M = L*trans(L);
#if 1
		printf("M = \n");
		M.print();
#endif
		float genError = 0.0;

		for(int i=0;i <NvalidationSet; i++){

			frowvec xp = XValidation.row(i);
			float ytilde = kernelRegressor(XTraining, yTraining, xp, M, sigma);
			float yexact = yValidation(i);

#if 0
			printf("x:\n");
			xp.print();
			printf("ytilde = %10.7f, yexact = %10.7f\n",ytilde,yexact);
#endif



			if( lossFunType == L1_LOSS_FUNCTION) genError += fabs(yexact-ytilde);
			if( lossFunType == L2_LOSS_FUNCTION) genError += (yexact-ytilde)*(yexact-ytilde);

		}

		genError = genError/NvalidationSet;

#if 1
		printf("Generalization error = %10.7f\n",genError);
#endif
		if(genError < optGenError) {

#if 1
			printf("Better L has been found, updating L...\n");
#endif
			bestL = L;
			bestsigma = sigma;
			optGenError = genError;


		}



	} /* end of cv loop */

	L = bestL;
	sigma = bestsigma;


	delete[] inputVec;
	delete[] inputVecb;
	delete[] inputVecRegb;
	delete[] dataVecTraining;
	delete[] gradientVec;
	return 0;

}


