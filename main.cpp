#include "stackint.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// const int RIGHT_PAREN = -1;
const int LEFT_PAREN = -6;
const int LEFT_SHIFT = -7;
const int RIGHT_SHIFT = -8;
const int ADDITION = -9;
const int MUPLIFICATION = -10;

bool pushLeftOperation(StackInt& stack, const char& c) {
	if (c == '(') {
		stack.push(LEFT_PAREN);
		return true;
	}
	if (c == '<') {
		stack.push(LEFT_SHIFT);
		return true;
	}
	if (c == '>') {
		stack.push(RIGHT_SHIFT);
		return true;
	}
	if (c == '+') {
		stack.push(ADDITION);
		return true;
	}
	if (c == '*') {
		stack.push(MUPLIFICATION);
		return true;
	}
	return false;
}

void shiftAndPush(StackInt& stack, int& num) {
	int opr = stack.top();
	stack.pop();
	if (opr == LEFT_SHIFT) num = num << 1;
	if (opr == RIGHT_SHIFT) num = num >> 1;	
	stack.push(num);
}

bool addMulAndPush(StackInt& stack, int& num) {
	int opr, lhs;
	opr = stack.top();
	stack.pop();
	lhs = stack.top();
	stack.pop();
	if(lhs < 0) return false;

	if (opr == ADDITION) num += lhs;
	if (opr == MUPLIFICATION) num *= lhs;
	stack.push(num);
	return true;
}

int main(int argc, char const *argv[])
{
	if(argc < 3){
	cerr << "Please provide an input and output file." << endl;
	return 1;
	}

	ifstream input(argv[1]);
	ofstream output(argv[2]);

	string curr;
    
    while(getline(input, curr)) {
    	StackInt stack; // initialize a stack for every line
        stringstream ss; // string stream to parse lines
        ss << curr;
        bool syntaxError = false;

        char tempC;
        while (ss.get(tempC)) {
            // whitespace
            if (tempC == ' ') continue;
            
            // left-operation
            if (pushLeftOperation(stack, tempC)) continue;

            // number
            if (tempC >= '0' && tempC <= '9') {
                ss.seekg((size_t)ss.tellg() - 1);
                int num;
                ss >> num;
                cout << num << endl;

                // deal with the number
                if (stack.empty()) { syntaxError = true; break; }

                int prevOpr = stack.top();               
                if (prevOpr == LEFT_PAREN) { stack.push(num); continue; }
                if (prevOpr == LEFT_SHIFT || prevOpr == RIGHT_SHIFT) {
                	shiftAndPush(stack, num);
                	continue;
                }
                if (prevOpr == ADDITION || prevOpr == MUPLIFICATION) {
                	// syntax error if detect operator befor '+' or '*'
                	if(!addMulAndPush(stack, num)) {
                		syntaxError = true; 
                		break;
                	}
                	continue;
                }             
            }

            // right-operation (')')
            if (tempC == ')') {
            	int num, opr;
            	num = stack.top(); stack.pop();
            	opr = stack.top(); stack.pop();
            	if (num < 0 || opr != LEFT_PAREN) {
            		syntaxError = true;
            		break;
            	}
            	stack.push(num);
            }         	        
        }

        // if this line contains syntax error, skip this line
        if (syntaxError) continue;
        // check stack
        int result = stack.top();
        stack.pop();
        if (stack.empty()) output << result << endl;
    }


	return 0;
}
