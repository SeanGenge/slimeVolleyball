#include "DNA.h"

DNA::DNA(int numGenes)
{
	float max = 1.0f;

	for (int i = 0; i < numGenes; i++)
	{
		m_genes.push_back(Rand::randFloat(-max, max));
	}

	m_fitness = 0;
	m_mutationRate = 0.05f;
	m_crossOverChance = 0.01f;
	m_learningRate = 0.1;
}

vector<float> DNA::getGenes()
{
	return m_genes;
}

void DNA::setGenes(vector<float> &newGenes)
{
	m_genes = newGenes;
}

void DNA::setFitness(float newFitness)
{
	m_fitness = newFitness;
}

float DNA::getFitness()
{
	return m_fitness;
}

void DNA::crossOver(DNA *partner, float fitness)
{
	//Only cross over if the gene is performing very bad
	if (fitness < 0.0f)
	{
		for (int i = 0; i < m_genes.size(); i++)
		{
			if (Rand::randFloat() < m_crossOverChance)
			{
				m_genes[i] = partner->m_genes[i];
			}
		}
	}
}

void DNA::mutate(float fitness)
{
	//mutationRate depends on the fitness
	if (fitness < 0.0f)
	{
		m_mutationRate = 0.15f;
		m_learningRate = 0.7f;
	}
	else
	{
		//The mutation and learning rate depends on how well the fitness function is doing, min for both is 0.1f
		m_mutationRate = 1 / (fitness * 1.2f) < 0.02f ? 0.01f : 1 / (fitness * 1.2f);
		m_learningRate = 1 / (fitness * 0.5f) < 0.02f ? 0.01f : 1 / (fitness * 0.5f);
	}

	for (int i = 0; i < m_genes.size(); i++)
	{
		if (Rand::randFloat() < m_mutationRate)
		{
			float r = Rand::randGaussian() * m_learningRate;
			int x = 0;
			while (x <= 10 && (r + m_genes[i] > 1.0f || r + m_genes[i] < -1.0f))
			{
				r = Rand::randGaussian() * m_learningRate;
				m_learningRate = m_learningRate - 0.01 < 0 ? 0.01 : m_learningRate - 0.01;
				x++;
			}

			if (r + m_genes[i] > 1.0f || r + m_genes[i] < -1.0f)
			{
				m_genes[i] += r;
			}
		}
	}
}