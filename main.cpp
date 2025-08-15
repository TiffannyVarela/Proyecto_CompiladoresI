#include "lectura.h"
#include "LexerRust.h"
#include "ParserRust.h"
#include "Logger.h"
#include <iostream>
#include <vector>
#include <string>
#include <cctype>


using namespace std;

int main() {
    try
    {
        lectura fileReader("prueba.rs");
        vector<char> fileContent = fileReader.readFile();

        //vector<char> -> string
        string codigo(fileContent.begin(), fileContent.end());

        //Analisis Lexico
        LexerRust lexer(codigo);
        auto tokens = lexer.analiza();

        //Mostrar en consola
        Logger logger;
        logger.askUserConsola([&] (const string& input){
            return logger.validateYesNo(input);
        });

        //Crear archivo de Tokens
        logger.askUserArchivo([&](const string &input){
            return logger.validateYesNo(input);
        });

        logger.inizializar();
        logger.logTokens(tokens);

        //Analisis sintactitco
        ParserRust parser(tokens);
        auto arbol = parser.parse();

        cout << "\n=== ARBOL AST ===" << endl;
        if (arbol)
        {
            arbol->print();
            cout<<"Arbol Impreso Correctamente"<<endl;
        }
        else{
            cerr << "Error: El árbol AST es nulo" << endl;
        }
    }
    catch(const exception& e)
    {
        cerr<<"ERROR: " << e.what() << '\n';
        return 1;
    }
    

    return 0;
}

