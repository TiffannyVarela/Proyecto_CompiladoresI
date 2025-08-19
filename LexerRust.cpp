#include "LexerRust.h"
#include <cctype>
#include <iostream>
#include <regex>

using namespace std;

LexerRust::LexerRust(const string& codigo) 
    : codigo(codigo), pos(0), line(1), column(1) 
{
        Reglas = {
        // Palabras Reservadas
        {regex("^(as|async|await|break|const|continue|crate|dyn|extern|false|fn|impl|in|let|mod|move|mut|println|pub|ref|return|self|Self|static|super|trait|true|type|union|unsafe|use|where)\\b"), Tipo::PALABRA_RESERVADA},

        //Control de flujo
        {regex("^(if|else|while|for|loop|match)\\b"), Tipo::CONTROL_FLUJO},

        // Tipos primitivos
        {regex("^(i8|i16|i32|i64|i128|isize|u8|u16|u32|u64|u128|usize|f32|f64|bool|char|\\(\\)|str|struct|enum)\\b"), Tipo::TIPO_PRIMITIVO},

        //Tipos Estandar
        {regex("^(String|Vec|HashMap|Option|Result|Box|Rc|Arc)\\b"), Tipo::TIPO_ESTANDAR},

        //Comentarios
        {regex("^//.*"), Tipo::COMENTARIO},
        {regex("^/\\*[^*]*\\*+([^/*][^*]*\\*+)*/"), Tipo::COMENTARIO},

        //Cadenas
        {regex("^\"([^\"\\\\]|\\\\.)*\""), Tipo::CADENA},

        //Caracteres
        {regex("^'([^'\\\\]|\\\\.)'"), Tipo::CARACTER},

        //Numeros
        {regex("^[0-9]+(\\.[0-9]+)?[eE][+-]?[0-9]+"), Tipo::NUMERO_CIENTIFICO},
        {regex("^[0-9]+\\.[0-9]+"), Tipo::NUMERO_DECIMAL},
        {regex("^[0-9]+"), Tipo::NUMERO_ENTERO},

        //Operadores
        {regex("^(==|!=|<=|>=|&&|\\|\\||\\+=|-=|\\*=|/=|%=|->|::|\\.\\.|\\.\\.=|=>|\\|=|&=|\\^=|<<|>>|<<=|>>=)"), Tipo::OPERADOR},
        {regex("^[-+*/=<>!&|^.:@?$~#]"), Tipo::OPERADOR},

        //Puntuación
        {regex("^[,;:(){}\\[\\]]"), Tipo::PUNTUACION},

        //Identificadores
        {regex("^[a-zA-Z_][a-zA-Z0-9_]*"), Tipo::IDENTIFICADOR}
    };
}

vector<Token> LexerRust::analiza() {
    vector<Token> tokens;
    while (pos < codigo.size()) {
        char c = codigo[pos];
        if (isspace(c)) {
            if (c == '\n') { line++; column = 1; }
            else column++;
            pos++;
            continue;
        }

        string subCodigo = codigo.substr(pos);
        bool matched = false;

        for (const auto& regla : Reglas) {
            smatch match;
            if (regex_search(subCodigo, match, regla.patron)) {
                string valor = match.str(0);
                
                if (regla.tipo != Tipo::COMENTARIO) {
                    tokens.push_back({regla.tipo, valor, line, column});
                }
                for (char ch : valor) {
                    if (ch == '\n') {
                        line++;
                        column = 1;
                    } 
                    else {
                        column++;
                    }
                }
                pos += valor.size();
                matched = true;
                break;
            }
        }
        if (!matched) {
            cerr << "Error: símbolo inválido en L" << line << ", C" << column << ": " <<codigo[pos] << endl;
            pos++; column++;
            string valor(1, c);
            tokens.push_back({Tipo::ERROR, valor, line, column});
            pos++;
            column++;
        }
    }

    tokens.push_back({Tipo::FIN, "", line, column});
    return tokens;
}

void LexerRust::printTokens(vector<Token>& tokens) {
    map<Tipo, string> tipoString = {
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

    for (const auto& token : tokens) {
        cout << "[L:" << token.line << ",C:" << token.column << "] "
             << tipoString[token.tipo] << ": " << token.valor << endl;
    }
}

LexerRust::~LexerRust() {}
