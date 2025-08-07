#ifndef PARSERUST_H
#define PARSERUST_H
#include <string>
#include <vector>

using namespace std;

class ParserRust {
private:
    vector<string> tokens;
    
public:
    ParserRust(const vector<string>& tokens);
    void verifParentesis() const;
    void verifFunciones() const;
    void verifVariables() const;
    void verifEstructuras() const;
    ~ParserRust();
};

#endif // PARSERUST_H