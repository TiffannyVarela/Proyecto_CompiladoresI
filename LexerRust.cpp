#include "LexerRust.h"
#include <cctype>
#include <set>
#include <iostream>
//#include <regex>
//#include <algorithm>

using namespace std;

LexerRust::LexerRust(const string& codigo) : codigo (codigo), pos(0), line(1), column(1) {
    palabrasClave = {"as", "async", "await", "break", "const", "continue", "crate", "dyn", "else", 
        "enum", "extern", "false", "fn", "for", "if", "impl", "in", "let", "loop", 
        "match", "mod", "move", "mut", "pub", "ref", "return", "self", "Self", 
        "static", "struct", "super", "trait", "true", "type", "union", "unsafe", 
        "use", "where", "while"};
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
        
//Palabras clave
        if(isalpha(c) || c == '_'){
            int inicioCol = column;
            string lexema;
            while (pos < codigo.size() && (isalnum(codigo[pos]) || codigo[pos] == '_'))
            {
                lexema += codigo[pos++];
                column++;
            }
            Tipo tipo = palabrasClave.count(lexema) ? Tipo::PALABRA_CLAVE : Tipo::IDENTIFICADOR;
            tokens.push_back({tipo, lexema, line, inicioCol});
            continue;            
        } 

//Numeros
        if (isdigit(c))
        {
            int inicioCol = column;
            string num;
            while (pos < codigo.size() && isdigit(codigo[pos]))
            {
                num += codigo[pos++];
                column++;
            }
            tokens.push_back({Tipo::NUMERO, num, line, inicioCol});
            continue;
        }

//Cadena de caracteres
        if (c== '"')
        {
            int inicioCol = column;
            string cadena;
            cadena += codigo[pos++]; //abretura de las comillas
            column++;
            bool closed = false;
            while (pos < codigo.size())
            {
                char actual = codigo[pos];

                if (actual == '\\' && pos+1 <codigo.size())
                {
                    cadena += codigo[pos++];
                    cadena += codigo[pos++];
                    column += 2;
                    continue;
                }

                //Fin cadena
                if (actual == '"')
                {
                    cadena += codigo[pos++];
                    column++;
                    closed = true;
                    break;
                }

                if (actual == '\n')
                {
                    line++;
                    column = 1;
                }
                else {
                    column++;
                }
                cadena += codigo[pos++];              
            }
            if (!closed)
            {
                cerr << "Error: cadena no cerrada en línea " << line << ", columna " << inicioCol << endl;
            }
            tokens.push_back({Tipo::CADENA, cadena, line, inicioCol});
            continue;
        }

//Comillas simples
        if (c == '\'')
        {
            int inicioCol = column;
            string caracter;
            caracter += codigo[pos++];
            column++;
            bool closed = false;

            while (pos < codigo.size())
            {
                char actual = codigo[pos];

                if (actual == '\\' && pos+1 < codigo.size())
                {
                    caracter += codigo[pos++];
                    caracter += codigo[pos++];
                    column +=2;
                }
                else{
                    caracter += codigo[pos++];
                    column++;
                }

                if (codigo[pos] == '\'')
                {
                    caracter += codigo[pos++];
                    column++;
                    closed=true;
                    break;
                }               
            }

            if (!closed)
            {
                cerr << "Error: carácter no cerrado en línea " << line << ", columna " << inicioCol << endl;
            }
            
            tokens.push_back({Tipo::CARACTER, caracter, line, inicioCol});
            continue;
        }
        
        static const set<string> validos ={";", ",", "(", ")", "{", "}", "[", "]", "+", "-", "*", "/", "=", "%", "<", ">", "&", "|", "!", "^", ".", ":"};
        
        int inicioCol = column;
        string symbol(1, c);
        pos++;
        column++;

        if (validos.count(symbol))
        {
            tokens.push_back({Tipo::SIMBOLO, symbol, line, inicioCol});
        }
        else{
            cerr << "Token inválido en línea " << line << ", columna " << inicioCol << ": " << symbol << endl;
        }
        
    }

    tokens.push_back({Tipo::FIN, "", line, column});
    return tokens;
}

LexerRust::~LexerRust() {
    
}