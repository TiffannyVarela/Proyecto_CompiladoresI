#include "ParserRust.h"
#include "NodoAST.h"
#include <iostream>
#include <vector>
#include <memory>
//#include <stack>
//#include <regex>

using namespace std;

ParserRust::ParserRust(const vector<Token>& tokens) : tokens(tokens), pos(0){}

Token ParserRust::actual() const{
    if (tokens.empty())
    {
        return Token{Tipo::FIN, "EOF", -1, -1};
    }

    if (pos >= tokens.size())
    {
        cerr << "Advertencia: Posición " << pos << " fuera de rango" << endl;
        return tokens.back(); //Toma ultimo token
    }
    
    return tokens[pos];
}

void ParserRust::avanza(){
    if (pos < tokens.size())
    {
        cout << "Avanzando de " << pos << " (" << tokens[pos].valor << ") a " << (pos+1)<< endl;
        pos++;
    }
    else{
        cout << "No se puede avanzar, fin de tokens alcanzado" << endl;
    }
}

unique_ptr<NodoAST> ParserRust::parse(){
    cout << "Iniciando parsing... Tokens restantes: " << (tokens.size() - pos) << endl;
    if (tokens.empty() || (tokens.size() == 1 &&tokens[0].tipo == Tipo::FIN))
    {
        cerr << "Error: No hay código para parsear" << endl;
        return nullptr;
    }

    try
    {
        //nodo raiz
        auto program = make_unique<NodoAST>("PROGRAMA", "");
        size_t tokensProcesados = 0;
        const size_t maxTokens = tokens.size()*2; //Prevenir bucles infinitos

        while (actual().tipo != Tipo::FIN && tokensProcesados < maxTokens)
        {
            unique_ptr<NodoAST> declaracion;
            size_t posInicia = pos;

            if (actual().valor=="fn")
            {
                declaracion = parseFunc();
            }

            else if (actual().valor == "let")
            {
                declaracion = parseAsig();
            }

            else if (actual().valor == "if")
            {
                declaracion = parseIf();
            }

            else{
                cerr << "Advertencia: Declaración no reconocida - "<< actual().valor << endl; 
                avanza();
            }

            if (declaracion)
            {
                program->addChild(declaracion.release());
            }

            else if (pos == posInicia)
            {
                //No pudo avanzar y trata de prevenir bucle infinito
                cerr << "Error: No se progresó en el parsing. Token actual: "<< actual().valor << endl;
                avanza();
            }
            tokensProcesados++;
        }

        if (program && !program->haveChilds())
        {
            cerr << "Advertencia: No se encontraron declaraciones válidas" << endl;
            return nullptr;
        }
        
        return program;
    }
    catch(const std::exception& e)
    {
        cerr << "Error de parsing: " << e.what() << endl;
        return nullptr;
    }    
}

unique_ptr<NodoAST> ParserRust::parseFunc(){
    cout << "Parseando " << __func__ << " en token: " << actual().valor << endl;
    if (!wait("fn"))
    {
        return nullptr;
    }

    if (actual().tipo != Tipo::IDENTIFICADOR)
    {
        throw runtime_error("Se espera nombre de la funcion.");
    }

    string name = actual().valor;
    avanza();
    
    if (!wait("("))
    {
        throw runtime_error("Se espera '(' luego del nombre");
    }

    //Parsear los parametros
    auto parameters = make_unique<NodoAST>("PARAMETROS", "");
    while (!(actual().tipo == Tipo::PUNTUACION && actual().valor == ")"))
    {
        if (actual().tipo == Tipo::IDENTIFICADOR)
        {
            parameters->addChild(new NodoAST("PARAMETRO", actual().valor));
            avanza();
            if (actual().valor == ",")
            {
                avanza();
            }           
        }
        else{
            throw runtime_error("Se espera ')'");
        }         
    }
    avanza();

    //Tipo de return
    if (actual().valor == "->")
    {
        avanza();
        if (actual().tipo != Tipo::IDENTIFICADOR)
        {
            throw runtime_error("Se espera el tipo de retorno");
        }
        avanza();        
    }

    //Bloque de la funcion
    if(!wait("{")){
        throw runtime_error("Se esperaba '{");
    }

    auto body = make_unique<NodoAST>("CUERPO", "");
    while (true) 
    {
        if (actual().tipo == Tipo::PUNTUACION && actual().valor == "}") 
        {
            break;
        }
        else if (actual().tipo == Tipo::FIN) 
        {
            throw runtime_error("Se llegó al FIN sin encontrar '}'");
        }
        else if (actual().valor == "let") 
        {
            auto asig = parseAsig();
            if (asig) body->addChild(asig.release());
        }
        else if (actual().valor == "return") 
        {
            auto ret = parseReturn();
            if (ret) body->addChild(ret.release());
        }
        else if (actual().valor == "if") 
        {
            auto ifNode = parseIf();
            if (ifNode) body->addChild(ifNode.release());
        }
        else {
            // Token no reconocido - avanzar con precaución
            cerr << "Advertencia: Token no manejado en cuerpo de función: " << actual().valor << endl;
            avanza();
        }
    }
    
    avanza();

    auto function = make_unique<NodoAST>("FUNCION", name);
    function->addChild(parameters.release());
    function->addChild(body.release());

    return function;    
}

unique_ptr <NodoAST> ParserRust::parseIf(){
    cout << "Parseando " << __func__ << " en token: " << actual().valor << endl;
    if (!wait("if"))
    {
        return nullptr;
    }

    //Parsear la condicion
    auto condicion = parseExpre();
    if (!condicion)
    {
        throw runtime_error("Condicion invalida dentro del if");
    }

    //Bloque then**
    if (!wait("{"))
    {
        throw runtime_error("Se espera '{' despues de la condicion");
    }

    auto bloqueThen = make_unique<NodoAST>("BLOQUE", "");
    while (true)
    {
        if (actual().tipo == Tipo::PUNTUACION && actual().valor == "}")
        {
            break;
        }
        
        if (actual().valor == "let")
        {
            auto asig = parseAsig();
            if (asig)
            {
                bloqueThen->addChild(asig.release());
            }
        }
        else if (actual().valor == "return")
        {
            auto ret = parseReturn();
            if (ret)
            {
                bloqueThen->addChild(ret.release());
            }
        }
        else{
            avanza();
        }
    }
        
    wait("}");

    //Parsear el bloque else
    unique_ptr<NodoAST> bloqueElse;
    if (actual().valor == "else")
    {
        avanza();

        if (actual().valor == "{")
        {
            avanza();
            bloqueElse = make_unique<NodoAST>("BLOQUE", "");
            while (actual().valor != "}")
            {
                if (actual().valor == "let")
                {
                    auto asig = parseAsig();
                    if (asig)
                    {
                        bloqueElse->addChild(asig.release());
                    }                    
                }
                else if (actual().valor == "return")
                {
                    auto ret = parseReturn();
                    if (ret)
                    {
                        bloqueElse->addChild(ret.release());
                    }                    
                }
                else{
                    avanza();
                }                
            }
            wait("}");
        }
        else if (actual().valor == "if")
        {
            //si existe else if
            bloqueElse = parseIf();
        }
    }
    
    auto nodoif = make_unique<NodoAST>("IF", "");
    nodoif->addChild(condicion.release());
    nodoif->addChild(bloqueThen.release());
    if (bloqueElse)
    {
        nodoif->addChild(bloqueElse.release());
    }
    return nodoif;
}

unique_ptr <NodoAST> ParserRust::parseAsig(){
    cout << "Parseando " << __func__ << " en token: " << actual().valor << endl;
    if (!wait("let"))
    {
        return parseExpre();
    }

    if (actual().tipo != Tipo::IDENTIFICADOR)
    {
        throw runtime_error("Se espera un identificador despues de 'let");
    }

    string name = actual().valor;
    avanza();

    if (!wait("="))
    {
        throw runtime_error("Se espera un '=' luego del nombre de una variable");
    }

    auto expresion = parseExpre();
    if (!expresion)
    {
        throw runtime_error("Expresión inválida después de '=");
    }
    
    auto asig = make_unique<NodoAST>("ASIGNACION", "=");
    asig->addChild(new NodoAST("VARIABLE", name));
    asig->addChild(expresion.release());
    
    return asig;
}

unique_ptr <NodoAST> ParserRust::parseExpre(){
    cout << "Parseando " << __func__ << " en token: " << actual().valor << endl;
    auto izq = parseTerm();
    if (!izq)
    {
        return nullptr;
    }

    while (actual().tipo == Tipo::OPERADOR && (actual().valor == "+" || actual().valor == "-"))
    {
        string operador = actual().valor;
        avanza();

        auto der = parseTerm();
        if (!der)
        {
            return nullptr;
        }

        auto operacion = make_unique<NodoAST>("OPERACION", operador);
        operacion->addChild(izq.release());
        operacion->addChild(der.release());
        izq = move(operacion);        
    }
    return izq;    
}

unique_ptr <NodoAST> ParserRust::parseTerm(){
    cout << "Parseando " << __func__ << " en token: " << actual().valor << endl;
    //Numeros
    if (actual().tipo == Tipo::NUMERO_DECIMAL || actual().tipo == Tipo::NUMERO_ENTERO || actual().tipo == Tipo::NUMERO_CIENTIFICO)
    {
        string tipe;
        switch (actual().tipo)
        {
        case Tipo::NUMERO_ENTERO: tipe = "ENTERO";
        break;

        case Tipo::NUMERO_DECIMAL: tipe = "DECIMAL";
        break;

        case Tipo::NUMERO_CIENTIFICO: tipe = "CIENTIFICO";
        break;
        
        default:
            tipe = "NUMERO";
        }

        string value = actual().valor;
        avanza();
        return make_unique<NodoAST>(tipe, value);
    }

    //Identificadores y variables
    if (actual().tipo == Tipo::IDENTIFICADOR)
    {
        string name = actual().valor;
        avanza();
        return make_unique<NodoAST>("VARIABLE", name);
    }

    //Caracteres
    if (actual().tipo == Tipo::CARACTER)
    {
        string value = actual().valor;
        avanza();
        return make_unique<NodoAST>("CARACTER", value);
    }
    
    throw runtime_error("Token inesperado en expresión: " + actual().valor + " (Tipo: " + to_string(static_cast<int>(actual().tipo)) + ")");
}

unique_ptr <NodoAST> ParserRust::parseReturn(){
    cout << "Parseando " << __func__ << " en token: " << actual().valor << endl;
    if (!wait("return"))
    {
        return nullptr;
    }
    auto expresion = parseExpre();
    auto ret = make_unique<NodoAST>("RETORNO", "");

    if (expresion)
    {
        ret->addChild(expresion.release());
    }

    if (!wait(";"))
    {
        throw runtime_error("Se espera un ';' despues del return");
    }
    return ret;
}

bool ParserRust::wait(const string& value){
    if (actual().valor == value)
    {
        avanza();
        return true;
    }
    return false;
    
}

bool ParserRust::wait(Tipo tipe){
    if (actual().tipo == tipe)
    {
        avanza();
        return true;
    }
    return false;    
}

ParserRust::~ParserRust() {

}
