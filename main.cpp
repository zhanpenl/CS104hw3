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

void shift(StackInt& stack, int& num) {
	int opr = stack.top();
	stack.pop();
	if (opr == LEFT_SHIFT) num = num << 1;
	if (opr == RIGHT_SHIFT) num = num >> 1;	
}

void addMul(StackInt& stack, int& num, bool& syntaxError, const int& opr) {
	// operator inconsistency
	if (stack.top() != opr) {
		syntaxError = true;
		return;
	}
	stack.pop();

	int lhs;
	// opr = stack.top();
	
	// ~ + 3 
	if (stack.empty()) {
		syntaxError = true;
		return;
	}
	// + + 3
	lhs = stack.top();
	stack.pop();
	if(lhs < 0) { 
		num = -100; // for test uses
		syntaxError = true;
		return; 
	}

	if (opr == ADDITION) num += lhs;
	if (opr == MUPLIFICATION) num *= lhs;
}

void doShiftings(StackInt& stack, int& num) {
	if (!stack.empty())
        while(stack.top() == LEFT_SHIFT || stack.top() == RIGHT_SHIFT) {
	        shift(stack, num);
	        if (stack.empty()) break;
        }
}

// void computeStack(StackInt& stack, int& num, bool& syntaxError) {
// 	int prevOpr;
//     // keep computing until the number meets open parenthesis
//     // or the number be the bottom
//     if (!stack.empty())
//         while ((prevOpr = stack.top()) != LEFT_PAREN) {
//             if (prevOpr == LEFT_SHIFT || prevOpr == RIGHT_SHIFT) {
//             	shift(stack, num);
//             }
//             if (prevOpr == ADDITION || prevOpr == MUPLIFICATION) {
//             	// syntax error if detect operator befor '+' or '*'
//             	addMul(stack, num, syntaxError, opr);
//             	if (syntaxError) break;
//             }
//             // if the previous is a number, syntax error
//             if (prevOpr > 0) { syntaxError = true; break; }
//             if (stack.empty()) break;
//         }
// }

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
    	if (curr.length() == 0) continue;

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

                // do all shiftings(rev Mar 26) avalible in the stack
                doShiftings(stack, num);
	            cout << endl << "push: " << num << endl;
	            stack.push(num);
            }

            // right-operation (')')
            if (tempC == ')') {
            	int num;
            	int opr; // used to check consistency
            	// ~)
            	if (stack.empty()) { syntaxError = true; break; }
            	num = stack.top(); stack.pop();
            	// <) or ~5)
            	if (num < 0 || stack.empty()) { syntaxError = true; break; }
            	// 5 + 5 + ... + 5)
            	opr = stack.top();
            	while (stack.top() == ADDITION || stack.top() == MUPLIFICATION) {
            		addMul(stack, num, syntaxError, opr);
            		if (syntaxError) break;
            	}            	
            	// ~5 + 5 .. + 5)
            	if (stack.empty()) {syntaxError = true; break;}
            	// 5 5)
            	if (stack.top() != LEFT_PAREN) {
            		syntaxError = true;
            		break;
            	}
            	stack.pop();
            	// after poping the open parenthesis, compute avaliable computations
            	// computeStack(stack, num, syntaxError);
	            // if (syntaxError) break;
            	cout << endl << "push: " << num << endl;
            	stack.push(num);
            }         	        
        }

        // if this line contains syntax error, skip this line
        if (syntaxError) {
        	output << "Malformed" << endl;
        	continue;
        }
        // finish the left computations and check stack
        int result = stack.top();
        stack.pop();
        doShiftings(stack, result);
        if (stack.empty()) output << result << endl;
        else output << "Malformed" << endl;
    }


	return 0;
}
