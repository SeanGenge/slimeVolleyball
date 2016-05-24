#pragma once
#include <queue>
#include "DNA.h"

/*
	This file contains everything needed to train the network using genetic algorithms
*/

//Create the genetic pool with random values
vector<DNA*> createNewGeneticPool(int poolSize, int numGenes);
//Returns two random DNA from the pool. removes them from the list
vector<DNA*> selectDNA(int numAgents, vector<DNA*> *pool);
//Creates the new pool from the old pool, depends on the fitness
vector<DNA*> createExistingGeneticPool(vector<DNA*> &pool, int geneLength, float &bestGenFitness);
