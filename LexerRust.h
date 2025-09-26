#ifndef LEXERRUST_H
#define LEXERRUST_H

#include "Token.h"
#include "Logger.h"
#include <string>
#include <vector>
#include <map>
#include <regex> //Libreria para usar expresiones regulares

using namespace std;

struct Reglas
{
    regex patron; // Expresión regular para el patrón
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
    vector <Reglas> Reglas; //Listar las reglas con sus patrones y tipos
};
#endif // LEXERRUST_H