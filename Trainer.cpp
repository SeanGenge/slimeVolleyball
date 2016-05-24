#include "Trainer.h"

vector<DNA*> createNewGeneticPool(int poolSize, int numGenes)
{
	vector<DNA*> pool;

	for (int i = 0; i < poolSize; i++)
	{
		pool.push_back(new DNA(numGenes));
	}
	
	return pool;
}

vector<DNA*> selectDNA(int numAgents, vector<DNA*> *pool)
{
	int num = 0;
	vector<DNA*> selected;

	while (num < numAgents)
	{
		int randNum = Rand::randInt(pool->size());

		selected.push_back(pool->at(randNum));
		pool->erase(pool->begin() + randNum);

		num++;
	}

	return selected;
}

vector<DNA*> createExistingGeneticPool(vector<DNA*> &pool, int geneLength, float &bestGenFitness)
{
	priority_queue<pair<float, DNA>, vector<pair<float, DNA*> > > fitness;
	vector<DNA*> newPool;
	//Pool size
	int elite = (int)(pool.size() * 0.2);
	int nonElite = pool.size() - elite;

	for (int i = 0; i < pool.size(); i++)
	{
		fitness.push(make_pair(pool[i]->getFitness(), pool[i]));
	}

	bestGenFitness = fitness.top().first;

	//Add the first 20% of the pool to newPool with no modifications to the dna
	for (int i = 0; i < elite; i++)
	{
		DNA *dna = fitness.top().second;
		//Copy the dna over
		DNA *eliteDna = new DNA(geneLength);
		eliteDna->setGenes(dna->getGenes());

		newPool.push_back(eliteDna);
		fitness.pop();
	}

	//Create a new vector where the fitness determines the amount of times the dna is in this new vector
	vector<DNA*> fitDna;

	for (int i = 0; i < pool.size(); i++)
	{
		int amount = (int)pool[i]->getFitness() * 2;
		
		if (amount <= 0)
		{
			amount = 1;
		}

		for (int x = 0; x < amount; x++)
		{
			fitDna.push_back(pool[i]);
		}
	}

	//For the remainder of newPool, cross the dna and mutate from pool
	for (int i = 0; i < nonElite; i++)
	{
		int randPartner1 = Rand::randInt(fitDna.size());
		int randPartner2 = Rand::randInt(fitDna.size());

		DNA* newDna = new DNA(geneLength);
		newDna->setGenes(fitDna[randPartner1]->getGenes());

		newPool.push_back(newDna);

		//Crossover and mutate
		newPool[newPool.size() - 1]->crossOver(fitDna[randPartner2], fitDna[randPartner1]->getFitness());
		newPool[newPool.size() - 1]->mutate(fitDna[randPartner1]->getFitness());
	}

	//Delete old pool's dna
	for (int i = 0; i < pool.size(); i++)
	{
		pool.erase(pool.begin() + i);
	}

	return newPool;
}