#include "lectura.h"
#include <iostream>
#include <fstream>

using namespace std;

lectura::lectura(const string& filePath){
    this->filePath = filePath;
}

//Metodo para leer un archivo binario y devolver su contenido en un vector de caracteres
vector<char> lectura::readFile() {
    // Abrir el archivo en modo binario
    ifstream file(filePath, ios::binary);
    // Verificar si el archivo se abrió correctamente
    if (!file) {
        cerr << "Error para abrir el archivo: " << filePath << endl;
        return {}; // Retornar un vector vacío en caso de error
    }
    //Mueve el puntero al final del archivo
    file.seekg(0, ios::end);
    //Obtiene la posicion del puntero, que es el tamaño del archivo
    size_t fileSize = file.tellg();
    //Mueve el puntero al inicio del archivo
    file.seekg(0, ios::beg);

    //Crear un vector para almacenar el contenido del archivo
    vector<char> buffer(fileSize);
    //Leer el contenido del archivo y almacenarlo en el vector
    if (file.read(buffer.data(), fileSize)) {
        return buffer;
    } else {
        cerr << "Error para leer el archivo: " << filePath << endl;
        return {};// Retornar un vector vacío en caso de error
    }
    
    cerr << "El archivo esta vacio o no puede ser abierto y leido: " << filePath << endl;
    return {};
    
}