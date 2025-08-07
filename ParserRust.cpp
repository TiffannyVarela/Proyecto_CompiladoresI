#include "ParserRust.h"
#include <iostream>
#include <stack>
#include <regex>

using namespace std;

ParserRust::ParserRust(const vector<string>& tokens){
    this->tokens = tokens;
}

void ParserRust::verifEstructuras() const {
    int structs = 0;
    for (const auto& token : tokens) {
        if (token == "struct") {
            structs++;
        }
    }
    cout << "Total structures (struct): " << structs << endl;
}

void ParserRust::verifParentesis() const {
    stack<char> parentesisStack;
    for (const auto& token : tokens) {
        for (char c : token) {
            if (c == '(' || c == '{' || c == '[') {
                parentesisStack.push(c);
            } else if (c == ')' || c == '}' || c == ']') {
                if (parentesisStack.empty()) {
                    cout << "Unmatched closing parenthesis: " << c << endl;
                    //return;
                    continue; // Skip unmatched closing parenthesis
                }
                char top = parentesisStack.top();
                if ((c == ')' && top != '(') ||
                    (c == '}' && top != '{') ||
                    (c == ']' && top != '[')) {
                    cout << "Mismatched parentheses: " << top << " and " << c << endl;
                    return;
                }
                //parentesisStack.pop();
                else {
                    parentesisStack.pop();
                }
            }
        }
    }
    if (!parentesisStack.empty()) {
        cout << "Unmatched opening parenthesis found." << endl;
    } else {
        cout << "All parentheses are matched." << endl;
    }
}

void ParserRust::verifFunciones() const {
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "fn") {
            if (i + 1 < tokens.size() && regex_match(tokens[i + 1], regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
                cout << "Function found: " << tokens[i + 1] << endl;
            } else {
                cout << "Invalid function declaration at position " << i << endl;
            }
        }
    }
}

void ParserRust::verifVariables() const {
    for (size_t i=0; i< tokens.size(); i++){
        if (tokens[i] == "let" && i> 0 && (tokens[i-1] == "if" || tokens[i-1] =="while"))
        {
            continue;
        }
        
        if (tokens[i] == "let")
        {
            size_t nextIndex = i + 1;
            bool isMutable = false;

            if (nextIndex < tokens.size() && tokens[nextIndex] == "mut") {
                isMutable = true;
                nextIndex++;
            }

            if (nextIndex >= tokens.size() || !regex_match(tokens[nextIndex], regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
                cout << "Error: declaración con 'let" << (isMutable ? " mut" : "") << "' sin nombre válido de variable." << endl;
                continue;
            }

            size_t afterVarIndex = nextIndex + 1;

            if (afterVarIndex < tokens.size() && tokens[afterVarIndex] == ":")
            {
                afterVarIndex++;
                if (afterVarIndex >= tokens.size() || !regex_match(tokens[afterVarIndex], regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
                    cout << "Error: tipo inválido en la declaración de variable '" << tokens[nextIndex] << "'." << endl;
                    continue;
                }
                afterVarIndex++;
            }          

            if (afterVarIndex >= tokens.size() || tokens[afterVarIndex] != "=") {
                cout << "Error: falta '=' en la declaración de variable '" << tokens[nextIndex] << "'." << endl;
                continue;
            }
            
            cout << "Variable declarada correctamente: " << (isMutable ? "(mutable) " : "") << tokens[nextIndex] << endl;
        }
    }
}

ParserRust::~ParserRust() {
    tokens.clear();
    vector<string>().swap(tokens); // Clear and deallocate memory
}
