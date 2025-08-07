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

        if (!tokenValido(token) && !(token.front() == '"' && token.back() == '"'))
        {
            cout << "Invalid token at position " << i << ": " << token << endl;
        }
    }
}

bool LexerRust::tokenValido(const string& token) const {
    // Define valid tokens using regex patterns
    static const regex identifierPattern("^[a-zA-Z_][a-zA-Z0-9_]*$");
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
    vector<string>().swap(tokens); // Clear and deallocate memory
}