#ifndef ANALIZADORRUST_H
#define ANALIZADORRUST_H

#include <string>
#include <vector>
 using namespace std;

class AnalizadorRust
{
private:
    vector<string> tokens;
public:
    AnalizadorRust(const vector<string>& tokens);
    void contFunciones() const;
    void contVariables() const;
    void listPalabrasReservadas() const;
    void contFuncionesDefinidas() const;
    void contEstructuras() const;
    ~AnalizadorRust();
};

 #endif // ANALIZADORRUST_H