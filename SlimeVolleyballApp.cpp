#include "SlimeVolleyballApp.h"

void prepareSettings(App::Settings* settings)
{
	settings->setWindowSize(1024, 600);
	//Used for training
	settings->setFrameRate(1000.0f);
}

void SlimeVolleyballApp::setup()
{
	//Allow the program to go at a much faster framerate
	gl::enableVerticalSync(false);

	//Init variables
	m_background = Color(0.78f, 0.824f, 0.839f);
	m_gravity = vec2(0, 0.45f);
	m_greenSlimeStartLoc = vec2(150, app::getWindowHeight());
	m_redSlimeStartLoc = vec2(app::getWindowWidth() - 150, app::getWindowHeight());
	m_ballStartLoc = vec2(app::getWindowWidth() / 2, app::getWindowHeight() / 2 - 150);
	m_maxPauseTime = 45;
	m_currPauseTime = m_maxPauseTime;
	m_drawObjects = true;

	//Init objects
	m_fence = new Fence(app::getWindowWidth() / 2, app::getWindowHeight());
	m_greenSlime = new Agent(m_greenSlimeStartLoc);
	m_redSlime = new Agent(m_redSlimeStartLoc);
	m_ball = new Ball(m_ballStartLoc);

	//Fence
	m_fence->setWidth(40);
	m_fence->setHeight(90);

	//Green slime - Main player
	m_greenSlime->setRadius(50);
	m_greenSlime->setMaxSpeed(10, 10);
	m_greenSlime->setColour(Color(0.4667f, 0.7882f, 0.451f));
	m_greenSlime->setBackgroundColour(m_background);
	m_greenSlime->setMiddle(m_fence->getLoc().x - m_fence->getWidth() / 2, m_fence->getLoc().y);
	m_greenSlime->setJumpKey('w');
	m_greenSlime->setLeftKey('a');
	m_greenSlime->setRightKey('d');
	m_greenSlime->setOpponent(m_redSlime);
	m_greenSlime->setBall(m_ball);

	//Red slime - Main agent, can be a player too
	m_redSlime->setRadius(50);
	m_redSlime->setMaxSpeed(10, 10);
	m_redSlime->setColour(Color(0.8314f, 0.3804f, 0.3804f));
	m_redSlime->setBackgroundColour(m_background);
	m_redSlime->setMiddle(m_fence->getLoc().x + m_fence->getWidth() / 2, m_fence->getLoc().y);
	m_redSlime->setJumpKey('i');
	m_redSlime->setLeftKey('j');
	m_redSlime->setRightKey('l');
	m_redSlime->setOpponent(m_greenSlime);
	m_redSlime->setBall(m_ball);
	//Start off using the neural network
	m_redSlime->setNeuralNetwork(true);

	//Ball
	m_ball->setRadius(20);
	m_ball->setMaxSpeed(11, 11);

	//Setup the slime locations only once
	m_greenSlime->setLoc(m_greenSlimeStartLoc);
	m_redSlime->setLoc(m_redSlimeStartLoc);

	//Neural network and genetic algorithm stuff
	//Set to true to have two agents fights. Uses genetic algorithms to improve the neural network weights
	m_trainingMode = true;
	m_numSimulations = 0;
	m_numAgentsVersed = 0;
	m_maxAgentsVersed = 2;
	m_generationNum = 0;
	m_bestGenLastFitness = 0;
	m_bestTotalFitness = -50.0f;
	m_bestGen = 0;
	m_prevGreenX = 0;
	m_currGreenX = 0;
	m_prevRedX = 0;
	m_currRedX = 0;
	m_greenOverFence = 0;
	m_redOverFence = 0;
	//Calculate the maximum number of connections between the nn layers
	m_maxGeneLength = 13 * 4 + 4 * 4 + 4 * 3;

	//Setup trainingraining
	if (m_trainingMode)
	{
		m_greenSlime->setNeuralNetwork(true);
		m_redSlime->setNeuralNetwork(true);

		//Start from a specific file? Set to true to start from a specific generation
		bool startFromFile = true;
		int maxPoolSize = 10;
		
		if (startFromFile)
		{
			//Change this number to open the selected file
			m_generationNum = 49;

			ifstream inFile("Genes//" + to_string(m_generationNum) + "GenAllWeights.txt");

			while (!inFile.eof())
			{
				//Create a new dna object
				DNA *newDna = new DNA(m_maxGeneLength);
				//The new gene strand
				vector<float> newGenes;

				//populate the gene vector
				for (int i = 0; i < m_maxGeneLength; i++)
				{
					float num;
					inFile >> num;
					newGenes.push_back(num);
				}

				newDna->setGenes(newGenes);
				m_pool.push_back(newDna);
			}

			//m_pool.pop_back();
			inFile.close();

			//Start from the next generation
			m_generationNum++;
		}
		else
		{
			//Creates a new pool. the number of genes is equal to the number of synapses in the neural network. I hardcoded the value in
			m_pool = createNewGeneticPool(maxPoolSize, m_maxGeneLength);
		}

		//The max simulations have to be half of the pool size. No larger
		m_maxSimulations = maxPoolSize / 2;
		m_trainingPool = m_pool;
	}
	else
	{
		//Load the bot
		m_generationNum = 337;
		ifstream inFile("Genes//" + to_string(m_generationNum) + "GenBestWeights.txt");

		while (!inFile.eof())
		{
			//Create a new dna object
			DNA *newDna = new DNA(m_maxGeneLength);
			//The new gene strand
			vector<float> newGenes;

			//populate the gene vector
			for (int i = 0; i < m_maxGeneLength; i++)
			{
				float num;
				inFile >> num;
				newGenes.push_back(num);
			}

			newDna->setGenes(newGenes);

			m_redSlime->setupWeights(newDna);
		}

		inFile.close();
	}

	setupGame();
}

void SlimeVolleyballApp::setupGame()
{
	//Init starting locations
	m_ball->setLoc(m_ballStartLoc);

	//Choose a random velocity for the ball
	int ballVelX = randInt(-10, 10);
	while (ballVelX == 0)
	{
		ballVelX = randInt(-10, 10);
	}
	m_ball->setVelocity(randInt(-10, 10), randInt(-5, 5));

	m_currPauseTime = m_maxPauseTime;
	m_totalTime = 0;
	m_greenTouchedBall = 0;
	m_redTouchedBall = 0;
	m_ballPrevSide = 0;
	m_ballCurrSide = 0;

	//Only perform 1000 iterations
	/*if (m_generationNum == 1001)
	{
		exit(0);
	}*/

	if (m_trainingMode)
	{
		m_maxPauseTime = 0;

		if (m_numSimulations < m_maxSimulations)
		{
			if (m_numAgentsVersed == m_maxAgentsVersed || m_numSimulations == 0)
			{
				//Pick one random agent. This agent will verse 10 random agents in the training pool
				m_currentAgents = selectDNA(2, &m_trainingPool);

				//Setup the weights of the agents
				m_greenSlime->setupWeights(m_currentAgents[0]);
				m_redSlime->setupWeights(m_currentAgents[1]);

				//Store the current agents positions
				m_prevGreenX = m_greenSlime->getLoc().x;
				m_currGreenX = m_prevGreenX;
				m_prevRedX = m_redSlime->getLoc().x;
				m_currRedX = m_prevRedX;

				m_greenOverFence = 0;
				m_redOverFence = 0;

				m_numAgentsVersed = 0;
				m_numSimulations++;
			}

			//Pick another agent for this agent to verse
			/*int r = randInt(m_pool.size());
			if (m_currentAgents.size() == 2)
			{
				m_currentAgents.pop_back();
			}
			m_currentAgents.push_back(m_pool[r]);
			m_redSlime->setupWeights(m_currentAgents[0]);*/

			//Alternate the ball between left and right
			int ballVelX = randInt(1, 10);
			int leftRight = m_numAgentsVersed % 2 == 0 ? 1 : -1;
			m_ball->setVelocity(ballVelX * leftRight, randInt(-5, 5));

			m_numAgentsVersed++;
		}
		else
		{
			//Create a new generation
			m_numSimulations = 0;

			//Save the best agent's weights in the current generation
			saveBestWeights();
			saveAllWeights();

			//Create a new pool
			m_pool = createExistingGeneticPool(m_pool, m_maxGeneLength, m_bestGenLastFitness);

			if (m_bestGenLastFitness > m_bestTotalFitness)
			{
				m_bestTotalFitness = m_bestGenLastFitness;
				m_bestGen = m_generationNum;
			}

			m_trainingPool = m_pool;

			m_generationNum++;
		}
		
	}
}

void SlimeVolleyballApp::keyUp(KeyEvent event)
{
	//Read in key presses
	m_greenSlime->setKeysReleased(event.getChar());
	//m_redSlime->setKeysReleased(event.getChar());

	if (event.getCode() == event.KEY_F1)
	{
		m_drawObjects = m_drawObjects == 0 ? 1 : 0;
	}
}

void SlimeVolleyballApp::keyDown(KeyEvent event)
{
	//read in key presses
	m_greenSlime->setKeysPressed(event.getChar());
	//m_redSlime->setKeysPressed(event.getChar());
}

bool SlimeVolleyballApp::pauseGame()
{
	//Returns false if the game is not paused and true otherwise
	if (m_currPauseTime == -1)
	{
		return true;
	}
	else if (m_currPauseTime > 0)
	{
		m_currPauseTime -= 1;

		return m_currPauseTime != 0;
	}

	return false;
}

void SlimeVolleyballApp::update()
{
	//Apply gravity to the objects
	m_greenSlime->applyForce(m_gravity);
	m_redSlime->applyForce(m_gravity);

	//Only update the ball if the game has not been paused
	if (!pauseGame())
	{
		m_ball->applyForce(m_gravity);
		m_ball->update();
	}

	/*if (m_trainingMode)
	{
		if (m_ball->getLoc().x < m_fence->getLoc().x)
		{
			if (m_greenSlime->getLoc().x < m_ball->getLoc().x)
			{
				m_greenSlime->setVelocity(m_greenSlime->getMaxSpeed().x, m_greenSlime->getVelocity().y);
			}
			else if (m_greenSlime->getLoc().x > m_ball->getLoc().x)
			{
				m_greenSlime->setVelocity(-m_greenSlime->getMaxSpeed().x, m_greenSlime->getVelocity().y);
			}
		}
	}*/

	//Update the game objects
	m_greenSlime->update();
	m_redSlime->update();

	if (m_ball->getLoc().x > m_fence->getLoc().x + 10)
	{
		//On the right side
		if (m_ballCurrSide == 0)
		{
			m_ballCurrSide = 1;
			m_ballPrevSide = 1;
		}
		else if (m_ballCurrSide == -1 && m_ballCurrSide == -1) //Moved from the left side, has not been on the right side before
		{
			m_ballCurrSide = 1;
			m_greenOverFence++;
		}
		else if (m_ballCurrSide == -1 && m_ballPrevSide == 1) //Moved from the left side, has been on the right side before
		{
			m_ballCurrSide = 1;
			m_ballPrevSide = -1;
			m_greenOverFence++;
		}
	}
	else if (m_ball->getLoc().x < m_fence->getLoc().x - 10)
	{
		//On the left side
		if (m_ballCurrSide == 0) //Just moved into this side
		{
			m_ballCurrSide = -1;
			m_ballPrevSide = -1;
		}
		else if (m_ballCurrSide == 1 && m_ballPrevSide == 1) //Moved from the right side, has not been on the left side before
		{
			m_ballCurrSide = -1;
			m_redOverFence++;
		}
		else if (m_ballCurrSide == 1 && m_ballPrevSide == -1) //Moved from the right side, has been on the left side before
		{
			m_ballCurrSide = -1;
			m_ballPrevSide = 1;
			m_redOverFence++;
		}
	}

	//Collision detection
	if (m_ball->isColliding(m_greenSlime))
	{
		m_ball->bounce(m_greenSlime);
		m_greenTouchedBall++;
	}

	if (m_ball->isColliding(m_redSlime))
	{
		m_ball->bounce(m_redSlime);
		m_redTouchedBall++;
	}

	if (m_ball->isColliding(m_fence))
	{
		m_ball->bounce(m_fence);
	}

	m_currGreenX = m_greenSlime->getLoc().x;
	m_currRedX = m_redSlime->getLoc().x;

	//Check if the game is over or if the max time has passed for training mode
	if (m_ball->gameOver() || (m_trainingMode && m_totalTime >= 60 * 10))
	{
		//Check which side the ball landed on. This will work as the ball will never equal the location of the fence and be touching the floor
		if (m_ball->getLoc().x < m_fence->getLoc().x) //Left
		{
			m_redSlime->addScore(1);
		}
		else if (m_ball->getLoc().x > m_fence->getLoc().x) //Right
		{
			m_greenSlime->addScore(1);
		}

		//Training only
		if (m_trainingMode)
		{
			//Fill out the fitness function
			//How long the bot lasted against the other bot. Not as important
			float timeFitness = (m_totalTime / 60.0f) * 0.15;
			//The winner gains a little bonus for winning. Used to separate from the losing bot
			float winFitnessGreen = (m_greenSlime->getScore() == 0 && m_redSlime->getScore() == 1) ? -1.0f : 1.0f;
			float winFitnessRed = (m_redSlime->getScore() == 0 && m_greenSlime->getScore() == 1) ? -1.0f : 1.0f;
			//Movement fitness is somewhat important too, the agent is penalized greatly if it does not move once
			float moveFitnessGreen = m_currGreenX != m_prevGreenX ? 1.0f : -3.0f;
			float moveFitnessRed = m_currRedX != m_prevRedX ? 1.0f : -3.0f;
			//Used to determine if the agent managed to get the ball to the other side, most important
			float sideFitnessGreen = m_greenOverFence * moveFitnessGreen;
			float sideFitnessRed = m_redOverFence * moveFitnessRed;
			float touchFitnessGreen = m_greenTouchedBall * moveFitnessGreen;
			float touchFitnessRed = m_redTouchedBall * moveFitnessRed;

			float greenFitness = sideFitnessGreen + touchFitnessGreen + winFitnessGreen + timeFitness;
			float redFitness = sideFitnessRed + touchFitnessRed + winFitnessRed + timeFitness;
			
			//If the bot did nothing, do not penalize it as the other bot failed
			/*if (winFitnessGreen > 0 && sideFitnessRed == 0)
			{
				greenFitness = 0;
			}
			else if (winFitnessRed > 0 && sideFitnessGreen == 0)
			{
				redFitness = 0;
			}*/

			//Only store the red fitness as that is the bot that will be versing 10 bots
			//The fitness depends on the time the ball stays in play but also another method, it also depends how many times the agent hit the ball
			m_currentAgents[0]->setFitness(m_currentAgents[0]->getFitness() + greenFitness);
			m_currentAgents[1]->setFitness(m_currentAgents[1]->getFitness() + redFitness);

			m_greenSlime->addScore(-m_greenSlime->getScore());
			m_redSlime->addScore(-m_redSlime->getScore());
		}

		//Start the game over again
		setupGame();
	}

	m_totalTime++;
}

void SlimeVolleyballApp::draw()
{
	//Clear the screen to sky blue
	gl::clear(m_background);

	//Draw the ui
	if (m_trainingMode)
	{
		drawTrainerData(m_numSimulations, m_generationNum, m_bestGenLastFitness, m_bestTotalFitness, m_bestGen);
	}
	else
	{
		drawScore(m_greenSlime, m_redSlime);
	}

	if (m_drawObjects)
	{
		//Draw the objects
		m_greenSlime->draw();
		m_redSlime->draw();
		m_ball->draw();
		m_fence->draw();
	}

	//Display framerate
	Font frameRateFont = Font("Arial", 16.0f);
	gl::drawString("Framerate: " + to_string(getAverageFps()), vec2(app::getWindowWidth() - 130, 5.0f), Color::black(), frameRateFont);
}

void SlimeVolleyballApp::saveBestWeights()
{
	//Look for the best agent. One with the highest fitness
	float maxFitness = -100;
	DNA* bestDNA = m_pool[0];

	for (int i = 0; i < m_pool.size(); i++)
	{
		float fitness = m_pool[i]->getFitness();

		if (fitness > maxFitness)
		{
			maxFitness = fitness;
			bestDNA = m_pool[i];
		}
	}

	//Save weights to a file
	ofstream outFile("Genes\\" + to_string(m_generationNum) + "GenBestWeights.txt");

	for (int i = 0; i < bestDNA->getGenes().size(); i++)
	{
		outFile << bestDNA->getGenes()[i];

		if (i != bestDNA->getGenes().size() - 1)
		{
			outFile << "\n";
		}
	}

	outFile.close();
}

void SlimeVolleyballApp::saveAllWeights()
{
	//Save all weights to a file. This allows you to continue from a particular spot
	ofstream outFile("Genes\\" + to_string(m_generationNum) + "GenAllWeights.txt");

	for (int i = 0; i < m_pool.size(); i++)
	{
		vector<float> genes = m_pool[i]->getGenes();

		for (int x = 0; x < genes.size(); x++)
		{
			outFile << genes[x];

			if (i != m_pool.size() - 1)
			{
				outFile << "\n";
			}
		}
	}

	outFile.close();
}

CINDER_APP(SlimeVolleyballApp, RendererGl, prepareSettings)