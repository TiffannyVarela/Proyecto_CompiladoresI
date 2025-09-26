#include "ParserRust.h"
#include <iostream>
#include <stdexcept>

using namespace std;

ParserRust::ParserRust(const vector<Token>& tokens) : tokens(tokens), pos(0), profundidad(0) {}

ParserRust::~ParserRust() {}

// Recuperación de errores: Avanza hasta el siguiente punto seguro
void ParserRust::synchronize() {
    // Avanza hasta el siguiente punto seguro (punto y coma, llave de cierre, o inicio de una nueva declaración)
    if (isEnd()) return;

    // Log inicial (útil para depurar dónde se llama synchronize)

    size_t startPos = pos;

    if (!(actual().valor == "}" && profundidad>0))
    {
        avanza();
    }

    int safety = 0;
    while (!isEnd()) {
        if (actual().valor == ";")
        {
            avanza();
            return;
        }
        
        if (actual().valor == "}") 
        {
            if (profundidad > 0)
            {
                // Estamos dentro de un bloque: NO consumimos '}', dejamos que el caller lo cierre.
                return;
            }
            else {
                avanza();
                return;
            }
        }

        if (actual().valor == "fn" || actual().valor == "let" || actual().valor == "if" || actual().valor == "for")
        {
            return;
        }
        avanza();

        // Safety guard: si por alguna razón estamos iterando sin fin, rompa el bucle tras muchas iteraciones.
        if (++safety > 2000) {
            return;
        }
    } 

    // Fallback: si synchronize no avanzó nada (startPos == pos) y no estamos al final,
    // forzar un avance para asegurar progreso (esto previene loops infinitos).
    if (pos == startPos && !isEnd()) {
        avanza();
    }
}

// Función auxiliar para convertir Tipo a string (para mensajes de error)
static string tipeToString(Tipo tipe) {
    switch (tipe) {
        case Tipo::IDENTIFICADOR: return "IDENTIFICADOR";
        case Tipo::NUMERO_ENTERO: return "NUMERO_ENTERO";
        case Tipo::NUMERO_DECIMAL: return "NUMERO_DECIMAL";
        case Tipo::NUMERO_CIENTIFICO: return "NUMERO_CIENTIFICO";
        case Tipo::CADENA: return "CADENA";
        case Tipo::CARACTER: return "CARACTER";
        case Tipo::TIPO_PRIMITIVO: return "TIPO_PRIMITIVO";
        case Tipo::TIPO_ESTANDAR: return "TIPO_ESTANDAR";
        case Tipo::SIMBOLO: return "SIMBOLO";
        case Tipo::FIN: return "FIN";
        default: return "DESCONOCIDO";
    }
}

// Devuelve el token actual o un token de fin si se ha llegado al final
const Token& ParserRust::actual() const {
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    // Retorna un token de fin si se ha llegado al final
    static Token endToken = {Tipo::FIN, "EOF", -1, -1};
    return endToken;
}

// Verifica si se ha llegado al final de los tokens
bool ParserRust::isEnd() const {
    return pos >= tokens.size() || actual().tipo == Tipo::FIN;
}

// Avanza al siguiente token si no se ha llegado al final
void ParserRust::avanza() {
    if (!isEnd()) {
        //actual() devuelve el token actual a consumir
        const string &valor = actual().valor;

        //Actualizar la profundidad de las llaves
        if (valor == "{") {
            //Consume la llave de apertura y aumenta la profundidad
            pos++;
            profundidad++;
            return;
        } else if (valor == "}") {
            //Consume la llave de cierre y disminuye la profundidad
            pos++;
            if (profundidad>0)
            {
                profundidad--;
            }
            return;
        }
        pos++;
    }
}

//Compara el token actual con un valor léxico específico y avanza si coincide
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

//Compara el token actual con un tipo específico y avanza si coincide
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

//Verifica que el token actual coincida con un valor léxico específico, lanza error si no coincide
void ParserRust::expectLexical(const string& value, const char* errorMsg) {
    if (!matchLexical(value)) {
        throw runtime_error("Linea: " + to_string(actual().line) + ", Columna: " + to_string(actual().column) + ". Token esperado '" + value + "', pero se encontro '" + actual().valor + "'. " + errorMsg);
    }
}

//Verifica que el token actual coincida con un tipo específico, lanza error si no coincide
void ParserRust::expectTipo(Tipo tipe, const char* errorMsg) {
    if (!matchTipo(tipe)) {
        throw runtime_error("Linea: " + to_string(actual().line) + ", Columna: " + to_string(actual().column) + ". Tipo de token esperado '" + tipeToString(tipe) + "', pero se encontro '" + actual().valor + "'. " + errorMsg);
    }
}

// Inicia el análisis sintáctico y devuelve la raíz del AST
unique_ptr<NodoAST> ParserRust::parse() {
    return parseProgram();
}

// Analiza el programa completo
unique_ptr<NodoAST> ParserRust::parseProgram() {
    auto programNode = make_unique<NodoAST>("Programa", "");
    while (!isEnd()) {
        try
            {
                if (actual().valor == "fn" || actual().valor == "let" || actual().valor == "if" || actual().valor == "for") {
                    auto declNode = parseDeclaracion();
                    if (declNode) {
                        programNode->addChild(move(declNode));
                    } else {
                        cerr<< "Declaracion nula en la linea " << actual().line << endl;
                        //Error a archivo Errores.txt
                        logger.logError(msg+"Declaracion nula en la linea " + to_string(actual().line), actual().line, actual().column, "Errores.txt");
                        size_t prev = pos;
                        synchronize(); // Intenta recuperar el error
                        if (pos == prev && !isEnd())
                        {
                            avanza(); // Asegura progreso si synchronize no avanzó
                        }
                    }
                } 
                else {
                    cerr << "Token inesperado en la declaracion: " << actual().valor << " en la linea " << actual().line << endl;
                    logger.logError(msg+"Token inesperado en la declaracion: " + actual().valor, actual().line, actual().column, "Errores.txt");
                    size_t prev = pos;
                    synchronize();
                    if (pos == prev && !isEnd())
                    {
                        avanza();
                    }
                }        
            }
            
            catch(const std::exception& e)
            {
                cerr << "Error de sintaxis: " << e.what() << endl;
                logger.logError(msg+"Error de sintaxis: " + string(e.what()), actual().line, actual().column, "Errores.txt");
                size_t prev = pos;
                synchronize();
                if (pos == prev && !isEnd())
                {
                    cerr << "No se avanzo en synchronize, forzando avanza() para evitar loop infinito.\n";
                    logger.logError(msg+"No se avanzo en synchronize, forzando avanza() para evitar loop infinito.", actual().line, actual().column, "Errores.txt");
                    avanza();
                }
            }
        }
    return programNode;
}

//Determina el tipo de declaración y la analiza
unique_ptr<NodoAST> ParserRust::parseDeclaracion() {
    if (actual().valor == "fn") 
    {
        return parseFunc();
    } else if (actual().valor == "let") 
    {
        auto asigNode = parseAsig();
        return asigNode;
    } else if (actual().valor == "if") 
    {
        return parseIf();
    } else 
    {
        throw runtime_error("Declaracion inesperada: " + actual().valor);
    } 
}

//Analiza una declaración de función
unique_ptr<NodoAST> ParserRust::parseFunc() {
    expectLexical("fn", "Esperando la palabra clave 'fn' .");
    expectTipo(Tipo::IDENTIFICADOR, "Esperando nombre de la funcion.");
    string funcName = tokens[pos-1].valor;

    expectLexical("(", "Esperando '(' despues del nombre de la funcion.");
    auto parameters = parseParamOpt();
    expectLexical(")", "Esperando ')' despues de los parametros de la funcion.");

    auto funcNode = make_unique<NodoAST>("Funcion", funcName);
    funcNode->addChild(move(parameters));

    //Para soportar funciones con tipo de retorno
    if (matchLexical("->")) {
        if(!(matchTipo(Tipo::IDENTIFICADOR) || matchTipo(Tipo::TIPO_PRIMITIVO) || matchTipo(Tipo::TIPO_ESTANDAR)))
        {
            throw runtime_error("Esperando tipo de retorno despues de '->' en la linea" + to_string(actual().line));
        }
        string retornoTipo = tokens[pos-1].valor;
        auto retNode = make_unique<NodoAST>("Tipo Retorno", retornoTipo);
        funcNode->addChild(move(retNode));
    }
    else {
        //Tipo de retorno por defecto (void)
        auto retNode = make_unique<NodoAST>("Tipo Retorno", "void");
        funcNode->addChild(move(retNode));
    }
    //funcNode->addChild(move(parameters));
    funcNode->addChild(move(parseBloque()));//Cuerpo de la funcion

    return funcNode;
}

//Analiza parámetros de función opcionales
unique_ptr<NodoAST> ParserRust::parseParamOpt() {
    auto paramsNode = make_unique<NodoAST>("Parametros", "");
    while (actual().tipo == Tipo::IDENTIFICADOR) {
        string paramName = actual().valor;
        avanza();
        string paramType;
        //Para soportar fn miFuncion(x: i32, y: f64) {...}
        if (matchLexical(":")) {
            if (!(matchTipo(Tipo::IDENTIFICADOR) || matchTipo(Tipo::TIPO_PRIMITIVO) || matchTipo(Tipo::TIPO_ESTANDAR)))
            {
                throw runtime_error("Esperando tipo despues de ':' en la linea " + to_string(actual().line));
            }
            paramType = tokens[pos-1].valor;
        }

        auto paramNode = make_unique<NodoAST>("Parametro", paramName + (paramType.empty() ? "" : ": " + paramType));
        paramsNode->addChild(move(paramNode));
        
        if (!matchLexical(",")) { //Si no hay coma, termina la lista de parámetros
            break;
        }
    }
    return paramsNode;
}

//Analiza una declaración if-else
unique_ptr<NodoAST> ParserRust::parseIf() {
    expectLexical("if", "Esperando palabra clave 'if'.");
    auto conditionNode = parseExpre();//Condición del if
    
    auto ifNode = make_unique<NodoAST>("If", "");
    ifNode->addChild(move(conditionNode));
    
    ifNode->addChild(move(parseBloque()));//Cuerpo del if
    
    //Para manejar else if y else
    if (matchLexical("else")) {
        if (matchLexical("if")) {//else if
            ifNode->addChild(move(parseIf()));
        } else {//else
            ifNode->addChild(move(parseBloque()));
        }
    }
    
    return ifNode;
}

//Analiza una declaración for
unique_ptr<NodoAST> ParserRust::parseFor() {
    expectLexical("for", "Esperando la palabra clave 'for'.");
    expectTipo(Tipo::IDENTIFICADOR, "Esperando identificador para la variable del for.");
    string loopVar = tokens[pos-1].valor;
    expectLexical("in", "Esperando la palabra clave 'in' despues de la variable del for.");
    auto rangeNode = parseExpre(); //Rango o colección a iterar
    auto forNode = make_unique<NodoAST>("For", "");
    forNode->addChild(move(rangeNode));    
    forNode->addChild(move(parseBloque()));//Cuerpo del for
    return forNode;
}

//Analiza un bloque de código delimitado por llaves
unique_ptr<NodoAST> ParserRust::parseBloque() {
    expectLexical("{", "Esperando '{' para iniciar el bloque.");
    auto blockNode = make_unique<NodoAST>("Bloque", "");
    
    //Analiza las sentencias dentro del bloque
    while (!isEnd() && actual().valor != "}") {
        try
        {
            blockNode->addChild(move(parseSentence()));
        }
        catch(const std::exception& e)
        {
            cerr << "Error de sintaxis en el bloque: " << e.what() << endl;
            logger.logError(msg+"Error de sintaxis en el bloque: " + string(e.what()), actual().line, actual().column, "Errores.txt");
            size_t prev = pos;
            synchronize();
            if (pos == prev && !isEnd())
            {
                cerr << "No se avanzo en synchronize, forzando avanza() para evitar loop infinito.\n";
                logger.logError(msg+"No se avanzo en synchronize, forzando avanza() para evitar loop infinito.", actual().line, actual().column, "Errores.txt");
                avanza();
            }
        }
    }
    expectLexical("}", "Esperando '}' para finalizar el bloque.");
    return blockNode;
}

//Analiza una sentencia individual
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
        expectLexical(";", "Esperando ';' despues de la expresion.");
        return exprNode;
    }
}

//Analiza una sentencia return
unique_ptr<NodoAST> ParserRust::parseReturn() {
    expectLexical("return", "Esperando palabra clave 'return'.");
    auto returnNode = make_unique<NodoAST>("Return", "");
    
    //Para soportar return; y return expr;
    if (!isEnd() && actual().valor != ";") {
        returnNode->addChild(move(parseExpre()));
    }
    
    expectLexical(";", "Esperando ';' despues del 'return'.");
    return returnNode;
}

//Analiza una asignación de variable
unique_ptr<NodoAST> ParserRust::parseAsig() {
    expectLexical("let", "Esperando palabra clave 'let' para asignar.");
    expectTipo(Tipo::IDENTIFICADOR, "Esperando identificador para el nombre de la variable.");

    string varName = tokens[pos-1].valor;
    string varType;

    //Para soportar let x: i32 = ....;
    if (matchLexical(":")) {
        if (!(matchTipo(Tipo::IDENTIFICADOR) || matchTipo(Tipo::TIPO_PRIMITIVO) || matchTipo(Tipo::TIPO_ESTANDAR)))
        {
            throw runtime_error("Esperando tipo despues de ':' en la linea " + to_string(actual().line));
        }
        varType = tokens[pos-1].valor;
    }
    
    auto asigNode = make_unique<NodoAST>("Asignacion", varName + (varType.empty() ? "" : ": " + varType));
    
    expectLexical("=", "Esperando '=' para la asignacion.");
    asigNode->addChild(move(parseExpre()));//Valor asignado a la variable
    
    expectLexical(";", "Esperando ';' despues de la asignacion.");
    return asigNode;
}

// Analiza una expresión completa
unique_ptr<NodoAST> ParserRust::parseExpre() {
    return parseComp();
}

// Analiza comparaciones
unique_ptr<NodoAST> ParserRust::parseComp() {
    auto node = parseSum();
    while (matchLexical("==") || matchLexical("!=") || matchLexical("<") || matchLexical("<=") || matchLexical(">") || matchLexical(">=")) {
        auto opNode = make_unique<NodoAST>("Operador Comparacion", tokens[pos-1].valor);
        opNode->addChild(move(node));
        opNode->addChild(move(parseSum()));
        node = move(opNode);
    }
    return node;
}

// Analiza sumas y restas
unique_ptr<NodoAST> ParserRust::parseSum() {
    auto node = parseMultDiv();
    while (matchLexical("+") || matchLexical("-")) {
        auto opNode = make_unique<NodoAST>("Operador", tokens[pos-1].valor);
        opNode->addChild(move(node));
        opNode->addChild(move(parseMultDiv()));
        node = move(opNode);
    }
    return node;
}

// Analiza multiplicaciones, divisiones y módulos
unique_ptr<NodoAST> ParserRust::parseMultDiv() {
    auto node = parseUn();
    while (matchLexical("*") || matchLexical("/") || matchLexical("%")) {
        auto opNode = make_unique<NodoAST>("Operador", tokens[pos-1].valor);
        opNode->addChild(move(node));
        opNode->addChild(move(parseUn()));
        node = move(opNode);
    }
    return node;
}

// Analiza operadores unarios
unique_ptr<NodoAST> ParserRust::parseUn() {
    if (matchLexical("+") || matchLexical("-")) {
        auto opNode = make_unique<NodoAST>("Operador Unario", tokens[pos-1].valor);
        opNode->addChild(move(parsePrim()));
        return opNode;
    }
    return parsePrim();
}

// Analiza operandos primarios
unique_ptr<NodoAST> ParserRust::parsePrim() {
    if (actual().tipo == Tipo::NUMERO_ENTERO || actual().tipo == Tipo::NUMERO_DECIMAL || actual().tipo == Tipo::NUMERO_CIENTIFICO) 
    {
        auto numNode = make_unique<NodoAST>("Numero", actual().valor);
        avanza();
        return numNode;
    } 
    else if (actual().tipo == Tipo::CADENA) 
    {
        auto strNode = make_unique<NodoAST>("Cadena de Caracteres", actual().valor);
        avanza();
        return strNode;
    } 
    else if (actual().tipo == Tipo::CARACTER) 
    {
        auto charNode = make_unique<NodoAST>("Caracter", actual().valor);
        avanza();
        return charNode;
    } 
    else if (actual().tipo == Tipo::IDENTIFICADOR) 
    {

        //Identificador (variable, funcion, macro, namespace/module) String::from...
        string idValue = actual().valor;
        avanza();

        //Llamada a macro
        if (matchLexical("!"))
        {
            expectLexical("(", "Esperando '(' despues del identificador para la llamada de la macro.");
            auto callNode = make_unique<NodoAST>("Llamada a Macro", idValue + "!");
            if (actual().valor != ")")
            {
                callNode->addChild(move(parseExpre()));
                while (matchLexical(","))//Si hay más argumentos
                {
                    callNode->addChild(move(parseExpre()));
                }
            }
            expectLexical(")", "Esperando ')' despues de los argumento de la macro.");
            return callNode;
        }

        //Si encuentra ::
        if (matchLexical("::"))
        {
            expectTipo(Tipo::IDENTIFICADOR, "Esperando identificador despues de '::' para el acceso al modulo.");
            idValue += "::" + tokens[pos-1].valor;
        }
        
        //Llamada a funcion
        if (matchLexical("("))
        {
            auto callNode = make_unique<NodoAST>("Llamada a Funcion", idValue);
            if (actual().valor != ")")
            {
                callNode->addChild(move(parseExpre()));
                while (matchLexical(","))//Si hay más argumentos
                {
                    callNode->addChild(move(parseExpre()));
                }
            }
            expectLexical(")", "Esperando ')' despues de los argumento para llamar a la funcion.");
            return callNode;
        }

        //Identificador simple
        return make_unique<NodoAST>("Identificador", idValue);
    } 
    else if (matchLexical("(")) //Expresión entre paréntesis
    {
        auto exprNode = parseExpre();
        expectLexical(")", "Esperando ')' despues de la expresion.");
        return exprNode;
    }
    
    throw runtime_error("Token inesperando en la expresion: " + actual().valor + " en la linea " + to_string(actual().line) + ", columna " + to_string(actual().column));
}

//Loguea el avance al siguiente token para depuración
void ParserRust::logAvance(const string* value) const {
    if (value) {
        cout << "Avanzando a token: " << *value << endl;
        cout << "[Parser] " << *value << " → Token: " << actual().valor<< " (linea " << actual().line << ")\n";
    } else {
        cout << "Avanzando al final de los tokens." << endl;
        cout << "[Parser] (null) → Token: " << actual().valor<< " (linea " << actual().line << ")\n";
    }

    cout << "[Parser] " << value << " → Token: " << actual().valor<< " (linea " << actual().line << ")\n";
}
