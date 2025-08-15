#include "LexerRust.h"
#include <cctype>
#include <iostream>

using namespace std;

LexerRust::LexerRust(const string& codigo) 
    : codigo(codigo), pos(0), line(1), column(1) 
{
    // 🔹 Palabras clave
    palabrasReservadas = {
        "as", "async", "await", "break", "const", "continue", "crate", "dyn", "extern", "false", "fn", "impl", "in", "let", "mod", "move", "mut", "println", "pub", "ref", "return", "self", "Self", "static", "super", "trait", "true", "type", "union", "unsafe", "use", "where"};

    // 🔹 Tipos primitivos
    tiposPrimitivos = {
        "i8", "i16", "i32", "i64", "i128", "isize",  "u8", "u16", "u32", "u64", "u128", "usize", "f32", "f64", "bool", "char", "()", "str", "struct", "enum"
    };

    // 🔹 Tipos estándar
    tiposEstandar = {
        "String", "Vec", "HashMap", "Option", "Result", "Box", "Rc", "Arc"
    };

    // 🔹 Palabras de control de flujo
    controlFlujo = {
        "if", "else", "while", "for", "loop", "match"
    };

    // 🔹 Símbolos compuestos
    simbolosCompuestos = {
        "==", "!=", "<=", ">=", "&&", "||", "+=", "-=", "*=", "/=", "%=", "->", "::", "..", "..=", "=>", "|=", "&=", "^=", "<<", ">>", "<<=", ">>="
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

        if (c == '/' && pos + 1 < codigo.size() && codigo[pos + 1] == '/') {
            comentarioLinea(tokens); continue;
        }

        if (c == '/' && pos + 1 < codigo.size() && codigo[pos + 1] == '*') {
            comentarioBloque(tokens); continue;
        }

        if (isalpha(c) || c == '_') {
            identificador(tokens); continue;
        }

        if (isdigit(c)) {
            numero(tokens); continue;
        }

        if (c == '"') {
            cadena(tokens); continue;
        }

        if (c == '\'') {
            caracter(tokens); continue;
        }

        simbolos(tokens);
    }

    tokens.push_back({Tipo::FIN, "", line, column});
    return tokens;
}

// ---------------------- COMENTARIOS ----------------------
void LexerRust::comentarioLinea(vector<Token>& tokens) {
    int iniLine = line, iniCol = column;
    string lex = "//";
    pos += 2; column += 2;

    while (pos < codigo.size() && codigo[pos] != '\n') {
        lex += codigo[pos++]; column++;
    }

    //tokens.push_back({Tipo::COMENTARIO, lex, iniLine, iniCol});
}

void LexerRust::comentarioBloque(vector<Token>& tokens) {
    int iniLine = line, iniCol = column;
    string lex = "/*";
    pos += 2; column += 2;
    bool closed = false;

    while (pos < codigo.size()) {
        if (codigo[pos] == '*' && pos + 1 < codigo.size() && codigo[pos + 1] == '/') {
            lex += "*/"; pos += 2; column += 2; closed = true; break;
        }
        if (codigo[pos] == '\n') { line++; column = 1; }
        else column++;
        lex += codigo[pos++];
    }

    if (!closed) cerr << "Error: comentario no cerrado en L" << iniLine << ", C" << iniCol << endl;

    //tokens.push_back({Tipo::COMENTARIO, lex, iniLine, iniCol});
}

// ---------------------- IDENTIFICADOR ----------------------
void LexerRust::identificador(vector<Token>& tokens) {
    int iniLine = line, iniCol = column;
    string lex;

    while (pos < codigo.size() && (isalnum(codigo[pos]) || codigo[pos] == '_')) {
        lex += codigo[pos++]; column++;
    }

    Tipo tipo;
    if (palabrasReservadas.count(lex)) tipo = Tipo::PALABRA_RESERVADA;
else if (controlFlujo.count(lex)) tipo = Tipo::CONTROL_FLUJO;
else if (tiposPrimitivos.count(lex)) tipo = Tipo::TIPO_PRIMITIVO;
else if (tiposEstandar.count(lex)) tipo = Tipo::TIPO_ESTANDAR;
else tipo = Tipo::IDENTIFICADOR;


    tokens.push_back({tipo, lex, iniLine, iniCol});
}

// ---------------------- NÚMEROS ----------------------
void LexerRust::numero(vector<Token>& tokens) {
    int iniLine = line, iniCol = column;
    string lex;
    bool decimal = false, cientifico = false;

    while (pos < codigo.size() && (isdigit(codigo[pos]) || codigo[pos] == '_')) {
        if (codigo[pos] != '_') lex += codigo[pos];
        pos++; column++;
    }

    if (pos < codigo.size() && codigo[pos] == '.') {
        decimal = true; lex += codigo[pos++]; column++;
        while (pos < codigo.size() && (isdigit(codigo[pos]) || codigo[pos] == '_')) {
            if (codigo[pos] != '_') lex += codigo[pos];
            pos++; column++;
        }
    }

    if (pos < codigo.size() && (codigo[pos] == 'e' || codigo[pos] == 'E')) {
        cientifico = true; lex += codigo[pos++]; column++;
        if (pos < codigo.size() && (codigo[pos] == '+' || codigo[pos] == '-')) {
            lex += codigo[pos++]; column++;
        }
        while (pos < codigo.size() && isdigit(codigo[pos])) { lex += codigo[pos++]; column++; }
    }

    Tipo tipo = decimal ? (cientifico ? Tipo::NUMERO_CIENTIFICO : Tipo::NUMERO_DECIMAL) : Tipo::NUMERO_ENTERO;
    tokens.push_back({tipo, lex, iniLine, iniCol});
}

// ---------------------- CADENA ----------------------
void LexerRust::cadena(vector<Token>& tokens) {
    int iniLine = line, iniCol = column;
    string lex = "\""; pos++; column++;
    bool closed = false, esc = false;

    while (pos < codigo.size()) {
        char actual = codigo[pos];

        if (esc) { lex += actual; pos++; column++; esc = false; continue; }
        if (actual == '\\') { lex += actual; pos++; column++; esc = true; continue; }
        if (actual == '"') { lex += actual; pos++; column++; closed = true; break; }

        if (actual == '\n') { line++; column = 1; }
        else column++;

        lex += actual; pos++;
    }

    if (!closed) cerr << "Error: cadena no cerrada en L" << iniLine << ", C" << iniCol << endl;

    tokens.push_back({Tipo::CADENA, lex, iniLine, iniCol});
}

// ---------------------- CARACTER ----------------------
void LexerRust::caracter(vector<Token>& tokens) {
    int iniLine = line, iniCol = column;
    string lex = "'"; pos++; column++;
    bool closed = false, esc = false;

    while (pos < codigo.size()) {
        char actual = codigo[pos];

        if (esc) { lex += actual; pos++; column++; esc = false; continue; }
        if (actual == '\\') { lex += actual; pos++; column++; esc = true; continue; }
        if (actual == '\'') { lex += actual; pos++; column++; closed = true; break; }

        column++; lex += actual; pos++;
    }

    if (!closed) cerr << "Error: carácter no cerrado en L" << iniLine << ", C" << iniCol << endl;

    tokens.push_back({Tipo::CARACTER, lex, iniLine, iniCol});
}

// ---------------------- SIMBOLOS ----------------------
void LexerRust::simbolos(vector<Token>& tokens) {
    int iniLine = line, iniCol = column;

    // Verificar compuestos
    if (pos + 1 < codigo.size()) {
        string doble = string(1, codigo[pos]) + codigo[pos + 1];
        if (simbolosCompuestos.count(doble)) {
            tokens.push_back({Tipo::OPERADOR, doble, iniLine, iniCol});
            pos += 2; column += 2; return;
        }
    }

    // Individuales
    char c = codigo[pos];
    switch (c) {
        case ';': case ',': case '(': case ')': case '{': case '}': case '[': case ']':
            tokens.push_back({Tipo::PUNTUACION, string(1,c), iniLine, iniCol}); break;
        case '+': case '-': case '*': case '/': case '=': case '%': case '<': case '>':
        case '&': case '|': case '!': case '^': case '.': case ':': case '@': case '$':
        case '?': case '~': case '#':
            tokens.push_back({Tipo::OPERADOR, string(1,c), iniLine, iniCol}); break;
        default:
            cerr << "Error: símbolo inválido en L" << line << ", C" << column << ": " << c << endl; break;
    }
    pos++; column++;
}

// ---------------------- PRINT ----------------------
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
