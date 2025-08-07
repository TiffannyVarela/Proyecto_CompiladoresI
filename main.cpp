#include "lectura.h"
#include "AnalizadorRust.h"
#include "LexerRust.h"
#include "ParserRust.h"
#include <iostream>
#include <vector>
#include <string>
#include <cctype>


using namespace std;

vector<string> tokenize(const vector<char>& content) {
    vector<string> tokens;
    string currentToken;
    const string symbols = ";,(){}[]:+-*/=%<>&|!^.";

    for (size_t i = 0; i < content.size(); i++) {
        char c = content[i];

        if (c == '"') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            string strToken ="\"";
            i++;
            bool escape = false;
            while (i<content.size())
            {
                char cc = content[i];
                strToken +=cc;
                if (escape){
                    escape=false;
                }
                else{
                    if (cc == '\\'){
                        escape = true;
                    }
                    else if (cc == '"'){
                        break;
                    }
                }
                i++;
            }
            tokens.push_back(strToken);
            continue;            
        }

        if (isspace(c) || symbols.find(c) != string::npos) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }

            if (i+1 < content.size())
            {
                string twoCharSymbol = string(1, c) + content[i+1];
                if(twoCharSymbol == "==" || twoCharSymbol == "!=" || twoCharSymbol == "<=" || twoCharSymbol == ">=" || 
                   twoCharSymbol == "&&" || twoCharSymbol == "||" || twoCharSymbol == "+=" || twoCharSymbol == "-=" ||
                   twoCharSymbol == "*=" || twoCharSymbol == "/=" || twoCharSymbol == "%=" || twoCharSymbol == "->" ||
                   twoCharSymbol == "::" || twoCharSymbol == "..")
                {
                    tokens.push_back(twoCharSymbol);
                    i++; // Skip the next character as it's part of this symbol
                    continue;
                }
            }
            
            if (!isspace(c)) {
                tokens.push_back(string(1, c)); 
            }
            
        } 
        else {
            currentToken += c;
        }
    }
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    return tokens;
}

int main() {
    lectura fileReader("main.rs");
    vector<char> fileContent = fileReader.readFile();

    /*cout << "File content : " << endl;
    for (char c : fileContent) {
        //cout << c;
    }*/

    vector<string> tokens = tokenize(fileContent);
    /*for (const auto& token : tokens) {
        cout << "[" << token<< "]" << endl;
    }*/
    
    cout << "\n\n" << endl;

    LexerRust lexer(tokens);
    lexer.verifyTokens();
    
    AnalizadorRust analyzer(tokens);
    //analyzer.contFunciones();
    analyzer.contVariables();
    analyzer.listPalabrasReservadas();
    analyzer.contFuncionesDefinidas();
    //analyzer.contEstructuras();

    ParserRust parser(tokens);
    parser.verifEstructuras();
    parser.verifParentesis();
    parser.verifFunciones();
    parser.verifVariables();

    lexer.~LexerRust();
    analyzer.~AnalizadorRust();
    return 0;
}

