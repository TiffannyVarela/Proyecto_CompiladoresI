#ifndef LEXERRUST_H
#define LEXERRUST_H

#include <string>
#include <vector>

using namespace std;

class LexerRust {
public:
    LexerRust(const vector<string>& tokens);
    void verifyTokens() const;
    ~LexerRust();

private:
    vector<string> tokens;
    bool comentarioMultiLine;
    bool inCadena;
    bool tokenValido(const string& token) const;
};
#endif // LEXERRUST_H