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
            continue; // Skip tokens inside multi-line comments
        }

        if (token=="//") {
            // Skip the rest of the line for single-line comments
            while (i < tokens.size() && tokens[i] != "\n") {
                i++;
            }
            continue;
        }

        if (token == "\"") {
            inCadenaLocal = !inCadenaLocal; // Toggle string state
            continue;
        }
        if (inCadenaLocal) {
            continue; // Skip tokens inside strings
        }
        if (!tokenValido(token))
        {
            cout << "Invalid token at position " << i << ": " << token << endl;
        }
    }
}

bool LexerRust::tokenValido(const string& token) const {
    // Define valid tokens using regex patterns
    static const regex identifierPattern("^[a-zA-Z_][a-zA-Z0-9_]*$");
    static const regex numberPattern("^\\d+(\\.\\d+)?$");
    static const regex operatorPattern("^(\\+|\\-|\\*|\\/|=|==|!=|<|>|<=|>=|&&|\\|\\||!|;|,|\\.|::|->)$");
    static const regex punctuationPattern("^(\\(|\\)|\\{|\\}|\\[|\\])$");
    static const vector<string> keywords = {
        "fn", "let", "mut", "if", "else", "match", "struct", "impl", "enum", "return", "while", "for", "in", "break", "continue", "use", "mod", "pub", "crate", "super", "self"
    };

    if (regex_match(token, identifierPattern)) {
        // Palabra clave o identificador valido?
        if (find(keywords.begin(), keywords.end(), token) != keywords.end()) {
            return true; // Es una palabra clave valida
        }
        return true; // Es un identificador valido
    }
    if (regex_match(token, numberPattern) || regex_match(token, operatorPattern) || regex_match(token, punctuationPattern)) {
        return true;
    }
    return false;
}

LexerRust::~LexerRust() {
    tokens.clear();
    vector<string>().swap(tokens); // Clear and deallocate memory
}