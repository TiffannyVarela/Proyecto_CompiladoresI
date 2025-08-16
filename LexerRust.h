#ifndef LEXERRUST_H
#define LEXERRUST_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class LexerRust {
public:
    LexerRust(const string& codigo);
    vector<Token> analiza();
    void comentarioLinea(vector<Token>& tokens);
    void comentarioBloque(vector<Token>& tokens);
    void identificador(vector<Token>& tokens);
    void numero(vector<Token>& tokens);
    void cadena(vector<Token>& tokens);
    void caracter(vector<Token>& tokens);
    void simbolos(vector<Token>& tokens);
    void printTokens(vector<Token>& tokens);
    ~LexerRust();

private:
    string codigo;
    size_t pos;
    int line;
    int column;
    set<string> palabrasReservadas;
    set<string> simbolosCompuestos;
    set<string> tiposPrimitivos;
    set<string> tiposEstandar;
    set<string> controlFlujo;
};
#endif // LEXERRUST_H