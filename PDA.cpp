#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <iterator>
#include <cstdio>

#define INPUT_ALPHABET_SIZE 4
#define STACK_ALPHABET_SIZE 6
#define N_STATES 2
#define ERROR_RETURN_CODE -1
#define STACK_EMPTY_MARKER 'Z'
#define COMPARE_WITH_INPUT_ALPHABET true
#define COMPARE_WITH_STACK_ALPHABET false
#define POP_STACK_CODE 'P'
#define KEEP_STACK_UNCHANGED_CODE 'N'

typedef int state;
char inputAlphabet[INPUT_ALPHABET_SIZE] = {'{', '[', '}', ']'};
char stackAlphabet[STACK_ALPHABET_SIZE] = {'{', '[', '}', ']', 'P', 'Z'};

class TransitionRuleInput{
public:
	state startStateCode;
	char inputChar;
	char stackChar;
	TransitionRuleInput(){;}
	TransitionRuleInput(const TransitionRuleInput& transitionRuleInput);
};

TransitionRuleInput::TransitionRuleInput(const TransitionRuleInput& transitionRuleInput){
	startStateCode = transitionRuleInput.startStateCode;
	inputChar = transitionRuleInput.inputChar;
	stackChar = transitionRuleInput.stackChar;
}


class TransitionRuleOutput{
public:
	state nextStateCode;
	char nextStackInstruction;
	TransitionRuleOutput(){;}
	TransitionRuleOutput(const TransitionRuleOutput& transitionRuleOutput);
};

TransitionRuleOutput::TransitionRuleOutput(const TransitionRuleOutput& transitionRuleOutput){
	nextStateCode = transitionRuleOutput.nextStateCode;
	nextStackInstruction = transitionRuleOutput.nextStackInstruction;	     
}

struct ruleOrder{
	bool operator()(TransitionRuleInput input1, TransitionRuleInput input2) const{
	
		bool leftLTright = false;
		state code1 = input1.startStateCode;
		state code2 = input2.startStateCode;
		char char1 = input1.inputChar;
		char char2 = input2.inputChar;
	         	    
		if (code1 < code2)
			leftLTright = true;
		else if (code1 == code2){
			if (char1 < char2)
				leftLTright = true;
			else if (char1 == char2)  	    	
				leftLTright = (input1.stackChar <  input2.stackChar);
		}
			
		return (leftLTright);
	}
};

typedef std::map<TransitionRuleInput, TransitionRuleOutput, ruleOrder>  ruleMap;
typedef std::set<state> stateSet;


class TransitionTable{
	ruleMap  transitionRules;
public:
//Holds transition table in form 
//<curr. state (#)> <input char> <stack top char> <next state (#)> <stack instruction>
//where 
//<stack instruction> ::= <stack char to push> | POP_STACK_CODE | KEEP_STACK_UNCHANGED_CODE
	TransitionTable(){;}
	TransitionTable(const TransitionTable& transitionTable);
	void TransitionTableInit(char* inFileName);
	TransitionRuleOutput getNextStateNStack(TransitionRuleInput transitionRuleInput);
	bool isInAlphabet(char c, bool isInputAlphabet);
};

TransitionTable::TransitionTable(const TransitionTable& transitionTable){

	transitionRules = transitionTable.transitionRules;
}


void TransitionTable::TransitionTableInit(char* inFileName){

	TransitionRuleInput thisRuleInput;
	TransitionRuleOutput thisRuleOutput;
	char inchar;
	state startStateCode, nextStateCode;
	char stateCodeString[2] = {'\0','\0'};
	bool moreTransitionRules = true;

    std::ifstream ifile(inFileName);

	if (ifile.fail()){
		std::cout << "Error:  Cannot read from transition table file " << inFileName << "\n";
		std::exit(ERROR_RETURN_CODE);
	}	    	
	else{
		while (moreTransitionRules){
			ifile >> startStateCode;
			if (ifile.eof())
				moreTransitionRules = false;
			else {
				thisRuleInput.startStateCode = startStateCode;
				ifile >> inchar;
				if (isInAlphabet(inchar, COMPARE_WITH_INPUT_ALPHABET))
					thisRuleInput.inputChar = inchar;
				ifile >> inchar;
				if (isInAlphabet(inchar, COMPARE_WITH_STACK_ALPHABET))
					thisRuleInput.stackChar = inchar;
		      	   	ifile >> inchar;
				if (std::isdigit(inchar)){
				//rule is to move to another state, so stack unchanged
					stateCodeString[0] = inchar;
					thisRuleOutput.nextStateCode = (state) std::atoi(stateCodeString);
					thisRuleOutput.nextStackInstruction = KEEP_STACK_UNCHANGED_CODE;
				}
				else {
				//rule is to push/pop stack; leave state unchanged
					thisRuleOutput.nextStateCode = startStateCode;
					//inchar is in stack alphabet or the POP code
					thisRuleOutput.nextStackInstruction = inchar;  
	   	   	   	}
				if ((thisRuleInput.startStateCode == 0) && (thisRuleInput.stackChar == 'Z'))
					thisRuleOutput.nextStackInstruction = thisRuleInput.inputChar;
				transitionRules[thisRuleInput] = thisRuleOutput;
			}
		}
	}
	
for (ruleMap::iterator tstIterator = transitionRules.begin(); tstIterator != transitionRules.end(); ++tstIterator){
std::cout << "Rule Input: " << (*tstIterator).first.startStateCode << " " << (*tstIterator).first.inputChar << " " << (*tstIterator).first.stackChar << "\n";
std::cout << "Rule Output: " << (*tstIterator).second.nextStateCode << " " << (*tstIterator).second.nextStackInstruction << "\n";
}

}
	
bool TransitionTable::isInAlphabet(char c, bool isInputAlphabet){

	std::string alphabetType;
	char testChar;
	bool inAlphabet = false;
	int alphabetSize = INPUT_ALPHABET_SIZE;
	
	if (!isInputAlphabet)
		alphabetSize = STACK_ALPHABET_SIZE;	 

	for (int i = 0; i < alphabetSize; i++){
		if (isInputAlphabet)
			testChar = inputAlphabet[i];
		else
			testChar = stackAlphabet[i];				
	   	if (c == testChar){
			inAlphabet = true;
			break;
		}
	}
	
	if (!isInputAlphabet && ((c == POP_STACK_CODE) || (c == KEEP_STACK_UNCHANGED_CODE)))
		inAlphabet = true;
	
	if (!inAlphabet){
		if (isInputAlphabet)
			std::cout << "Error:  invalid input alphabet character: " << c << "\n";
		else
			std::cout << "Error:  invalid stack alphabet character: " << c << "\n";	      	  
		std::exit(ERROR_RETURN_CODE);
	}
	else return (inAlphabet);
}

TransitionRuleOutput TransitionTable::getNextStateNStack(TransitionRuleInput thisRuleInput){

	if (!isInAlphabet(thisRuleInput.inputChar, COMPARE_WITH_INPUT_ALPHABET)){
		std::cout << "\nError:  " << thisRuleInput.inputChar << " not in alphabet!\n\n";
		exit(0);
	}
	else {
		ruleMap::iterator nextStateIterator = transitionRules.find(thisRuleInput);
		if (nextStateIterator == transitionRules.end()){
			std::cout << "\nCrash:  Rule Input (startState, inputChar, stackChar) = (S" 
					  << thisRuleInput.startStateCode << ", " << thisRuleInput.inputChar << ", " << thisRuleInput.stackChar 
					  << ") not found in rule table!\n";
			exit(0);
		}
		else
			return ((*nextStateIterator).second);
	}
}

class PDA{
	state states[N_STATES]; 	//accepted states in "acceptedStates" set below
	TransitionTable transitionTable;
	stateSet acceptedStates;
public:
	PDA(char* tableFile, char* acceptedStatesFile);
	bool checkForAcceptance(state checkedState, char stackChar);
	TransitionRuleOutput getNextStateNStack(TransitionRuleInput transitionRuleInput)
	 	 {return (transitionTable.getNextStateNStack(transitionRuleInput));}
};

PDA::PDA(char* tableFile, char* acceptedStatesFile){

	int i;
	state thisCode;

	transitionTable.TransitionTableInit(tableFile);
	for (i = 0; i < N_STATES; i++)
		states[i] = i;

	std::ifstream acceptedStatesInput(acceptedStatesFile);
	acceptedStatesInput >> thisCode;
	while (!acceptedStatesInput.eof()){
		acceptedStates.insert(thisCode);
		acceptedStatesInput >> thisCode;
	}
}

bool PDA::checkForAcceptance(state checkedState, char stackChar){

	if ((checkedState == 1) && (stackChar == STACK_EMPTY_MARKER))
		return (true);
	else
		return (false);
}

int main(int argc, char* argv[]){

	char *transitionTableFile;
	char *acceptedStatesFile;
	int initStateCode;
	bool isAccepted = false;
	char currInput;
	state currStateCode;
	TransitionRuleInput thisRuleInput;
	TransitionRuleOutput thisRuleOutput;
	std::stack<char> stk;
	char stackInstruction;

    if (argv[1] == NULL){
        std::cout << "usage:  stack transitionTableFile acceptedStatesFile startState \n";
        std::exit(0);
    }

    transitionTableFile = argv[1];
    acceptedStatesFile = argv[2];
	std::sscanf(argv[3], "%d", &initStateCode);
	currStateCode = initStateCode;
	stk.push(STACK_EMPTY_MARKER);
	PDA thisPDA(transitionTableFile, acceptedStatesFile);

	while (!std::cin.eof() && !isAccepted && !stk.empty()){
		if (thisPDA.checkForAcceptance(currStateCode, stk.top())) {
			isAccepted = true;
			std::cout << "\n\n In state S" << currStateCode << "; ACCEPTED!\n";
		}
		else {
			std::cout << "\nEnter next input ...\n";
	   	   	std::cin >> currInput;
			thisRuleInput.inputChar = currInput;
			thisRuleInput.startStateCode = currStateCode;
			thisRuleInput.stackChar = stk.top();
			//getNextStateNStack issues crash message and exits if rule does not exist in transition table
	   	   	thisRuleOutput = thisPDA.getNextStateNStack(thisRuleInput);
			currStateCode = thisRuleOutput.nextStateCode;
			stackInstruction = thisRuleOutput.nextStackInstruction;
			if (stackInstruction != KEEP_STACK_UNCHANGED_CODE)
				if (stackInstruction == POP_STACK_CODE)
					stk.pop();
				else
					stk.push(stackInstruction);
	   	   	std::cout << "*********Now in State S"<< currStateCode << " top of stack: " << stk.top() << "\n\n";
		}
	}
/*
	if ((stk.top() == STACK_EMPTY_MARKER) && (std::cin.eof()))
		std::cout << "\n\n In state S" << currStateCode << " " << "Stack top: " << stk.top() << " ACCEPTED!\n";
	else if (stk.empty())
		std::cout << "There were one too many stack pops!\n\n";
	else
		std::cout << "Input sequence not accepted :( \n";	 
*/
}
