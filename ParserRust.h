#ifndef PARSERUST_H
#define PARSERUST_H

#include "Token.h"
#include "NodoAST.h"
#include <string>
#include <vector>
#include <memory>

using namespace std;

class ParserRust {
private:
    const vector<Token> tokens;
    size_t pos; // Posición actual en la lista de tokens

    const Token& actual() const;
    bool isEnd() const;
    void avanza();
    bool matchLexical(const string& value);
    bool matchTipo(Tipo tipe);
    void expectLexical(const string& value, const char* errorMsg);
    void expectTipo(Tipo tipe, const char* errorMsg);

    //Recuperacion de errores
    void synchronize();
    
    //Funciones de análisis sintáctico
    unique_ptr<NodoAST> parseProgram();
    unique_ptr<NodoAST> parseDeclaracion();
    unique_ptr<NodoAST> parseFunc();
    unique_ptr<NodoAST> parseParamOpt();
    unique_ptr<NodoAST> parseIf();
    unique_ptr<NodoAST> parseFor();
    unique_ptr<NodoAST> parseBloque();
    unique_ptr<NodoAST> parseSentence();
    unique_ptr<NodoAST> parseReturn();
    unique_ptr<NodoAST> parseAsig();
    unique_ptr<NodoAST> parseExpre();
    unique_ptr<NodoAST> parseSum();
    unique_ptr<NodoAST> parseMultDiv();
    unique_ptr<NodoAST> parseUn();
    unique_ptr<NodoAST> parsePrim();
    void logAvance(const string* value) const;

public:
    explicit ParserRust(const vector<Token>& tokens);
    unique_ptr<NodoAST>parse();

    ~ParserRust();
};

#endif // PARSERUST_H