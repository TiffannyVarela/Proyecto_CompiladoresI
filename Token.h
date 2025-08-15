#ifndef TOKEN_H
#define TOKEN_H 

#include <string>

using namespace std;

enum class Tipo {
    CADENA,
    CARACTER,
    COMENTARIO,
    CONTROL_FLUJO,
    ERROR,
    FIN,
    IDENTIFICADOR,
    NUMERO_CIENTIFICO,
    NUMERO_DECIMAL,
    NUMERO_ENTERO,
    OPERADOR,
    PALABRA_CLAVE,
    PUNTUACION,
    SIMBOLO,
    TIPO_ESTANDAR,
    TIPO_PRIMITIVO
};

struct Token
{
    Tipo tipo;
    string valor;
    int line;
    int column;
};


#endif