#ifndef TOKEN_H
#define TOKEN_H 

#include <string>

using namespace std;

enum class Tipo{
    IDENTIFICADOR,
    PALABRA_CLAVE,
    NUMERO,
    SIMBOLO,
    CADENA,
    CARACTER,
    ERROR,
    FIN
};

struct Token
{
    Tipo tipo;
    string valor;
    int line;
    int column;
};


#endif