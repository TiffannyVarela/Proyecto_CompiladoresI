#include "Logger.h"
#include <iostream>
#include <string>
#include <limits>
#include <functional>

using namespace std;

Logger::Logger(){
    this -> console = false;
    this -> file = false;
}

Logger::~Logger(){
    if (output.is_open())
    {
        output.close();
    }
}

void Logger::inizializar(){
    tipeString = {
        {Tipo::CADENA, "CADENA"},
        {Tipo::CARACTER, "CARACTER"},
        {Tipo::COMENTARIO, "COMENTARIO"},
        {Tipo::CONTROL_FLUJO, "CONTROL_FLUJO"},
        {Tipo::ERROR, "ERROR"},
        {Tipo::FIN, "FIN"},
        {Tipo::IDENTIFICADOR, "IDENTIFICADOR"},
        {Tipo::NUMERO_CIENTIFICO, "NUMERO_CIENTIFICO"},
        {Tipo::NUMERO_DECIMAL, "NUMERO_DECIMAL"},
        {Tipo::NUMERO_ENTERO, "NUMERO_ENTERO"},
        {Tipo::OPERADOR, "OPERADOR"},
        {Tipo::PALABRA_RESERVADA, "PALABRA_RESERVADA"},
        {Tipo::PUNTUACION, "PUNTUACION"},
        {Tipo::SIMBOLO, "SIMBOLO"},
        {Tipo::TIPO_ESTANDAR, "TIPO_ESTANDAR"},
        {Tipo::TIPO_PRIMITIVO, "TIPO_PRIMITIVO"}
    };
}

void Logger::logTokens(const vector<Token>& tokens){
    if (console)
    {
        cout << "=== TOKENS ENCONTRADOS ===" << endl;
        cout<<tokens.size()<<endl;

        for (const auto& token : tokens)
        {
            cout<< "[L:" << token.line << ",C:" << token.column << "] "<< tipeString[token.tipo] << ": " << token.valor << endl;
        }
        cout << "=== FIN TOKENS ===" << endl;
    }
    
    if (file)
    {
        try
        {
            output.open(filename, ios::out | ios::trunc);
            if (!output.is_open())
            {
                throw runtime_error("No se pudo abrir el archivo: " + filename);
            }
            output << "=== TOKENS ENCONTRADOS ===" << endl;
            output<<tokens.size()<<endl;

            for (const auto& token : tokens)
            {
                output<< "[L:" << token.line << ",C:" << token.column << "] "<< tipeString[token.tipo] << ": " << token.valor << endl;
            }
            output << "=== FIN TOKENS ===" << endl;
            output.close();
            cout << "\nTokens guardados en '" << filename << "'" <<endl;
        }
        catch(const exception& e)
        {
            cerr << "Error al escribir en archivo: " << e.what() <<endl;
        }
    }   
}

string Logger::toUpper(const string& input){
    string output = input;
    for (auto& c:output)
    {
        c=toupper(c);
    }
    return output;
}

bool Logger::validateYesNo(const string& input){
    string upper = toUpper(input);
    return (upper=="S" || upper=="N");
}

bool Logger::askUserConsola(function<bool(const string&)> validator = nullptr){
    string resp;
    do
    {
        cout << "Mostrar tokens en consola? (S/N): ";
        cin >> resp;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (validator && !validator(resp)) {
            cout << "Entrada no válida. Use S o N.\n";
            continue;
        }
        resp = toUpper(resp);
        console = (resp == "S");
        return console;
    } while (true);
}

bool Logger::askUserArchivo(function<bool(const string&)> validator = nullptr){
    string resp;
    do
    {
        cout << "Mostrar tokens en archivo de texto? (S/N): ";
        cin >> resp;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (validator && !validator(resp)) {
            cout << "Entrada no válida. Use S o N.\n";
            continue;
        }
        resp = toUpper(resp);
        file = (resp == "S");

        if (file)
        {
            cout << "Ingrese nombre del archivo (ej: tokens.txt): ";
            getline(cin, filename);

            if (filename.empty())
            {
                filename = "Tokens.txt";
            }
        }
        return file;
    } while (true);
}

void Logger::setConsoleOutput(bool activo){
    console = activo;
}

void Logger::setFileOutput(bool activo, const string& filename){
    file = activo;
    if (activo && !filename.empty())
    {
        this->filename = filename;
    }  
}

