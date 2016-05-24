#pragma once
#include <vector>
#include "cinder/Rand.h"

using namespace ci;
using namespace std;

class DNA
{
private:
	//Holds all the genes
	vector<float> m_genes;
	//The fitness of the dna
	float m_fitness;
	//The chance for a gene to mutate
	float m_mutationRate;
	//The chance that a gene will be crossed over with the partner
	float m_crossOverChance;
	//Prevent drastic changes in the dna
	float m_learningRate;

public:
	DNA(int numGenes);

	void setGenes(vector<float> &newGenes);
	vector<float> getGenes();

	void setFitness(float newFitness);
	float getFitness();

	void crossOver(DNA *partner, float fitness);
	void mutate(float fitness);

	bool operator<(const DNA &dna) const
	{
		return m_fitness < dna.m_fitness;
	}
};