#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>  // For transform
#include <numeric>    // For accumulate
#include <cstdlib>    // For abs
#include <cmath>

//#include <conio.h>	

#include "models.h"
#include <vector>

using namespace std;





//******************************************************************************
//******************************************************************************
//
//  Parameter setting for the storage capacity
//
//******************************************************************************
//******************************************************************************

//The maximum capacity (maximum number of characters allowed) 
//		for the storing the vocabulary set
#define VocabularyStorageLimit 20000

//The maximum capacity (maximum number of characters allowed) 
//		for storing the corrupted words during spelling recognition
#define NoisyWordsStorageLimit 15000





//******************************************************************************
//******************************************************************************
//
//  Parameter settings (in global variables) for the Spelling model
//
//******************************************************************************
//******************************************************************************
double prSpRepeat = 0.2;
//The probability of repeating the current cognitive state again as the next state
//we make it 0.2 initially, but you can try different values to see the effects.

double prSpMoveOn = 0.8;
//The probability of moving from the current cognitive state to other states
//	as the next state
//we make it 0.8 initially, but you can try different values to see the effects.

//********************************************************
//Note that prSpRepeat + prSpMoveon should always equal 1
//********************************************************

double spDegenerateTransitionDistancePower = 2;
//The likelihood of moving from the cognitive state of typing some character in a word 
//to the next cognitive state is proportion to the inverse of 
//(spDegenerateTransitionDistancePower) to the 
//(the distance between the current state to the next state)th power.
//In the setting of the original spelling model in the project,
//we make it 2, but you can try different values to see the effects.

double spDegenerateInitialStateDistancePower = 2;
//The likelihood of some character in a word as the initial cognitive state
//is proportion to the inverse of 
//(spDegenerateInitialStateDistancePower) to the 
//(the position of the character in the word)th power.
//In the setting of the original spelling model in the project,
// spDegenerateInitialStateDistancePower and spDegenerateTransitionDistancePower
//have the same value, but you can make them different to see the effects
//By default, we make it 2, but you can try different values to see the effects.


void setParametersSpellingModel()
{
	cout << endl
		 << "Reset the parameters of the spelling model:" << endl << endl;

	cout << "Reset P_moveOn, the probability of moving on" << endl
		 << "   from the current cognitive state to other states:" << endl
		 << "P_moveOn = ";
	cin  >> prSpMoveOn;

	prSpRepeat = 1- prSpMoveOn;
	cout << endl 
		 << "Automatically reset P_repeat to 1-P_moveOn" << endl
		 << "P_repeat = " << prSpRepeat << endl;

	cout << endl
		<< "Do you want to change the deg_kb? (y or n): ";

	char option;
	cin >> option;

	if (option == 'y') 
	{
		cout << "Reset deg_sp, the power coefficient governing the probability of " << endl
			<< "   skipping from the current cognitive state to other states:" << endl
			<< "deg_sp = ";

		cin  >> spDegenerateTransitionDistancePower; 

		spDegenerateInitialStateDistancePower = spDegenerateTransitionDistancePower; 
	}
}

void displayParametersSpellingModel()
{
	cout << endl
		 << "Parameter values of the spelling model:" << endl
		 << "   P_repeat  = " << prSpRepeat << endl
		 << "   P_moveOn  = " << prSpMoveOn << endl
		 << "   deg_sp = " << spDegenerateTransitionDistancePower << endl << endl;
}

//******************************************************************************
//******************************************************************************
//
//  Parameter settings (in global variables) for the keyboard model
//
//******************************************************************************
//******************************************************************************

double prKbHit = 0.8;
//The probability that you want to type a character and you do successfully make it
//In the setting of the original keyboard model in the project,
//we make it 0.9, but you can try different values to see the effects.

double prKbMiss = 0.2;
//The sum of probabilities that you want to type a character but end in touching 
//a different character.
//we make it 0.1, but you can try different values to see the effects.

//*******************************************************
//Note that prKbHit + prKbMiss should always equal 1
//*******************************************************

//In the setting of the original keyboard model in the project,
//we make it 0.2, but you can try different values to see the effects.


double kbDegenerateDistancePower = 3;
//The likelihood you want to type a character but end in touching a different character
//is proportion to the inverse of 
//(kbDegenerateDistancePower) raised to the (distance between them) th power
//In the setting of the original keyboard model in the project,
//we make it 2, but you can try different constants to see the effects.


void setParametersKbModel()
{
	cout << endl
		 << "Reset the parameters of the keyboard model:" << endl << endl;

	cout << "Reset P_hit, the probability of hitting" << endl
		 << "   the right character wanted on the keyboard:" << endl
		 << "P_hit = ";
	cin  >> prKbHit;

	prKbMiss = 1- prKbHit;
	cout << endl 
		 << "Automatically reset P_miss to 1-P_hit" << endl
		 << "P_miss = " << prKbMiss << endl;

	cout << endl 
		<< "Do you want to change the deg_kb? (y or n): ";

	char option;
	cin >> option;

	if (option == 'y') 
	{
		cout << "Reset deg_kb, the power coefficient governing the probability of " << endl
			<< "   skipping from the current cognitive state to other states:" << endl
			<< "deg_kb = ";

		cin  >> kbDegenerateDistancePower; 
	}
}

void displayParametersKbModel()
{
	cout << endl
		 << "Parameter values of the keyboard model:" << endl
		 << "   P_hit  = " << prKbHit << endl
		 << "   P_miss = " << prKbMiss << endl
		 << "   deg_kb = " << kbDegenerateDistancePower << endl << endl;
}


//******************************************************************************
//******************************************************************************
//
//  Trace flag and the alphabet table
//
//******************************************************************************
//******************************************************************************
bool traceON = true;   // output tracing messages



/************************************************************************/
//Calculate and return the probability of charGenerated actually typed
//given charOfTheState as the underlying cognitive state.
/************************************************************************/

double prCharGivenCharOfState(char charGenerated, char charOfTheState)
{   // KEYBOARD STATE
    // CharGenerated = What we actually touched (typed)
    // CharOfTheState = What we want to type in our mind (our cognitive state)

	//**************************************************
	//Replace the following with your own implementation
	//**************************************************

	if (charGenerated == charOfTheState) {
		return prKbHit;
	}

	// Generate missdist array using the range from 1 to 25
	vector<int> diffASCII(25);
	for (int i = 0; i < 25; ++i) {
		diffASCII[i] = i + 1;
	}

	vector<double> missdist(25);
	transform(diffASCII.begin(), diffASCII.end(), missdist.begin(), [](int n) {
		return min(n, 26 - n);
	});

	vector<double> exponentialDegrade(25);
	transform(missdist.begin(), missdist.end(), exponentialDegrade.begin(), [](double n) {
		return pow(1.0 / kbDegenerateDistancePower, n);
	});

	// Calculate constant_x
	double sum_exponentialDegrade = accumulate(exponentialDegrade.begin(), exponentialDegrade.end(), 0.0);
	double constant_x = prKbMiss / sum_exponentialDegrade;

	// Calculate distASCII_x_y and distKB_x_y
	int distASCII_x_y = abs(static_cast<int>(charGenerated) - static_cast<int>(charOfTheState));
	int distKB_x_y = min(distASCII_x_y, 26 - distASCII_x_y);

	return constant_x * pow(1.0 / kbDegenerateDistancePower, distKB_x_y);
	return 0;
}



/************************************************************************/
//The following function implement part of the spelling model for
// spelling a word with sizeOfTable characters,
//This function
//(i) calculates for each cognitive state excluding the special states I and F,
//     the probability of that cognitive state being the first cognitive state
//     after the transition out of the special state I, and
//(ii) stores these probabilities in a prTable array of sizeOfTable elements.
//Note that the value of the parameter sizeOfTable should be
//     exactly the number characters in the word.
/************************************************************************/

//getPrTableForPossibleInitialStates and  

void getPrTableForPossibleInitialStates(double prTable[], int sizeOfTable) {
		vector<int> missDistance(sizeOfTable);
		vector<double> exponentialDegrade(sizeOfTable);

		// Fill missDistance with values 1 to sizeOfTable
		// [1,2,3,...,sizeOfTable]
		for (int i = 0; i < sizeOfTable; ++i) {
				missDistance[i] = i + 1;
		}

		// Compute exponentialDegrade
		// 
		for (int i = 0; i < sizeOfTable; ++i) {
				exponentialDegrade[i] = pow(1.0 / kbDegenerateDistancePower, missDistance[i]);
		}

		// Compute scaling constant
		double sum = 0.0;
		for (int i = 0; i < sizeOfTable; ++i) {
				sum += exponentialDegrade[i];
		}
		double scalingConstant = 1.0 / sum;

		// Fill prTable array with scaled values
		for (int i = 0; i < sizeOfTable; ++i) {
				prTable[i] = scalingConstant * exponentialDegrade[i];
		}
}


/************************************************************************/
//The following function implement part of the spelling model for
// spelling a word with sizeOfTable-1 characters,
// i.e. sizeOfTable should be 1 + the number characters in the word.
//This function
//(i) calculates for each actual cognitive state for a word
//     (excluding the special I state but including the special F state),
//     the probability of that cognitive state being the next cognitive state
//     given currentState as the index of the current state in the word, and
//(ii) stores these probabilities in a transitionPrTable array of sizeOfTable elements.
//Note that the value of the parameter sizeOfTable should be
//     1 + the number characters in the word.
/************************************************************************/

void getPrTableForPossibleNextStates(double transitionPrTable[], int sizeOfTable, int currentState) {
		vector<int> distances(sizeOfTable);
		vector<double> exponentialDegrade(sizeOfTable);

		// Compute distances (states as indices are from 0 to sizeOfTable - 1)
		for (int i = 0; i < sizeOfTable; ++i) {
				distances[i] = i - currentState;
		}

		// Compute exponentialDegrade
		for (int i = 0; i < sizeOfTable; ++i) {
				if (distances[i] > 0) {
						exponentialDegrade[i] = pow(1.0 / kbDegenerateDistancePower, distances[i]);
				} else {
						exponentialDegrade[i] = 0.0;
				}
		}

		// Compute scaling constant
		double sum = 0.0;
		for (int i = 0; i < sizeOfTable; ++i) {
				sum += exponentialDegrade[i];
		}
		double scalingConstant = prSpMoveOn / sum;

		// Fill transitionPrTable array with scaled values
		for (int i = 0; i < sizeOfTable; ++i) {
				transitionPrTable[i] = scalingConstant * exponentialDegrade[i];
		}

		// Set the probability of repeating the current state
		transitionPrTable[currentState] = prSpRepeat;
}






