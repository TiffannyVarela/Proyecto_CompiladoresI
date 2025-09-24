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
    Logger logger;
    try
    {
        lectura fileReader("prueba4.rs");
        vector<char> fileContent = fileReader.readFile();

        //vector<char> -> string
        string codigo(fileContent.begin(), fileContent.end());

        //Inicializa el logger
        logger.inizializar();

        cout<< "=== Inicio de Analisis ===" << endl;

        bool logConsola = logger.askUserConsola();
        logger.setConsoleOutput(logConsola);

        cout<<"---Archivo de Tokens---"<<endl;
        logger.askFileName("tokens.txt");

        //Analizador Léxico
        cout<<"---Realizando Analisis Lexico---"<<endl;
        LexerRust lexer(codigo);
        auto tokens = lexer.analiza();
        logger.logTokens(tokens);

        cout<<"---Archivo de AST---"<<endl;
        logger.askFileName("ast.txt");

        //Analizador Sintáctico
        cout<<"---Realizando Analisis Sintactico---"<<endl;
        ParserRust parser(tokens);
        auto arbol = parser.parse();

        if (arbol)
        {
            logger.logAST(arbol);
            if (logger.preguntarAST())
            {
                cout<<"---Mostrando AST---"<<endl;
                arbol->print(cout);
            }
        }
        else
        {
            cout<<"No se pudo generar el AST debido a errores."<<endl;
            logger.logError("No se pudo generar el AST debido a errores.", 0, 0, "Errores.txt");
        }
        cout<<"=== Fin de Analisis ==="<<endl;
        cout<<"Resultados en la carpeta: "<< logger.getCarpetaBase() << endl;
        
    }
    catch(const exception& e)
    {
        cerr<<"ERROR: " << e.what() << '\n';
        logger.logError("ERROR: " + string(e.what()), 0, 0);
        return 1;
    }
    return 0;
}