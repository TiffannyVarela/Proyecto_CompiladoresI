#ifndef LEXERRUST_H
#define LEXERRUST_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class LexerRust {
public:
    LexerRust(const string& codigo);
    vector<Token> analiza();
    ~LexerRust();

private:
    string codigo;
    size_t pos;
    int line;
    int column;
    set<string> palabrasClave;
};
#endif // LEXERRUST_H