#include "lectura.h"
#include <iostream>
#include <fstream>

using namespace std;

lectura::lectura(const string& filePath){
    this->filePath = filePath;
}

vector<char> lectura::readFile() {
    ifstream file(filePath, ios::binary);
    if (!file) {
        cerr << "Error opening file: " << filePath << endl;
        return {};
    }

    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(fileSize);
    if (file.read(buffer.data(), fileSize)) {
        return buffer;
    } else {
        cerr << "Error reading file: " << filePath << endl;
        return {};
    }
    
    cerr << "File is empty or could not be read: " << filePath << endl;
    return {};
    
}