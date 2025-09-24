#include "Logger.h"
#include <iostream>
#include <string>
#include <limits>
#include <functional>

using namespace std;

Logger::Logger(){
    this -> console = false; //Por defecto no muestra en consola
    this -> file = true; //Por defecto guarda en archivo
    this -> filename = "output.txt"; //Nombre por defecto
    this -> carpetaBase = "resultados"; //Carpeta base por defecto
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

    //Crear la carpeta base si no existe
    crearCarpetaResult();
}

//Obtener la fecha
string Logger::obtenerFecha(){
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm tm = *localtime(&now_c);

    stringstream ss;
    ss << carpetaBase <<"/" << (tm.tm_year + 1900) << "-" 
       << setw(2) << setfill('0') << (tm.tm_mon + 1) << "-" 
       << setw(2) << setfill('0') << tm.tm_mday << "_"
       << setw(2) << setfill('0') << tm.tm_hour << "-"
       << setw(2) << setfill('0') << tm.tm_min << "-"
       << setw(2) << setfill('0') << tm.tm_sec;

       return ss.str();
}

//Crea la carpeta para resultados si no existe
void Logger::crearCarpetaResult(){
    string carpetaFecha = obtenerFecha();
    try
    {
        //Crea la carpeta base si no existe
        if (!filesystem::exists(carpetaBase))
        {
            filesystem::create_directory(carpetaBase);
            cout << "Carpeta '" << carpetaBase << "' creada para resultados." << endl;
        }

        //Crea la subcarpeta con la fecha y hora actual
        if (!filesystem::exists(carpetaFecha))
        {
            filesystem::create_directory(carpetaFecha);
            carpetaBase = carpetaFecha; //Actualiza la carpeta base a la de fecha
            cout << "Carpeta '" << carpetaFecha << "' creada para resultados." << endl;
        }
        
    }
    catch(const filesystem::filesystem_error& e)
    {
        cerr << "Error al crear la carpeta de resultados: " << e.what() << endl;
    }
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
    
    if (file)
    {
        try
        {
            //Abre el archivo para sobreescribir
            output.open(filename, ios::out | ios::trunc);
            if (!output.is_open())
            {
                logError("No se pudo abrir el archivo: " + filename);
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
            logError("Error al escribir en archivo: " + string(e.what()), 0, 0);
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
bool Logger::askUserConsola(){
    string resp;
    do
    {
        cout << "Mostrar tokens/AST en consola? (S/N): ";
        cin >> resp;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (!validateYesNo(resp)) {
            cout << "Entrada no válida. Use S o N.\n";
            continue;
        }
        resp = toUpper(resp);
        console = (resp == "S");
        return console;
    } while (true);
}

//Pregunta al usuario si desea loguear en archivo y el nombre del archivo
bool Logger::askUserArchivo(const string& defaultName){
    string resp;
    do
    {
        cout <<"Desea guardar en un archivo de texto? (S/N): ";
        cin >> resp;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (!validateYesNo(resp)) {
            cout << "Entrada no válida. Use S o N.\n";
            continue;
        }
        resp = toUpper(resp);
        file = (resp == "S");

        if (file)
        {
            askFileName(defaultName);
        }
        return file;
    } while (true);
}

//Pregunta al usuario si desea usar el nombre por defecto o ingresar uno nuevo
bool Logger::askFileName(const string& defaultName){
    string resp;
    cout << "Desea usar el nombre por defecto '" << defaultName << "'? (S/N): ";
    do
    {
        cin >> resp;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        resp = toUpper(resp);
        if (!validateYesNo(resp)) {
            cout << "Entrada no válida. Use S o N: ";
            continue;
        }
         resp = toUpper(resp);
        if (resp == "S")
        {
            filename = carpetaBase + "/" + defaultName;
        }
        else
        {
            cout<< "Ingrese el nombre del archivo (con .txt): ";
            string personalizado;
            getline(cin, personalizado);
            if (personalizado.empty()){
                filename = carpetaBase + "/" + defaultName;
            }
            else {
                filename = carpetaBase + "/" + personalizado;
            }
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
        this->filename = carpetaBase + "/" + filename;
    }  
}

//Loguea un mensaje de error con línea y columna opcionales
void Logger::logError(const string& mensaje, int line, int column, const string& archivoErrores) {

    string logMSJError;
    if (line >= 0 && column >= 0) {
        logMSJError = "[L:" + to_string(line) + ",C:" + to_string(column) + "] ERROR: " + mensaje;
    } else {
        logMSJError = "ERROR: " + mensaje;
    }

    string archivo = carpetaBase + "/" + archivoErrores;

    try
    {
        ofstream errorOut(archivo, ios::out | ios::app);
        if (errorOut.is_open())
        {
            errorOut<<logMSJError<<endl;
            errorOut.close();
            cout << "\nError guardado en '" << archivo << "'" <<endl;
        }
        else
        {
            throw runtime_error("No se pudo abrir el archivo: " + archivo);
        }
    }
    catch(const std::exception& e)
    {
        cerr << "Error al escribir en archivo: " << e.what() <<endl;
    }

    if (console)
    {
        cerr << logMSJError << endl;
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
        
        if (!validateYesNo(resp)) {
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

    if (file)
    {
        try
        {
            output.open(filename, ios::out | ios::trunc);
            if (!output.is_open())
            {
                logError("No se pudo abrir el archivo: " + filename);
                return;
            }
            output<< "=== AST ===" << endl;
            root->print(output);
            output<< "=== FIN AST ===" << endl;
            output.close();
            cout << "\nAST guardado en '" << filename << "'" <<endl;
        }
        catch(const exception& e)
        {
            logError("Error al escribir en archivo: " + string(e.what()), 0, 0);
        }
    }      
}

void Logger::logErrorInterno(const string& mensaje, int line, int column, const string& filename){
    string logMSJError;
    if (line >= 0 && column >= 0) {
        logMSJError = "[L:" + to_string(line) + ",C:" + to_string(column) + "] ERROR: " + mensaje;
    } else {
        logMSJError = "ERROR: " + mensaje;
    }

    try
    {
        ofstream errorOut(filename, ios::out | ios::app);
        if (errorOut.is_open())
        {
            errorOut<<logMSJError<<endl;
            errorOut.close();
        }
    }
    catch(const exception& e)
    {
        cerr<< "Error al escribir en archivo: " << e.what() <<endl;
    }
    
}