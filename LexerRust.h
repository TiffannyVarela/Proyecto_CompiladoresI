#ifndef LEXERRUST_H
#define LEXERRUST_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

struct Reglas
{
    regex patron;
    Tipo tipo;
};


class LexerRust {
public:
    LexerRust(const string& codigo);
    vector<Token> analiza();
    void printTokens(vector<Token>& tokens);
    ~LexerRust();

private:
    string codigo;
    size_t pos;
    int line;
    int column;
    vector <Reglas> Reglas;
};
#endif // LEXERRUST_H