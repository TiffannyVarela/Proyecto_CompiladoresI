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

using namespace std;

class Logger
{
private:
    string filename;
    ofstream output;
    map<Tipo, string> tipeString; // Mapa para convertir Tipo a string

    bool console;
    bool file;
public:
    Logger();
    void logTokens(const vector<Token>& tokens);
    void inizializar();
    bool askUserConsola(function<bool(const string&)> validator=nullptr);
    bool askUserArchivo(function<bool(const string&)> validator, const string& defaultName);
    bool askFileName(const string& defaultName);
    void setConsoleOutput(bool active);
    void setFileOutput(bool active, const string& filename = "");
    string toUpper(const string& input);
    bool validateYesNo(const string& input);
    void logError(const string& mensaje, int line, int column);

    bool preguntarAST(function<bool(const string&)> validator=nullptr);
    void logAST(const unique_ptr<NodoAST>& root);
    ~Logger();
};

#endif