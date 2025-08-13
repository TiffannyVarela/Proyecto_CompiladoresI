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
    vector<Token> tokens;
    size_t pos;

    Token actual() const;
    void avanza();
    
public:
    ParserRust(const vector<Token>& tokens);
    unique_ptr<NodoAST>parse();
    unique_ptr<NodoAST> parseFunc();
    unique_ptr<NodoAST> parseIf();
    unique_ptr<NodoAST> parseAsig();
    unique_ptr<NodoAST> parseExpre();
    unique_ptr<NodoAST> parseTerm();
    unique_ptr<NodoAST> parseReturn();

    bool wait(const string& value);
    bool wait(Tipo tipe);
    ~ParserRust();
};

#endif // PARSERUST_H