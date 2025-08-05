#ifndef LECTURA_H
#define LECTURA_H

#include <string>
#include <vector>
using namespace std;

class lectura
{
private:
    string filePath;
public:
    lectura(const string& filePath);
    vector<char> readFile();
};

#endif
