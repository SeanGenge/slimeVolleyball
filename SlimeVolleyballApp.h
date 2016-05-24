#pragma once
#include <vector>
#include <fstream>
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Resources.h"
#include "Agent.h"
#include "Ball.h"
#include "Fence.h"
#include "ui.h"
#include "DNA.h"
#include "Trainer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SlimeVolleyballApp : public App
{
private:
	//The max pause time. In seconds
	int m_maxPauseTime;
	//The current pause time. 0 means do not pause
	int m_currPauseTime;

	//Used to put the agents into training mode, this means there will be no input from a player. Two agents verse
	bool m_trainingMode;
	//The total number of simulations
	int m_maxSimulations;
	//The current number of simulations
	int m_numSimulations;
	//The number of times the agents versed each other
	int m_numAgentsVersed;
	int m_maxAgentsVersed;
	//The current generation
	int m_generationNum;
	//The number of genes
	int m_maxGeneLength;
	//Keeps track of the best last generation fitness
	float m_bestGenLastFitness;
	float m_bestTotalFitness;
	int m_bestGen;
	//Used to calculate the total time passed in milliseconds
	int m_totalTime;
	//Keeps track of how many times the ball has been hit
	int m_greenTouchedBall;
	int m_redTouchedBall;
	//The number of times the agents hit the ball over the fence
	int m_greenOverFence;
	int m_redOverFence;
	//Keeps track of the agents positions before and after their simulation. This contributes to their movement fitness
	int m_prevRedX;
	int m_currRedX;
	int m_prevGreenX;
	int m_currGreenX;
	//Checks to see if the ball has gone to the other side. 0 means in the middle. -1 is left and 1 is right
	int m_ballPrevSide;
	int m_ballCurrSide;
	bool m_drawObjects;
	//The genetic pool
	vector<DNA*> m_pool;
	vector<DNA*> m_trainingPool;
	//The two agents currently fighting
	vector<DNA*> m_currentAgents;

	Agent *m_greenSlime;
	Agent *m_redSlime;
	Ball *m_ball;
	Fence *m_fence;

	//The starting locations of the moving objects
	vec2 m_greenSlimeStartLoc;
	vec2 m_redSlimeStartLoc;
	vec2 m_ballStartLoc;

	//The background colour
	Color m_background;

	//The gravity
	vec2 m_gravity;

public:
	void setup() override;
	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;
	void update() override;
	void draw() override;

	void saveBestWeights();
	void saveAllWeights();

	//Pauses the game
	bool pauseGame();

	//Setups the game - Called every time a player loses
	void setupGame();
};

void prepareSettings(App::Settings* settings);