#include "ParserRust.h"
#include <iostream>
#include <stdexcept>

using namespace std;

ParserRust::ParserRust(const vector<Token>& tokens) : tokens(tokens), pos(0) {}

ParserRust::~ParserRust() {}

const Token& ParserRust::actual() const {
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    static Token endToken = {Tipo::FIN, "EOF", -1, -1};
    return endToken;
}

bool ParserRust::isEnd() const {
    return pos >= tokens.size() || actual().tipo == Tipo::FIN;
}

void ParserRust::avanza() {
    if (!isEnd()) {
        pos++;
    }
}

bool ParserRust::matchLexical(const string& value) {
    if (isEnd())
    {
        return false;
    }
    if (actual().valor == value) {
        avanza();
        return true;
    }
    return false;
}

bool ParserRust::matchTipo(Tipo tipe) {
    if (isEnd()) 
    {
        return false;
    }
    if (actual().tipo == tipe) {
        avanza();
        return true;
    }
    return false;
}

void ParserRust::expectLexical(const string& value, const char* errorMsg) {
    if (!matchLexical(value)) {
        throw runtime_error("Linea: " + to_string(actual().line) + ", Columna: " + to_string(actual().column) + ". Expected token '" + value + "', but found '" + actual().valor + "'. " + errorMsg);
    }
}

void ParserRust::expectTipo(Tipo tipe, const char* errorMsg) {
    if (!matchTipo(tipe)) {
        throw runtime_error("Linea: " + to_string(actual().line) + ", Columna: " + to_string(actual().column) + ". Expected token type '" + to_string(static_cast<int>(tipe)) + "', but found '" + actual().valor + "'. " + errorMsg);
    }
}

unique_ptr<NodoAST> ParserRust::parse() {
    auto rootNode = parseProgram();
    while(actual().tipo != Tipo::FIN) {
        auto declNode = parseDeclaracion();
        if (declNode) {
            rootNode->addChild(move(declNode));
        } else {
            throw runtime_error("Error parsing declaration at line " + std::to_string(actual().line));
            avanza(); // Skip to next token if parsing fails
        }
    }
    return rootNode;
}

unique_ptr<NodoAST> ParserRust::parseProgram() {
    auto programNode = make_unique<NodoAST>("Program", "");
    while (!isEnd()) {
        if (actual().valor == "fn" || actual().valor == "let" || actual().valor == "if" || actual().valor == "for") {
            auto declNode = parseDeclaracion();
            if (declNode) {
                programNode->addChild(move(declNode));
            } else {
                throw runtime_error("Error parsing declaration at line " + std::to_string(actual().line));
            }
        }         
    }
    return programNode;
}

unique_ptr<NodoAST> ParserRust::parseDeclaracion() {
    if (actual().valor == "fn") 
    {
        return parseFunc();
    } else if (actual().valor == "let") 
    {
        auto asigNode = parseAsig();
        expectLexical(";", "Expected ';' after assignment.");
        return asigNode;
    } else if (actual().valor == "if") 
    {
        return parseIf();
    } else 
    {
        throw runtime_error("Unexpected declaration: " + actual().valor);
    } 
}

unique_ptr<NodoAST> ParserRust::parseFunc() {
    expectLexical("fn", "Expected 'fn' keyword.");
    expectTipo(Tipo::IDENTIFICADOR, "Expected function name.");
    string funcName = tokens[pos-1].valor;

    expectLexical("(", "Expected '(' after function name.");
    auto parameters = parseParamOpt();
    expectLexical(")", "Expected ')' after function parameters.");

    if (matchLexical("->")) {
        if(!(matchTipo(Tipo::IDENTIFICADOR) || matchTipo(Tipo::TIPO_PRIMITIVO) || matchTipo(Tipo::TIPO_ESTANDAR)))
        {
            throw runtime_error("Expected return type after '->' at line " + to_string(actual().line));
        }
    }
    
    auto funcNode = make_unique<NodoAST>("Function", funcName);
    funcNode->addChild(move(parameters));
    funcNode->addChild(move(parseBloque()));

    return funcNode;
}

unique_ptr<NodoAST> ParserRust::parseParamOpt() {
    auto paramsNode = make_unique<NodoAST>("Params", "");
    while (actual().tipo == Tipo::IDENTIFICADOR) {
        string paramName = actual().valor;
        avanza();
        string paramType;
        if (matchLexical(":")) {
            if (!(matchTipo(Tipo::IDENTIFICADOR) || matchTipo(Tipo::TIPO_PRIMITIVO) || matchTipo(Tipo::TIPO_ESTANDAR)))
            {
                throw runtime_error("Expected type after ':' at line " + to_string(actual().line));
            }
            paramType = tokens[pos-1].valor;
        }

        auto paramNode = make_unique<NodoAST>("Param", paramName + (paramType.empty() ? "" : ": " + paramType));
        paramsNode->addChild(move(paramNode));
        
        if (!matchLexical(",")) {
            break;
        }
    }
    return paramsNode;
}

unique_ptr<NodoAST> ParserRust::parseIf() {
    expectLexical("if", "Expected 'if' keyword.");
    auto conditionNode = parseExpre();
    
    auto ifNode = make_unique<NodoAST>("If", "");
    ifNode->addChild(move(conditionNode));
    
    ifNode->addChild(move(parseBloque()));
    
    if (matchLexical("else")) {
        if (matchLexical("if")) {
            ifNode->addChild(move(parseIf()));
        } else {
            ifNode->addChild(move(parseBloque()));
        }
    }
    
    return ifNode;
}

unique_ptr<NodoAST> ParserRust::parseFor() {
    expectLexical("for", "Expected 'for' keyword.");
    expectTipo(Tipo::IDENTIFICADOR, "Expected identifier for loop variable.");
    string loopVar = tokens[pos-1].valor;
    expectLexical("in", "Expected 'in' keyword after loop variable.");
    auto rangeNode = parseExpre() ;
    auto forNode = make_unique<NodoAST>("For", "");
    forNode->addChild(move(rangeNode));    
    forNode->addChild(move(parseBloque()));
    return forNode;
}

unique_ptr<NodoAST> ParserRust::parseBloque() {
    expectLexical("{", "Expected '{' to start block.");
    auto blockNode = make_unique<NodoAST>("Block", "");
    
    while (!isEnd() && actual().valor != "}") {
        blockNode->addChild(move(parseSentence()));
    }
    
    expectLexical("}", "Expected '}' to end block.");
    return blockNode;
}

unique_ptr<NodoAST> ParserRust::parseSentence() {
    if (actual().valor == "return") 
    {
        return parseReturn();
    } 
    else if (actual().valor == "let") 
    {
        return parseAsig();
    } 
    else if (actual().valor == "if") 
    {
        return parseIf();
    } 
    else if (actual().valor == "for") 
    {
        return parseFor();
    } 
    else if (actual().valor == "{") 
    {
        return parseBloque();
    }
    else 
    {
        auto exprNode = parseExpre();
        expectLexical(";", "Expected ';' after expression.");
        return exprNode;
    }
}

unique_ptr<NodoAST> ParserRust::parseReturn() {
    expectLexical("return", "Expected 'return' keyword.");
    auto returnNode = make_unique<NodoAST>("Return", "");
    
    if (!isEnd() && actual().valor != ";") {
        returnNode->addChild(move(parseExpre()));
    }
    
    expectLexical(";", "Expected ';' after return statement.");
    return returnNode;
}

unique_ptr<NodoAST> ParserRust::parseAsig() {
    expectLexical("let", "Expected 'let' keyword for assignment.");
    expectTipo(Tipo::IDENTIFICADOR, "Expected identifier for variable name.");

    string varName = tokens[pos-1].valor;
    string varType;

    //Para soportar let x: i32 = ....;
    if (matchLexical(":")) {
        if (!(matchTipo(Tipo::IDENTIFICADOR) || matchTipo(Tipo::TIPO_PRIMITIVO) || matchTipo(Tipo::TIPO_ESTANDAR)))
        {
            throw runtime_error("Expected type after ':' at line " + to_string(actual().line));
        }
        varType = tokens[pos-1].valor;
    }
    
    auto asigNode = make_unique<NodoAST>("Assignment", varName + (varType.empty() ? "" : ": " + varType));
    
    expectLexical("=", "Expected '=' for assignment.");
    asigNode->addChild(move(parseExpre()));
    
    expectLexical(";", "Expected ';' after assignment.");
    return asigNode;
}

unique_ptr<NodoAST> ParserRust::parseExpre() {
    auto node = parseSum();
    while (matchLexical("==") || matchLexical("!=") || matchLexical("<") || matchLexical("<=") || matchLexical(">") || matchLexical(">=")) {
        auto opNode = make_unique<NodoAST>("Operator", tokens[pos-1].valor);
        opNode->addChild(move(node));
        opNode->addChild(move(parseSum()));
        node = move(opNode);
    }
    return node;
}

unique_ptr<NodoAST> ParserRust::parseSum() {
    auto node = parseMultDiv();
    while (matchLexical("+") || matchLexical("-")) {
        auto opNode = make_unique<NodoAST>("Operator", tokens[pos-1].valor);
        opNode->addChild(move(node));
        opNode->addChild(move(parseMultDiv()));
        node = move(opNode);
    }
    return node;
}

unique_ptr<NodoAST> ParserRust::parseMultDiv() {
    auto node = parseUn();
    while (matchLexical("*") || matchLexical("/") || matchLexical("%")) {
        auto opNode = make_unique<NodoAST>("Operator", tokens[pos-1].valor);
        opNode->addChild(move(node));
        opNode->addChild(move(parseUn()));
        node = move(opNode);
    }
    return node;
}

unique_ptr<NodoAST> ParserRust::parseUn() {
    if (matchLexical("+") || matchLexical("-")) {
        auto opNode = make_unique<NodoAST>("UnaryOperator", tokens[pos-1].valor);
        opNode->addChild(move(parsePrim()));
        return opNode;
    }
    return parsePrim();
}

unique_ptr<NodoAST> ParserRust::parsePrim() {
    if (actual().tipo == Tipo::NUMERO_ENTERO || actual().tipo == Tipo::NUMERO_DECIMAL || actual().tipo == Tipo::NUMERO_CIENTIFICO) 
    {
        auto numNode = make_unique<NodoAST>("Number", actual().valor);
        avanza();
        return numNode;
    } 
    else if (actual().tipo == Tipo::CADENA) 
    {
        auto strNode = make_unique<NodoAST>("String", actual().valor);
        avanza();
        return strNode;
    } 
    else if (actual().tipo == Tipo::CARACTER) 
    {
        auto charNode = make_unique<NodoAST>("Character", actual().valor);
        avanza();
        return charNode;
    } 
    else if (actual().tipo == Tipo::IDENTIFICADOR) 
    {

        //Identificador (variable, funcion, macro, namespace/module) String::from...
        string idValue = actual().valor;
        avanza();

        //Si encuentra ::
        if (matchLexical("::"))
        {
            expectTipo(Tipo::IDENTIFICADOR, "Expected identifier after '::' for namespace or module access.");
            idValue += "::" + tokens[pos-1].valor;
        }

        //Llmada a macro
        if (matchLexical("!"))
        {
            expectLexical("(", "Expected '(' after identifier for function call.");
            auto callNode = make_unique<NodoAST>("MacroCall", idValue + "!");
            callNode->addChild(move(parseExpre()));
            expectLexical(")", "Expected ')' after function call arguments.");
            return callNode;
        }

        //Llmada a funcion
        if (matchLexical("("))
        {
            auto callNode = make_unique<NodoAST>("FunctionCall", idValue);
            if (actual().valor != ")")
            {
                callNode->addChild(move(parseExpre()));
                while (matchLexical(","))
                {
                    callNode->addChild(move(parseExpre()));
                }
            }
            expectLexical(")", "Expected ')' after function call arguments.");
            return callNode;
        }

        //Identificador simple
        return make_unique<NodoAST>("Identifier", idValue);
    } 
    else if (matchLexical("(")) 
    {
        auto exprNode = parseExpre();
        expectLexical(")", "Expected ')' after expression.");
        return exprNode;
    }
    
    throw runtime_error("Unexpected token in expression: " + actual().valor + " at line " + to_string(actual().line) + ", column " + to_string(actual().column));
}

void ParserRust::logAvance(const string* value) const {
    if (value) {
        cout << "Avanzando a token: " << *value << endl;
    } else {
        cout << "Avanzando al final de los tokens." << endl;
    }

    cout << "[Parser] " << value << " → Token: " << actual().valor<< " (linea " << actual().line << ")\n";
}
