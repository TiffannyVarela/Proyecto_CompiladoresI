#include "LexerRust.h"
#include <iostream>
#include <regex>
#include <algorithm>

using namespace std;

LexerRust::LexerRust(const vector<string>& tokens) {
    this->tokens = tokens;
    this->comentarioMultiLine = false;
    this->inCadena = false;
}

void LexerRust::verifyTokens() const {
    cout << "Verifying tokens..." << endl;
    bool comentarioMultiLineLocal = false;
    bool comentarioLinea = false;
    bool inCadenaLocal = false;

    for(size_t i = 0; i < tokens.size(); i++) {
        const string& token = tokens[i];

        if (token == "/*") {
            comentarioMultiLineLocal = true;
            continue;
        }

        if (token == "*/") {
            comentarioMultiLineLocal = false;
            continue;
        }

        if (comentarioMultiLineLocal) {
            continue; 
        }

        if (token=="//") {
            comentarioLinea=true;
            continue;
        }

        if (comentarioLinea && token == "\n"){
            comentarioLinea = false;
            continue;
        }

        if (comentarioLinea){
            continue;
        }

        if (token == "\"") {
            inCadenaLocal = !inCadenaLocal; 
            continue;
        }
        if (inCadenaLocal) {
            continue; 
        }

        if (!tokenValido(token) && !(token.front() == '"' && token.back() == '"'))
        {
            cout << "Invalid token at position " << i << ": " << token << endl;
        }
    }
}

bool LexerRust::tokenValido(const string& token) const {
    
    static const regex identifierPattern("^[a-zA-Z_áéíóúÁÉÍÓÚñÑ][a-zA-Z0-9_áéíóúÁÉÍÓÚñÑ]*$");
    static const regex numberPattern("^-?\\d+(_?\\d+)*(\\.\\d+(_?\\d+)*)?([eE][+-]?\\d+(_?\\d+)*)?$");
    static const regex operatorPattern("^(\\+|\\-|\\*|\\/|%|=|==|!=|<|>|<=|>=|&&|\\|\\||!|\\^|&|\\||\\?|\\.{1,3}|:|::|->|=>|\\+=|-=|\\*=|/=|%=|&=|\\|=|\\^=|<<|>>|<<=|>>=|\\$|@|_)$");
    static const regex punctuationPattern("^(\\(|\\)|\\{|\\}|\\[|\\]|,|;|`)$");
    static const vector<string> keywords = {
        "as", "async", "await", "break", "const", "continue", "crate", "dyn", "else", 
        "enum", "extern", "false", "fn", "for", "if", "impl", "in", "let", "loop", 
        "match", "mod", "move", "mut", "pub", "ref", "return", "self", "Self", 
        "static", "struct", "super", "trait", "true", "type", "union", "unsafe", 
        "use", "where", "while"
    };

    if ((token.front() == '"' && token.back() == '"') || (token.front() == '\'' && token.back() == '\''))
    {
        return false;
    }

    if (regex_match(token, identifierPattern)){
        return find(keywords.begin(), keywords.end(), token) != keywords.end() || true;
    }
    if (regex_match(token, numberPattern) || regex_match(token, operatorPattern) || regex_match(token, punctuationPattern)){
        return true;
    }
    return false;
}

LexerRust::~LexerRust() {
    tokens.clear();
    vector<string>().swap(tokens); 
}