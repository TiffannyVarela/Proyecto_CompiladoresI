#include "Logger.h"
#include <iostream>
#include <string>
#include <limits>
#include <functional>

using namespace std;

Logger::Logger(){
    this -> console = false; //Por defecto no muestra en consola
    this -> file = false; //Por defecto no guarda en archivo
    this -> filename = ""; //Por defecto no tiene nombre de archivo
}

Logger::~Logger(){
    if (output.is_open())//Verifica si el archivo está abierto antes de cerrarlo
    {
        output.close(); //Cierra el archivo si está abierto
    }
}

void Logger::inizializar(){
    //Inicializa el mapa de tipos a string
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

// Loguea los tokens en consola y/o archivo según la configuración
void Logger::logTokens(const vector<Token>& tokens){
    if (console) //Si está activado el log en consola
    {
        cout << "=== TOKENS ENCONTRADOS ===" << endl;
        cout<<tokens.size()<<endl;//Muestra la cantidad de tokens encontrados

        //Muestra cada token con su tipo y valor
        for (const auto& token : tokens)
        {
            cout<< "[L:" << token.line << ",C:" << token.column << "] "<< tipeString[token.tipo] << ": " << token.valor << endl;
        }
        cout << "=== FIN TOKENS ===" << endl;
    }
    
    if (file) //Si está activado el log en archivo
    {
        try
        {
            //Abre el archivo para sobreescribir
            output.open(filename, ios::out | ios::trunc);
            if (!output.is_open())
            {
                throw runtime_error("No se pudo abrir el archivo: " + filename);
            }
            //Escribe los tokens en el archivo
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

//Convierte una cadena a mayúsculas
string Logger::toUpper(const string& input){
    string output = input;
    for (auto& c:output)
    {
        c=toupper(c);
    }
    return output;
}

//Valida si la entrada es 'S' o 'N'
bool Logger::validateYesNo(const string& input){
    string upper = toUpper(input);
    return (upper=="S" || upper=="N");
}

//Pregunta al usuario si desea loguear en consola
bool Logger::askUserConsola(function<bool(const string&)> validator){
    string resp;
    do
    {
        cout << "Mostrar tokens/AST en consola? (S/N): ";
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

//Pregunta al usuario si desea loguear en archivo y el nombre del archivo
bool Logger::askUserArchivo(function<bool(const string&)> validator, const string& defaultName){
    string resp;
    do
    {
        cout <<"Desea guardar en un archivo de texto? (S/N): ";
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
            cout<< "Ingrese nombre del archivo (ej: "<<defaultName<<") o presione Enter para usar el nombre por defecto: ";
            getline(cin, filename);
            if (filename.empty())
            {
                filename = defaultName; //Por si acaso un nombre por defecto
            }
        }
        return file;
    } while (true);
}

//Pregunta al usuario si desea usar el nombre por defecto o ingresar uno nuevo
bool Logger::askFileName(const string& defaultName){
    string resp;
    cout << "Desee usar el nombre por defecto '" << defaultName << "'? (S/N): ";
    do
    {
        cin >> resp;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        resp = toUpper(resp);
        if (resp != "S" && resp != "N") {
            cout << "Entrada no válida. Use S o N: ";
            continue;
        }

        if (resp == "N")
        {
            cout << "Ingrese nombre del archivo (ej: "<<defaultName<<"): ";
            getline(cin, filename);
            if (filename.empty())
            {
                filename = defaultName;
            }
        }
        else
        {
            filename = defaultName; //Por si acaso
        }
        file = true;
        return file;
        
    } while (true);
    
}

//Activa o desactiva el log en consola
void Logger::setConsoleOutput(bool activo){
    console = activo;
}

//Activa o desactiva el log en archivo y opcionalmente establece el nombre del archivo
void Logger::setFileOutput(bool activo, const string& filename){
    file = activo;
    if (activo && !filename.empty())
    {
        this->filename = filename;
    }  
}

//Loguea un mensaje de error con línea y columna opcionales
void Logger::logError(const string& mensaje, int line, int column) {

    string logErrorMsg;
    // Formatea el mensaje de error
    if (line >= 0 && column >= 0) {
        logErrorMsg = "[L:" + to_string(line) + ",C:" + to_string(column) + "] ERROR: " + mensaje;
    } else {
        logErrorMsg = "ERROR: " + mensaje;
    }
    //Muestra en consola el error
    if (console) {
        cout << logErrorMsg << endl;
    }
    // Guarda en archivo el error
    if (!filename.empty())
    {
        try
        {
            ofstream errorOut(filename, ios::out | ios::app);
            if (!errorOut.is_open())
            {
                throw runtime_error("No se pudo abrir el archivo: " + filename);
            }
            errorOut<< "\n[Error] Linea: " << line << ", Columna: " << column << " - " << mensaje << endl;
            errorOut.close();
        }
        catch(const exception& e)
        {
            cerr<< "Error al escribir en archivo: " << e.what() <<endl;
        }
        
    }
    
}

//Pregunta al usuario si desea mostrar el AST en consola
bool Logger::preguntarAST(function<bool(const string&)> validator){
    string resp;
    do
    {
        cout << "Mostrar AST en consola? (S/N): ";
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

//Loguea el AST en consola y/o archivo según la configuración
void Logger::logAST(const unique_ptr<NodoAST>& root){
    if (!root)
    {
        logError("AST vacío o no generado.", 0, 0);
        return;
    }

    if (console)
    {
        cout << "=== AST ===" << endl;
        root->print(cout);
        cout << "=== FIN AST ===" << endl;
    }
    
    try
    {
        ofstream astOut(filename, ios::out | ios::trunc);
        if (!astOut.is_open())
        {
            throw runtime_error("No se pudo abrir el archivo: " + filename);
        }
        else {
            astOut << "=== AST ===" << endl;
            root->print(astOut);
            astOut << "=== FIN AST ===" << endl;
            astOut.close();
            cout << "\nAST guardado en '" << filename << "'" <<endl;
        }
    }
    catch(const exception& e)
    {
        cerr << "Error al escribir en archivo: " << e.what() <<endl;
    }      
}