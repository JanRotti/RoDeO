/*
 * RoDeO, a Robust Design Optimization Package
 *
 * Copyright (C) 2015-2022 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (nicolas.gauger@scicomp.uni-kl.de) or Dr. Emre Özkaya (emre.oezkaya@scicomp.uni-kl.de)
 *
 * Lead developer: Emre Özkaya (SciComp, TU Kaiserslautern)
 *
 *  file is part of RoDeO
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


#include "ea_optimizer.hpp"
#include "matrix_vector_operations.hpp"
#include "random_functions.hpp"
#include "auxiliary_functions.hpp"
#define ARMA_DONT_PRINT_ERRORS
#include <armadillo>
#include<cassert>

using namespace arma;
using namespace std;


EAIndividual::EAIndividual(unsigned int dim){

	assert(dim >0);
	dimension = dim;
	genes = zeros<vec>(dim);

}

EAIndividual::EAIndividual(){


}

unsigned int EAIndividual::getId(void) const{

	return id;

}


void  EAIndividual::setId(unsigned int idGiven){

	id = idGiven;


}

unsigned int EAIndividual::getDimension(void) const{

	return dimension;

}

double EAIndividual::getObjectiveFunctionValue(void) const{

	return objectiveFunctionValue;

}

double EAIndividual::getFitnessValue(void) const{

	return fitness;

}

double EAIndividual::getReproductionProbability(void) const{

	return reproductionProbability;

}

double EAIndividual::getDeathProbability(void) const{

	return deathProbability;

}


void EAIndividual::setFitnessValue(double value){

	fitness = value;

}

void EAIndividual::setReproductionProbabilityValue(double value){

	assert(value>0);
	assert(value<1.0);
	reproductionProbability = value;


}

void EAIndividual::setDeathProbabilityValue(double value){

	assert(value>=0);
	assert(value<1.0);
	deathProbability = value;


}


vec EAIndividual::getGenes(void) const{

	return genes;

}

void EAIndividual::setGenes(vec input){

	genes = input;
}

void EAIndividual::initializeGenes(vec values){

	assert(dimension == values.size());
	genes = values;

}


void EAIndividual::setObjectiveFunctionValue(double value){

	objectiveFunctionValue = value;

}

void EAIndividual::print(void) const{

	std::cout<<"\n";
	printScalar(id);
	printVector(genes, "genes = ");
	printScalar(objectiveFunctionValue);
	printScalar(fitness);
	printScalar(reproductionProbability);
	printScalar(deathProbability);
	std::cout<<"\n";


}

void EAIndividual::initializeRandom(void){

	assert(dimension>0);
	genes = generateRandomVector(0.0,1.0,dimension);
	fitness = generateRandomDouble(0.0,1.0);
	objectiveFunctionValue = generateRandomDouble(0.0,1.0);
	reproductionProbability = generateRandomDouble(0.0,0.1);
	deathProbability = generateRandomDouble(0.0,0.1);

}



void EAPopulation::setDimension(unsigned int value){

	dimension = value;

}

unsigned int EAPopulation::getSize(void) const{

	return population.size();

}

void EAPopulation::addIndividual(EAIndividual itemToAdd){

	population.push_back(itemToAdd);

	double J = itemToAdd.getObjectiveFunctionValue();
	unsigned int id = itemToAdd.getId();

	updateMinAndMaxIfNecessary(J, id);

}


void EAPopulation::addAGroupOfIndividuals(std::vector<EAIndividual> individualsToAdd){

	for(auto it = std::begin(individualsToAdd); it != std::end(individualsToAdd); ++it) {

		addIndividual(*it);

	}

}

void EAPopulation::removeIndividual(unsigned int id){

	unsigned int index = getIndividualOrderInPopulationById(id);

	population.erase(population.begin() + index);

	if(id == idPopulationMaximum || id == idPopulationMinimum){

		updatePopulationMinAndMax();
	}


}



unsigned int EAPopulation::getIdOftheIndividual(unsigned int index) const{

	assert(index < getSize());
	return population[index].getId();

}

EAIndividual EAPopulation::getIndividual(unsigned int id) const{

	unsigned int order = getIndividualOrderInPopulationById(id);
	assert(order!=-1);
	return population.at(order);


}

EAIndividual EAPopulation::getTheBestIndividual(void) const{

	return getIndividual(idPopulationMinimum);

}

EAIndividual EAPopulation::getTheWorstIndividual(void) const{

	return getIndividual(idPopulationMaximum);


}


int  EAPopulation::getIndividualOrderInPopulationById(unsigned int id) const{


	unsigned int index = 0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		if(id == it->getId()){

			return index;
		}

		index++;
	}

	/* if the id coud not be found return -1 as failure */
	return -1;
}


void EAPopulation::print(void) const{

	std::ios oldState(nullptr);
	oldState.copyfmt(std::cout);
	std::setprecision(9);
	std::cout<<"\nEA Population has total "<< getSize() << " individuals ...\n";
	std::cout<<"fmin = "<<populationMinimum<<" fmax = "<<populationMaximum<<"\n";
	std::cout<<"fmin ID = "<<idPopulationMinimum<<" fmax ID = "<<idPopulationMaximum<<"\n";
	std::cout<<"ID    Obj. Fun.     Fitness        Rep. P.       Death P.\n";
	for(auto it = std::begin(population); it != std::end(population); ++it) {


		unsigned int id = it->getId();
		double J = it->getObjectiveFunctionValue();
		double fitness = it->getFitnessValue();
		double reproductionP = it->getReproductionProbability();
		double deathP = it->getDeathProbability();
		cout << scientific;
		std::cout << id << "   "<< std::showpos<< setw(6) << J <<"  "<< std::noshowpos<< setw(6) << fitness <<"  "<<setw(6) << reproductionP<<"  "<< setw(6) << deathP<<"\n";
	}

	std::cout.copyfmt(oldState);

}

void EAPopulation::updateMinAndMaxIfNecessary(double J, unsigned int id) {



	if (J > populationMaximum) {
		populationMaximum = J;
		idPopulationMaximum = id;

	}
	if (J < populationMinimum) {
		populationMinimum = J;
		idPopulationMinimum = id;
	}


}

void EAPopulation::updatePopulationMinAndMax(void){

	populationMaximum = -LARGE;
	populationMinimum =  LARGE;
	for(auto it = std::begin(population); it != std::end(population); ++it) {


		double J = it->getObjectiveFunctionValue();
		unsigned int id = it->getId();

		updateMinAndMaxIfNecessary(J, id);
	}




}

void EAPopulation::updateFitnessValuesLinear() {
	for (auto it = std::begin(population); it != std::end(population); ++it) {
		double fitnessValue = (populationMaximum
				- it->getObjectiveFunctionValue())
														/ (populationMaximum - populationMinimum);
		it->setFitnessValue(fitnessValue);
	}
}

void EAPopulation::updateFitnessValuesQuadratic() {
	vec coefficients =
			findPolynomialCoefficientsForQuadraticFitnessDistribution();
	for (auto it = std::begin(population); it != std::end(population); ++it) {
		double x = it->getObjectiveFunctionValue();
		double fitnessValue = coefficients(0) * x * x + coefficients(1) * x
				+ coefficients(2);
		it->setFitnessValue(fitnessValue);
	}
}

void EAPopulation::updateFitnessValues(void){


	/* we assume always minimization => maximum value has the worst fitness, minimum value has the best */


	if(polynomialForFitnessDistrubution == "linear"){

		updateFitnessValuesLinear();

	}

	if(polynomialForFitnessDistrubution == "quadratic"){


		updateFitnessValuesQuadratic();
	}



}




vec EAPopulation::findPolynomialCoefficientsForQuadraticFitnessDistribution(void) const{

	/* f(x) = ax^2 + bx + c
	 *
	 * f(populationMinimum)  = 1
	 * f(populationMaximum)  = aValueCloseButNotEqualToZero
	 * f'(populationMaximum) = 0
	 *
	 * */
	vec coefficients(3);
	mat A(3, 3, fill::zeros);
	double aValueCloseButNotEqualToZero = 0.001;

	A(0,0) = populationMinimum*populationMinimum;
	A(0,1) = populationMinimum;
	A(0,2) = 1.0;

	A(1,0) = populationMaximum*populationMaximum;
	A(1,1) = populationMaximum;
	A(1,2) = 1.0;

	A(2,0) = 2.0*populationMaximum;
	A(2,1) = 1.0;
	A(2,2) = 0.0;

	vec rhs(3);
	rhs(0) = 1.0 ;
	rhs(1) = aValueCloseButNotEqualToZero;
	rhs(2) = 0.0;

	coefficients = inv(A)*rhs;


	double f1 = coefficients(0)*populationMinimum*populationMinimum + coefficients(1) * populationMinimum + coefficients(2);
	assert(fabs(f1-1.0) < 10E-8);
	double f2 = coefficients(0)*populationMaximum*populationMaximum + coefficients(1) * populationMaximum + coefficients(2);
	assert(fabs(f2-aValueCloseButNotEqualToZero) < 10E-5);
	double fmin = 2*coefficients(0)*populationMaximum + coefficients(1);
	assert(fabs(fmin) < 10E-8);
	return coefficients;

}


void EAPopulation::updateReproductionProbabilities(void){

	double sumFitness = 0.0;

	for(auto it = std::begin(population); it != std::end(population); ++it) {

		sumFitness+=it->getFitnessValue();
	}

	assert(sumFitness > 0.0);


	for(auto it = std::begin(population); it != std::end(population); ++it) {

		double reproductionProbability = it->getFitnessValue()/sumFitness;
		it->setReproductionProbabilityValue(reproductionProbability);

	}


}


void EAPopulation::updateDeathProbabilities(void){

	vec fitnessValues(getSize());
	unsigned int i = 0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		fitnessValues(i) = it->getFitnessValue();
		i++;
	}

	vec P = { 0.9 };
	vec Q = quantile(fitnessValues, P);

	double best10PercentThreshold = Q(0);


	vec oneOverFitness(getSize());


	for(i=0; i<getSize(); i++) {

		if(fitnessValues(i) > best10PercentThreshold){

			oneOverFitness(i) = 0.0;
		}
		else{

			oneOverFitness(i) = 1.0/fitnessValues(i);

		}

	}

	double sumoneOverFitness = sum(oneOverFitness);

	i=0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {


		double deathProbability = oneOverFitness(i)/sumoneOverFitness;
		it->setDeathProbabilityValue(deathProbability );
		i++;
	}


}


void EAPopulation::updatePopulationProperties(void){

	assert(getSize());
	updateFitnessValues();
	updateReproductionProbabilities();
	updateDeathProbabilities();

}


EAIndividual EAPopulation::pickUpARandomIndividualForReproduction(void) const{




}






EAOptimizer2::EAOptimizer2(){

	setNumberOfThreads(1);

}





void EAOptimizer2::setMutationProbability(double value){

	assert(value>0.0 && value <1.0);
	mutationProbability = value;
	mutationProbabilityLastGeneration = value;

}



void EAOptimizer2::setInitialPopulationSize(unsigned int size){

	sizeOfInitialPopulation = size;

}


void EAOptimizer2::setNumberOfNewIndividualsInAGeneration(unsigned int number){

	numberOfNewIndividualsInAGeneration = number;

}

void EAOptimizer2::setNumberOfDeathsInAGeneration(unsigned int number){

	numberOfDeathsInAGeneration = number;

}

void EAOptimizer2::setNumberOfGenerations(unsigned int number){

	numberOfGenerations = number;

}

void EAOptimizer2::setMaximumNumberOfGeneratedIndividuals(unsigned int number){

	maximumNumberOfGeneratedIndividuals = number;

}








void EAOptimizer2::callObjectiveFunction(EAIndividual &individual){

	assert(dimension > 0);

	vec designVector = individual.getGenes();
	assert(designVector.size() == dimension);

	double objectiveFunctionValue = 0.0;

	if(ifObjectiveFunctionIsSet){
		objectiveFunctionValue = calculateObjectiveFunction(designVector);

	}

	else{

		objectiveFunctionValue = calculateObjectiveFunctionInternal(designVector);

	}

	individual.setObjectiveFunctionValue(objectiveFunctionValue);


}


EAIndividual  EAOptimizer2::generateRandomIndividual(void){



	assert(parameterBounds.areBoundsSet());
	EAIndividual newIndividual(dimension);

	vec randomGenes = parameterBounds.generateVectorWithinBounds();
	newIndividual.initializeGenes(randomGenes);
	newIndividual.setId(totalNumberOfGeneratedIndividuals);
	totalNumberOfGeneratedIndividuals++;

	callObjectiveFunction(newIndividual);


	return newIndividual;

}


void EAOptimizer2::initializePopulation(void){

	assert(sizeOfInitialPopulation>0);
	assert(areBoundsSet());


#pragma omp parallel
	{

		std::vector<EAIndividual> slavePopulation;

#pragma omp for nowait
		for (unsigned int i = 0; i < sizeOfInitialPopulation; ++i)
		{

			EAIndividual generatedIndividual = generateRandomIndividual();
			slavePopulation.push_back(generatedIndividual);
		}


#pragma omp critical
		{
			/* merge populations from each thread */
			population.addAGroupOfIndividuals(slavePopulation);


		}
	}


	population.updatePopulationProperties();

	ifPopulationIsInitialized = true;
}


void EAOptimizer2::printPopulation(void) const{

	population.print();

}
unsigned int EAOptimizer2::getPopulationSize(void) const{

	return population.getSize();
}




std::pair<EAIndividual, EAIndividual> EAOptimizer2::generateRandomParents(void) const{

	assert(ifPopulationIsInitialized);
	std::pair<EAIndividual, EAIndividual> parents;

	EAIndividual mother = population.pickUpARandomIndividualForReproduction();


	while (true) {

		EAIndividual father = population.pickUpARandomIndividualForReproduction();


		if (mother.getId() != father.getId()) {

			parents.first = mother;
			parents.second = father;
			break;
		}

	}

	return parents;
}







EAOptimizer::EAOptimizer(){

	setNumberOfThreads(1);

}




void EAOptimizer::callObjectiveFunction(EAIndividual &individual){

	assert(dimension > 0);

	vec designVector = individual.getGenes();
	assert(designVector.size() == dimension);

	double objectiveFunctionValue = 0.0;

	if(ifObjectiveFunctionIsSet){
		objectiveFunctionValue = calculateObjectiveFunction(designVector);

	}

	else{

		objectiveFunctionValue = calculateObjectiveFunctionInternal(designVector);

	}

	individual.setObjectiveFunctionValue(objectiveFunctionValue);



}



void EAOptimizer::setMutationProbability(double value){

	assert(value>0.0 && value <1.0);
	mutationProbability = value;
	mutationProbabilityLastGeneration = value;

}



void EAOptimizer::setInitialPopulationSize(unsigned int size){

	sizeOfInitialPopulation = size;

}


void EAOptimizer::setNumberOfNewIndividualsInAGeneration(unsigned int number){

	numberOfNewIndividualsInAGeneration = number;

}

void EAOptimizer::setNumberOfDeathsInAGeneration(unsigned int number){

	numberOfDeathsInAGeneration = number;

}

void EAOptimizer::setNumberOfGenerations(unsigned int number){

	numberOfGenerations = number;

}

void EAOptimizer::setMaximumNumberOfGeneratedIndividuals(unsigned int number){

	maximumNumberOfGeneratedIndividuals = number;

}

EAIndividual  EAOptimizer::generateRandomIndividual(void){



	assert(parameterBounds.areBoundsSet());
	EAIndividual newIndividual(dimension);

	vec randomGenes = parameterBounds.generateVectorWithinBounds();
	newIndividual.initializeGenes(randomGenes);
	newIndividual.setId(totalNumberOfGeneratedIndividuals);
	totalNumberOfGeneratedIndividuals++;

	callObjectiveFunction(newIndividual);


	return newIndividual;

}

unsigned int  EAOptimizer::getIndividualLocation(unsigned int id) const{

	assert(id < totalNumberOfGeneratedIndividuals);
	unsigned int index = 0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		if(id == it->getId()){

			return index;
		}

		index++;
	}

	assert(false);

}




void EAOptimizer::applyMutation(vec& inputGenes) {


	assert(mutationProbability > 0.0 && mutationProbability < 1.0);
	assert(parameterBounds.areBoundsSet());

	vec randomGenes = parameterBounds.generateVectorWithinBounds();

	for(unsigned int i=0; i<inputGenes.size(); i++){

		double aRandomNumberBetweenZeroAndOne =  generateRandomDouble(0.0,1.0);

		if(aRandomNumberBetweenZeroAndOne < mutationProbability){

			totalNumberOfMutations++;
			inputGenes(i) = randomGenes(i);


		}


	}

}

void EAOptimizer::applyBounds(vec& inputGenes) const{

	assert(areBoundsSet());
	vec lb = parameterBounds.getLowerBounds();
	vec ub = parameterBounds.getUpperBounds();
	assert(inputGenes.size() == lb.size());

	for(unsigned int i=0; i<lb.size(); i++){

		if(inputGenes(i) < lb(i))  inputGenes(i) = lb(i);
		if(inputGenes(i) > ub(i))  inputGenes(i) = ub(i);

	}



}

vec EAOptimizer::crossOverGenes(unsigned int motherId, unsigned int fatherId){

	assert(motherId  != fatherId);

	unsigned int motherIndex = getIndividualLocation(motherId);
	unsigned int fatherIndex = getIndividualLocation(fatherId);

	vec motherGenes = population[motherIndex].getGenes();
	vec fatherGenes = population[fatherIndex].getGenes();

	assert(motherGenes.size() == fatherGenes.size());

	vec newGenes = zeros<vec>(motherGenes.size());

	for(unsigned int i=0; i<motherGenes.size(); i++){

		newGenes(i) = generateRandomDoubleFromNormalDist(motherGenes(i), fatherGenes(i), 6.0);
		totalNumberOfCrossOvers++;

	}

	if(parameterBounds.isPointWithinBounds(newGenes) == false){

		applyBounds(newGenes);

	}

	applyMutation(newGenes);

	return newGenes;

}

EAIndividual  EAOptimizer::generateIndividualByReproduction(std::pair<unsigned int, unsigned int> indicesParents){

	assert(areBoundsSet());
	EAIndividual newIndividual(dimension);


	unsigned int motherID = indicesParents.first;
	unsigned int fatherID = indicesParents.second;

	vec newGenes = crossOverGenes(motherID, fatherID);

	newIndividual.setGenes(newGenes);
	newIndividual.setId(totalNumberOfGeneratedIndividuals);
	callObjectiveFunction(newIndividual);


	totalNumberOfGeneratedIndividuals++;

	return newIndividual;

}



void wait(void){

	for(unsigned int i=0;i<100000; i++){

		double a = 1.0*i;
		double b = a*a+1;

	}

}


void EAOptimizer::initializePopulation(void){

	assert(sizeOfInitialPopulation>0);
	assert(areBoundsSet());


#pragma omp parallel
	{

		std::vector<EAIndividual> slavePopulation;

#pragma omp for nowait
		for (unsigned int i = 0; i < sizeOfInitialPopulation; ++i)
		{

			EAIndividual generatedIndividual = generateRandomIndividual();
			slavePopulation.push_back(generatedIndividual);
		}


#pragma omp critical
		{
			/* merge populations from each thread */
			addToList(slavePopulation,population);

		}
	}


	sizeOfPopulation = population.size();

	updatePopulationProperties();
	findTheBestIndividualInPopulation();
	printTheBestIndividual();
	ifPopulationIsInitialized = true;
}



void EAOptimizer::printPopulation(void) const{

	std::ios oldState(nullptr);
	oldState.copyfmt(std::cout);
	std::setprecision(9);
	std::cout<<"\nEA Population has total "<< sizeOfPopulation << " individuals ...\n";
	std::cout<<"fmax = "<<populationMaximum<<"\n";
	std::cout<<"fmin = "<<populationMinimum<<"\n";
	std::cout<<"ID    Obj. Fun.     Fitness        Rep. P.       Death P.\n";
	for(auto it = std::begin(population); it != std::end(population); ++it) {


		unsigned int id = it->getId();
		double J = it->getObjectiveFunctionValue();
		double fitness = it->getFitnessValue();
		double reproductionP = it->getReproductionProbability();
		double deathP = it->getDeathProbability();
		cout << scientific;
		std::cout << id << "   "<< std::showpos<< setw(6) << J <<"  "<< std::noshowpos<< setw(6) << fitness <<"  "<<setw(6) << reproductionP<<"  "<< setw(6) << deathP<<"\n";
	}

	std::cout.copyfmt(oldState);

}

unsigned int EAOptimizer::getPopulationSize(void) const{

	return sizeOfPopulation;


}





vec EAOptimizer::getPopulationFitnessValues(void) const{

	vec fitness(sizeOfPopulation);
	unsigned int i = 0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		fitness(i) = it->getFitnessValue();
		i++;
	}

	return fitness;
}

vec EAOptimizer::getPopulationObjectiveFunctionValues(void) const{

	vec objectiveFunction(sizeOfPopulation);
	unsigned int i = 0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		objectiveFunction(i) = it->getObjectiveFunctionValue();
		i++;
	}

	return objectiveFunction;
}

vec EAOptimizer::getPopulationReproductionProbabilities(void) const{

	vec reproductionProbabilities(sizeOfPopulation);
	unsigned int i = 0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		reproductionProbabilities(i) = it->getReproductionProbability();
		i++;
	}

	return reproductionProbabilities;
}

vec EAOptimizer::getPopulationDeathProbabilities(void) const{

	vec deathProbabilities(sizeOfPopulation);
	unsigned int i = 0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		deathProbabilities(i) = it->getDeathProbability();
		i++;
	}

	return deathProbabilities;
}



void EAOptimizer::updatePopulationMinAndMax(void){

	vec objectiveFunctionValues = getPopulationObjectiveFunctionValues();

	populationMinimum = min(objectiveFunctionValues);
	populationMaximum = max(objectiveFunctionValues);

}





vec EAOptimizer::findPolynomialCoefficientsForQuadraticFitnessDistribution(void) const{

	/* f(x) = ax^2 + bx + c
	 *
	 * f(populationMinimum)  = 1
	 * f(populationMaximum)  = aValueCloseButNotEqualToZero
	 * f'(populationMaximum) = 0
	 *
	 * */
	vec coefficients(3);
	mat A(3, 3, fill::zeros);
	double aValueCloseButNotEqualToZero = 0.001;

	A(0,0) = populationMinimum*populationMinimum;
	A(0,1) = populationMinimum;
	A(0,2) = 1.0;

	A(1,0) = populationMaximum*populationMaximum;
	A(1,1) = populationMaximum;
	A(1,2) = 1.0;

	A(2,0) = 2.0*populationMaximum;
	A(2,1) = 1.0;
	A(2,2) = 0.0;

	vec rhs(3);
	rhs(0) = 1.0 ;
	rhs(1) = aValueCloseButNotEqualToZero;
	rhs(2) = 0.0;

	coefficients = inv(A)*rhs;


	double f1 = coefficients(0)*populationMinimum*populationMinimum + coefficients(1) * populationMinimum + coefficients(2);
	assert(fabs(f1-1.0) < 10E-8);
	double f2 = coefficients(0)*populationMaximum*populationMaximum + coefficients(1) * populationMaximum + coefficients(2);
	assert(fabs(f2-aValueCloseButNotEqualToZero) < 10E-5);
	double fmin = 2*coefficients(0)*populationMaximum + coefficients(1);
	assert(fabs(fmin) < 10E-8);
	return coefficients;

}

void EAOptimizer::updatePopulationFitnessValues(void){

	updatePopulationMinAndMax();

	vec objectiveFunctionValues = getPopulationObjectiveFunctionValues();


	/* we assume always minimization => maximum value has the worst fitness, minimum value has the best */

	unsigned int i=0;

	if(polynomialForFitnessDistrubution == "linear"){

		for(auto it = std::begin(population); it != std::end(population); ++it) {

			double fitnessValue = (populationMaximum - objectiveFunctionValues(i)) / (populationMaximum - populationMinimum);
			it->setFitnessValue(fitnessValue);
			i++;
		}

	}

	if(polynomialForFitnessDistrubution == "quadratic"){


		vec coefficients = findPolynomialCoefficientsForQuadraticFitnessDistribution();


		for(auto it = std::begin(population); it != std::end(population); ++it) {

			double x = objectiveFunctionValues(i);
			double fitnessValue = coefficients(0)*x*x + coefficients(1)*x + coefficients(2);
			it->setFitnessValue(fitnessValue);
			i++;
		}



	}



}

void EAOptimizer::updatePopulationReproductionProbabilities(void){

	vec fitnessValues = this->getPopulationFitnessValues();

	double sumFitness = sum(fitnessValues);

	unsigned int i=0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		double reproductionProbability = fitnessValues(i)/sumFitness;
		it->setReproductionProbabilityValue(reproductionProbability);
		i++;
	}


}

void EAOptimizer::updatePopulationDeathProbabilities(void){


	vec fitnessValues = getPopulationFitnessValues();

	vec P = { 0.9 };
	vec Q = quantile(fitnessValues, P);

	double best10PercentThreshold = Q(0);


	vec oneOverFitness(sizeOfPopulation);

	for(unsigned int i=0; i<sizeOfPopulation; i++) {

		if(fitnessValues(i) > best10PercentThreshold){

			oneOverFitness(i) = 0.0;
		}
		else{

			oneOverFitness(i) = 1.0/fitnessValues(i);
		}

	}


	double sumoneOverFitness = sum(oneOverFitness);

	unsigned int i=0;
	for(auto it = std::begin(population); it != std::end(population); ++it) {


		double deathProbability = oneOverFitness(i)/sumoneOverFitness;
		it->setDeathProbabilityValue(deathProbability );
		i++;
	}


}

void EAOptimizer::updatePopulationProperties(void){

	assert(sizeOfPopulation > 0);
	updatePopulationFitnessValues();
	updatePopulationReproductionProbabilities();
	updatePopulationDeathProbabilities();

}

unsigned int EAOptimizer::getIdOftheIndividual(unsigned int index) const{

	return population[index].getId();

}

void EAOptimizer::findTheBestIndividualInPopulation(void){

	vec objectiveFunctionValues = getPopulationObjectiveFunctionValues();

	unsigned int i = objectiveFunctionValues.index_min();

	idPopulationBest = getIdOftheIndividual(i);


}

void EAOptimizer::printTheBestIndividual(void) const{

	unsigned int index = getIndividualLocation(idPopulationBest);
	EAIndividual bestIndividual = population[index];

	output.printMessage("EA Optimization: Best individual at id = ",idPopulationBest);
	double bestObjectiveFunctionValue = bestIndividual.getObjectiveFunctionValue();
	output.printMessage("EA Optimization: Objective function value = ", bestObjectiveFunctionValue);
	output.printMessage("EA Optimization: Design vector" , bestIndividual.getGenes());

}

unsigned int EAOptimizer::pickUpARandomIndividual(void) const{

	unsigned int index = 0;
	double probabilitySum = 0.0;
	double randomNumberBetweenOneAndZero = generateRandomDouble(0.0,1.0);
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		probabilitySum += it->getReproductionProbability();


		if (randomNumberBetweenOneAndZero < probabilitySum) {

			index= it->getId();
			break;
		}
	}

	return index;


}


std::pair<unsigned int, unsigned int> EAOptimizer::generateRandomParents(void) const{

	assert(ifPopulationIsInitialized);
	std::pair<unsigned int, unsigned int> indicesOfParents;

	indicesOfParents.first = pickUpARandomIndividual();


	while (true) {

		indicesOfParents.second = pickUpARandomIndividual();


		if (indicesOfParents.first != indicesOfParents.second) break;

	}

	return indicesOfParents;
}

void EAOptimizer::addToList(std::vector<EAIndividual> &slavePopulation,
		std::vector<EAIndividual> &groupOfNewIndividualsToAdd) {
	/* merge populations from each thread */
	groupOfNewIndividualsToAdd.insert(groupOfNewIndividualsToAdd.end(),
			std::make_move_iterator(slavePopulation.begin()),
			std::make_move_iterator(slavePopulation.end()));
}

void EAOptimizer::addNewIndividualsToPopulation() {

	assert(numberOfNewIndividualsInAGeneration > 0);
	std::vector<EAIndividual> groupOfNewIndividualsToAdd;


#pragma omp parallel
	{

		std::vector<EAIndividual> slavePopulation;

#pragma omp for nowait
		for (unsigned int i = 0; i < numberOfNewIndividualsInAGeneration; ++i)
		{

			std::pair<unsigned int, unsigned int> indicesOfParents;
			indicesOfParents = generateRandomParents();
			EAIndividual aNewIndividual = generateIndividualByReproduction(indicesOfParents);

			slavePopulation.push_back(aNewIndividual);
		}


#pragma omp critical
		{

			/* merge populations from each thread */
			addToList(slavePopulation, groupOfNewIndividualsToAdd);
		}
	}



	addAGroupOfIndividualsToPopulation(groupOfNewIndividualsToAdd);

}

unsigned int EAOptimizer::pickUpAnIndividualThatWillDie(void) const{

	unsigned int index = 0;
	double probabilitySum = 0.0;
	double randomNumberBetweenOneAndZero = generateRandomDouble(0.0,1.0);
	for(auto it = std::begin(population); it != std::end(population); ++it) {

		probabilitySum += it->getDeathProbability();


		if (randomNumberBetweenOneAndZero < probabilitySum) {

			index= it->getId();
			break;
		}
	}

	return index;

}

void EAOptimizer::removeIndividualFromPopulation(unsigned int id){

	unsigned int index = getIndividualLocation(id);

	population.erase(population.begin() + index);
	sizeOfPopulation--;

}

void EAOptimizer::addAGroupOfIndividualsToPopulation(std::vector<EAIndividual> individualsToAdd){

	for(auto it = std::begin(individualsToAdd); it != std::end(individualsToAdd); ++it) {

		addIndividualToPopulation(*it);

	}

}

void EAOptimizer::addIndividualToPopulation(EAIndividual individualToAdd){

	population.push_back(individualToAdd);
	sizeOfPopulation++;

}



void EAOptimizer::removeIndividualsFromPopulation(void) {

	for(unsigned int i=0; i<numberOfDeathsInAGeneration; i++){

		unsigned int id = pickUpAnIndividualThatWillDie();

		removeIndividualFromPopulation(id);

		updatePopulationProperties();


	}



}



void EAOptimizer::generateNewGeneration(void){

	output.printMessage("EA Optimizer: Generating a new generation");
	output.printMessage("EA Optimizer: Adding some individuals to the population");
	addNewIndividualsToPopulation();
	updatePopulationProperties();
	output.printMessage("EA Optimizer: Removing some individuals from the population");
	removeIndividualsFromPopulation();
	output.printMessage("EA Optimizer: Updating population properties");
	updatePopulationProperties();

	findTheBestIndividualInPopulation();

}


void EAOptimizer::checkIfSettingsAreOk(void) const{

	assert(areBoundsSet());
	assert(parameterBounds.checkIfBoundsAreValid());
	assert(numberOfNewIndividualsInAGeneration>0);
	assert(sizeOfInitialPopulation>0);
	assert(dimension >0);
	assert(numberOfGenerations >0);
	assert(maximumNumberOfGeneratedIndividuals>0);
	assert(numberOfDeathsInAGeneration < sizeOfInitialPopulation);

}


void EAOptimizer::optimize(void){

	output.printMessage("EA Optimizer start...");
	checkIfSettingsAreOk();

	initializePopulation();
#if 0
	printPopulation();
#endif

	for(unsigned int i=0; i<numberOfGenerations; i++){

		output.printMessage("EA Optimizer: Generation = ",i);

		generateNewGeneration();

		if(totalNumberOfGeneratedIndividuals > maximumNumberOfGeneratedIndividuals) break;

	}


	printTheBestIndividual();

	double mutationCrossOverRatio = double(totalNumberOfMutations)/double(totalNumberOfCrossOvers);
	output.printMessage("EA Optimization: Effective mutation ratio =",mutationCrossOverRatio);

}


vec EAOptimizer::getBestDesignvector(void) const{

	unsigned int index = 0;

	index = getIndividualLocation(idPopulationBest);

	return population[index].getGenes();


}

double EAOptimizer::getBestObjectiveFunction(void) const{

	unsigned int index = 0;

	index = getIndividualLocation(idPopulationBest);

	return population[index].getObjectiveFunctionValue();



}
