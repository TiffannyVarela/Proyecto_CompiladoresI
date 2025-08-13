#ifndef TOKEN_H
#define TOKEN_H 

#include <string>

using namespace std;

enum class Tipo{
    IDENTIFICADOR,
    PALABRA_CLAVE,
    COMENTARIO,
    NUMERO,
    NUMERO_ENTERO,
    NUMERO_CIENTIFICO,
    NUMERO_DECIMAL,
    SIMBOLO,
    OPERADOR,
    PUNTUACION,
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