#ifndef LOGGER_H
#define LOGGER_H

#include "Token.h"
#include "NodoAST.h"
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <limits> //Libreria para limites numericos
#include <memory> // Libreria para smart pointers
#include <filesystem> // Libreria para manejo de archivos
#include <chrono> // Libreria para manejo de tiempo y fechas
#include <sstream> // Libreria para manejo de streams
#include <iomanip> // Libreria para formateo de fechas

using namespace std;
namespace fs = std::filesystem;

class Logger
{
private:
    string filename;
    ofstream output;
    map<Tipo, string> tipeString; // Mapa para convertir Tipo a string

    bool console;
    bool consoleAST;
    bool file;

    string carpetaBase;
    string carpetaPruebas;

    string obtenerFechaHora();
    bool carpetaExiste(const string& path);

    void logASTInternal(const unique_ptr<NodoAST>& root);

    void crearDirectorio(const string& paths);

    bool headerErrores = false;

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
    void logError(const string& mensaje, int line = -1, int column = -1, const string& nombreArchivoErrores="Errores.txt");
    bool preguntarAST();
    void logAST(const unique_ptr<NodoAST>& root);

    string getCarpetaBase() const {return carpetaBase;}
    void setCarpetaBase(const string& carpeta) {carpetaBase = carpeta;}
    
    ~Logger();
};

#endif