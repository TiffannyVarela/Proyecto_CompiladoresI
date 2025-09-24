#ifndef LOGGER_H
#define LOGGER_H

#include "Token.h"
#include "NodoAST.h"
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <functional> //Libreria para usar funciones como parametros
#include <limits> //Libreria para limites numericos
#include <memory> // Libreria para smart pointers
#include <filesystem> // Libreria para manejo de archivos

using namespace std;

class Logger
{
private:
    string filename;
    ofstream output;
    map<Tipo, string> tipeString; // Mapa para convertir Tipo a string

    bool console;
    bool file;

    string carpetaBase;
    void crearCarpetaResult();
    string obtenerFecha();
    void logErrorInterno(const string& mensaje, int line, int column, const string& filename);
public:
    Logger();
    void inizializar();

    void logTokens(const vector<Token>& tokens);
    bool askUserConsola();
    bool askUserArchivo(const string& defaultName);
    bool askFileName(const string& defaultName);
    void setConsoleOutput(bool active);
    void setFileOutput(bool active, const string& filename = "");
    string toUpper(const string& input);
    bool validateYesNo(const string& input);
    void logError(const string& mensaje, int line, int column, const string& filename="Errores.txt");
    bool preguntarAST();
    void logAST(const unique_ptr<NodoAST>& root);
    void logErrorInterno(const string& mensaje, int line, int column, const string& filename);

    string getCarpetaBase() const {return carpetaBase;}
    void setCarpetaBase(const string& carpeta) {carpetaBase = carpeta;}
    
    ~Logger();
};

#endif