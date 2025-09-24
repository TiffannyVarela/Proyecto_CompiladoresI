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
        lectura fileReader("prueba4.rs");
        vector<char> fileContent = fileReader.readFile();

        //vector<char> -> string
        string codigo(fileContent.begin(), fileContent.end());

        //Analisis Lexico
        LexerRust lexer(codigo);
        auto tokens = lexer.analiza();

        //Logger
        Logger logger;
        logger.inizializar();

        //Preguntar al usuario si quiere ver los tokens en consola
        logger.askUserConsola([&](const string& input)
        {
            return logger.validateYesNo(input);
        });

        //Preguntar al usuario si quiere guardar los tokens en un archivo
        logger.askUserArchivo([&](const string& input){
            return logger.validateYesNo(input);
        }, "Tokens.txt");

        logger.logTokens(tokens);

        //Analisis Sintactico
        ParserRust parser(tokens);
        auto arbol = parser.parse();

        //Imprimir AST en consola
        logger.preguntarAST([&](const string& input){
            return logger.validateYesNo(input);
        });

        //Guardar archivo AST
        logger.askUserArchivo([&](const string& input){
            return logger.validateYesNo(input);
        }, "AST.txt");

        cout << "\n=== AST ===\n"<<endl;
        if (arbol)
        {
            logger.logAST(arbol);
            cout << "AST generado correctamente." << endl;
        }
        else
        {
            cout << "No se pudo generar el AST." << endl;
            logger.logError("No se pudo generar el AST.", 0, 0);
        }
    }
    catch(const exception& e)
    {
        cerr<<"ERROR: " << e.what() << '\n';
        return 1;
    }
    

    return 0;
}

