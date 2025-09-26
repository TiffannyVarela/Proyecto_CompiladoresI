#include "lectura.h"
#include "LexerRust.h"
#include "ParserRust.h"
#include "Logger.h"
#include <iostream>
#include <vector>
#include <string>
#include <cctype>


using namespace std;
namespace fs = std::filesystem;

int main() {
    Logger logger;
    try
    {
        //Inicializa el logger
        logger.inizializar();

        cout<< "=== Inicio de Analisis ===" << endl;
        cout<<"Carpeta de resultados: " << logger.getCarpetaBase() << endl<<endl;

        fs::path ruta = fs::current_path() / "Pruebas" / "prueba.rs";
        
        if (!fs::exists(ruta))
        {
            cerr<<"Error: No se encontro el archivo en la ruta especificada: "<< ruta << endl;
            return 1;
        }
        

        lectura fileReader(ruta.string());
        vector<char> fileContent = fileReader.readFile();

        //vector<char> -> string
        string codigo(fileContent.begin(), fileContent.end());

        //Analizador Léxico
        cout<<"---Realizando Analisis Lexico---"<<endl;
        //Mostrar en consola
        bool logConsole = logger.askUserConsola();
        logger.setConsoleOutput(logConsole);
        //Crear archivo para los tokens
        logger.setFileOutput(true, "tokens.txt");
        logger.askFileName("tokens.txt");
        LexerRust lexer(codigo);
        auto tokens = lexer.analiza();
        logger.logTokens(tokens);


        //Analizador Sintáctico
       cout<<"---Realizando Analisis Sintactico---"<<endl;

       //Pregunta si mostrar el AST en consola
        bool mostrarConsola = logger.preguntarAST();

        //Crear archivo para el AST
        logger.setFileOutput(true, "AST.txt");
        logger.askFileName("AST.txt");

        ParserRust parser(tokens);
        auto arbol = parser.parse();
       
        

        if (arbol)
        {
            logger.logAST(arbol);
            if (mostrarConsola)
            {
                cout<<"---AST Mostrado---"<<endl;
            }
        }
        else
        {
            cout<<"No se pudo generar el AST completo debido a errores."<<endl;
            logger.logAST(nullptr);
        }
        cout<<"=== Fin de Analisis ==="<<endl;        
    }
    catch(const exception& e)
    {
        logger.logError("ERROR FATAL: " + string(e.what()), 0, 0, "errores.txt");
        cerr<<"ERROR: " << e.what() << '\n';
        return 1;
    }
    return 0;
}