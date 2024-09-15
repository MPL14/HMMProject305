#include <string>
using namespace std;


double prCharGivenCharOfState(char charGenerated, char charOfTheState);
/************************************************************************/
//Calculate and return the probability of charGenerated actually typed
//given charOfTheState as the underlying cognitive state. 
/************************************************************************/

void getPrTableForPossibleInitialStates(double prTable[], int sizeOfTable);
/************************************************************************/
//Calculate for each actual cognitive state in a word of sizeOfTable characters,
//	the probability of that cognitive state being the actual first cognitive state
//	when the user types the word.
//Store these prbabilities in the prTable array.
/************************************************************************/

void getPrTableForPossibleNextStates
(double transitionPrTable[], int sizeOfTable, int currentState);
/************************************************************************/
//Calculate for each actual cognitive state in a word of sizeOfTable-1 characters,
//	the transition probability of that cognitive state being the next cognitive state
//	given currentState as the current state.
//Store these prbabilities in the transitionPrTable[] array.
/************************************************************************/


void setParametersSpellingModel();
/************************************************************************/
//Reset the parameters of the spelling model
/************************************************************************/

void displayParametersSpellingModel();
/************************************************************************/
//Display the parameters of the spelling model
/************************************************************************/

void setParametersKbModel();
/************************************************************************/
//Reset the parameters of the keyboard model
/************************************************************************/


void displayParametersKbModel();
/************************************************************************/
//Display the parameters of the keyboard model
/************************************************************************/
