#include "LexerRust.h"
#include <cctype>
#include <set>
#include <iostream>
#include <map>

using namespace std;

LexerRust::LexerRust(const string& codigo) : codigo (codigo), pos(0), line(1), column(1) {
    palabrasClave = {"as", "async", "await", "break", "bucle", "const", "condicional",  "continue", "crate", "declaration", "dyn", "else", 
        "enum", "extern", "false", "fn", "for", "funcion","if", "impl", "in", "let", "loop", 
        "match", "mod", "move", "mut", "mutable", "None", "pub", "ref", "return", "self", "Self", "Some",
        "static", "struct", "super", "trait", "true", "type", "union", "unsafe", 
        "use", "where", "while"};
    simbolosCompuestos = {
        "==", "!=", "<=", ">=", "&&", "||", "+=", "-=", "*=", "/=", "%=", 
        "->", "::", "..", "..=", "=>", "|=", "&=", "^=", "<<", ">>", "<<=", ">>="
    };
}

vector <Token> LexerRust::analiza(){
    vector<Token> tokens;
    while (pos < codigo.size())
    {
        char c = codigo[pos];

//Saltar espacios en blanco
        if (isspace(c))
        {
            if (c == '\n')
            {
                line++;
                column=1;
            }
            else{
                column++;
            }
            pos++;
            continue;            
        }    
//Comentarios una sola linea
        if (c == '/' && pos +1 < codigo.size() && codigo[pos+1] == '/')
        {
            comentarioLinea(tokens);
            continue;
        }
//Comentarios de bloque
        if (c == '/' && pos +1 < codigo.size() && codigo[pos+1] == '*')
        {
            comentarioBloque(tokens);
            continue;
        }
        
//Palabras clave e identificadores
        if(isalpha(c) || c == '_'){
            identificador(tokens);
            continue;            
        } 

//Numeros
        if (isdigit(c))
        {
            numero(tokens);
            continue;
        }

//Cadena de caracteres
        if (c== '"')
        {
            cadena(tokens);
            continue;
        }

//Comillas simples
        if (c == '\'')
        {
            caracter(tokens);
            continue;
        }
        
//Simbolos y operadores
        simbolos(tokens);
        
    }

    tokens.push_back({Tipo::FIN, "", line, column});
    return tokens;
}

void LexerRust::comentarioLinea(vector<Token>& tokens){
    int iniLine = line;
    int iniCol = column;
    string lex = "//";
    pos+=2;
    column+=2;

    while (pos < codigo.size() && codigo[pos] != '\n')
    {
        lex += codigo[pos++];
        column++;
    }
    tokens.push_back({Tipo::COMENTARIO, lex, iniLine, iniCol});    
}

void LexerRust::comentarioBloque(vector<Token>& tokens){
    int iniLine = line;
    int iniCol = column;
    string lex = "/*";
    pos+=2;
    column+=2;
    bool closed = false;

    while (pos < codigo.size())
    {
        if (codigo[pos] == '*' && pos +1 < codigo.size() && codigo[pos+1] == '/')
        {
            lex += "*/";
            pos +=2;
            column +=2;
            closed = true;
            break;
        }
        
        if (codigo[pos] =='\n')
        {
            line++;
            column=1;
        }
        else{
            column++;
        }
        lex += codigo[pos++];
    }
    if (!closed)
    {
        cerr<< "Error: comentario de bloque no cerrado en línea " << iniLine << ", columna " << iniCol << endl;
    }
    
    tokens.push_back({Tipo::COMENTARIO, lex, iniLine, iniCol});
}

void LexerRust::identificador(vector<Token>& tokens){
    int iniLine = line;
    int iniCol = column;
    string lex;

    while (pos < codigo.size() && (isalnum(codigo[pos]) || codigo[pos] == '_'))
    {
        lex +=codigo[pos++];
        column++;
    }

    Tipo tipo = palabrasClave.count(lex) ? Tipo::PALABRA_CLAVE : Tipo::IDENTIFICADOR;
    tokens.push_back({tipo, lex, iniLine, iniCol});
    
}

void LexerRust::numero(vector<Token>& tokens){
    int iniLine = line;
    int iniCol = column;
    string lex;
    bool decimal = false;
    bool cientifico = false; //prueba

    //Entero
    while (pos < codigo.size() && (isdigit(codigo[pos]) || codigo[pos] == '_'))
    {
        if (codigo[pos] != '_')
        {
            lex += codigo[pos];
        }
        pos++;
        column++;        
    }

    //Decimal
    if (pos < codigo.size() && codigo[pos] == '.')
    {
        decimal = true;
        lex += codigo[pos++];
        column++;

        while (pos < codigo.size() && (isdigit(codigo[pos]) || codigo[pos] == '_'))
        {
            if (codigo[pos] != '_')
            {
                lex += codigo[pos];
            }
            pos++;
            column++;
        }
    }
    
    //Exponencial
    if (pos < codigo.size() && (codigo[pos] == 'e' || codigo[pos] == 'E'))
    {
        cientifico = true;
        lex += codigo[pos++];
        column++;
        if (pos < codigo.size() && (codigo[pos] == '+' || codigo[pos] =='-'))
        {
            lex += codigo[pos++];
            column++;
        }
        while (pos < codigo.size() && (isdigit(codigo[pos]) || codigo[pos] == '_'))
        {
            if (codigo[pos] != '_')
            {
                lex += codigo[pos];
            }
            pos++;
            column++;
        }
    }
    
    //Tipo de numero
    Tipo tipo = decimal ? (cientifico ? Tipo::NUMERO_CIENTIFICO : Tipo::NUMERO_DECIMAL) : Tipo::NUMERO_ENTERO;

    tokens.push_back({tipo, lex, iniLine, iniCol});
}

void LexerRust::cadena(vector<Token>& tokens){
    int iniLine = line;
    int iniCol = column;
    string lex = "\"";
    pos++;
    column++;
    bool closed = false;
    bool esc = false;

    while (pos < codigo.size())
    {
        char actual = codigo[pos];

        if (esc)
        {
            lex += actual;
            pos++;
            column++;
            esc = false;
            continue;
        }

        if (actual == '\\')
        {
            esc = true;
            lex += actual;
            pos++;
            column++;
            continue;
        }

        if (actual == '"')
        {
            lex += actual;
            pos++;
            column++;
            closed = true;
            break;
        }

        if (actual == '\n')
        {
            line++;
            column = 1;
        }
        else{
            column++;
        }
        
        lex += actual;
        pos++;   
    }

    if (!closed)
    {
        cerr << "Error: cadena no cerrada en línea " << iniLine<< ", columna " << iniCol << endl;
    }
    
    tokens.push_back({Tipo::CADENA, lex, iniLine, iniCol});
}

void LexerRust::caracter(vector<Token>& tokens){
    int iniLine = line;
    int iniCol = column;
    string lex = "'";
    pos++;
    column++;
    bool closed = false;
    bool esc = false;

    while (pos < codigo.size())
    {
        char actual = codigo[pos];

        if (esc)
        {
            lex += actual;
            pos++;
            column++;
            esc = false;
            continue;
        }

        if (actual == '\\')
        {
            esc = true;
            lex += actual;
            pos++;
            column++;
            continue;
        }
        
        if (actual == '\'')
        {
            lex += actual;
            pos++;
            column++;
            closed = true;
            break;
        }
        
        column++;
        lex += actual;
        pos++;
    }

    if (!closed)
    {
        cerr << "Error: carácter no cerrado en línea " << iniLine << ", columna " << iniCol << endl;
    }
    
    //Caracter Valido
    if (lex.length() > 4 || (lex.length() == 3 && lex[1] != '\\'))
    {
        cerr << "Error: carácter inválido en línea " << iniLine << ", columna " << iniCol << ": " << lex << endl;
    }
    
    tokens.push_back({Tipo::CARACTER, lex, iniLine, iniCol});
}

void LexerRust::simbolos(vector<Token>& tokens){
    int iniLine = line;
    int iniCol = column;

    //Verificar compuestos
    if (pos +1 < codigo.size())
    {
        string doble = string(1, codigo[pos]) + codigo[pos+1];

        if (simbolosCompuestos.count(doble))
        {
            tokens.push_back({Tipo::OPERADOR, doble, iniLine, iniCol});
            pos += 2;
            column += 2;
            return;
        }        
    }
    
    //Individuales
    char c = codigo[pos];

    switch (c)
    {
    case ';':
    case ',':
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
        tokens.push_back({Tipo::PUNTUACION, string(1,c), iniLine, iniCol});
        break;

    case '+':
    case '-':
    case '*':
    case '/':
    case '=':    
    case '%':
    case '<':
    case '>':
    case '&':
    case '|':    
    case '!':
    case '^':
    case '.':
    case ':':
    case '@':
    case '$':
    case '?':
    case '~':
    case '#':
        tokens.push_back({Tipo::OPERADOR, string(1,c), iniLine, iniCol});
        break;

    default:
        cerr << "Error: símbolo inválido en línea " << line << ", columna " << column << ": " << c << endl;
        break;
    }
    pos++;
    column++;
}

void LexerRust::printTokens(vector<Token>& tokens){
    map<Tipo, string> tipoString = {
        {Tipo::CADENA, "CADENA"},
        {Tipo::CARACTER, "CARACTER"},
        {Tipo::COMENTARIO, "COMENTARIO"},
        {Tipo::ERROR, "ERROR"},
        {Tipo::FIN, "FIN"},
        {Tipo::IDENTIFICADOR, "IDENTIFICADOR"},
        {Tipo::NUMERO_CIENTIFICO, "NUMERO CIENTIFICO"},
        {Tipo::NUMERO_DECIMAL, "NUUMERO DECIMAL"},
        {Tipo::NUMERO_ENTERO, "NUMERO ENTERO"},
        {Tipo::OPERADOR, "OPERADOR"},
        {Tipo::PALABRA_CLAVE, "PALABRA CLAVE"},
        {Tipo::PUNTUACION, "PUNTUACION"},
        {Tipo::SIMBOLO, "SIMBOLO"}
    };
    for (const auto& token : tokens)
    {
        cout << "[L:" << token.line << ",C:" << token.column << "] "<< tipoString[token.tipo] << ": " << token.valor << endl;
    }
    
}

LexerRust::~LexerRust() {
    
}