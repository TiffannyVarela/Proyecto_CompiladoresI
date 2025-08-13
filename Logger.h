#ifndef LOGGER_H
#define LOGGER_H

#include "Token.h"
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <functional>

using namespace std;

class Logger
{
private:
    string filename;
    ofstream output;
    map<Tipo, string> tipeString;

    bool console;
    bool file;
public:
    Logger();
    void logTokens(const vector<Token>& tokens);
    void inizializar();
    bool askUserConsola(function<bool(const string&)> validator);
    bool askUserArchivo(function<bool(const string&)> validator);
    void setConsoleOutput(bool active);
    void setFileOutput(bool active, const string& filename = "");
    string toUpper(const string& input);
    bool validateYesNo(const string& input);
    ~Logger();
};

#endif