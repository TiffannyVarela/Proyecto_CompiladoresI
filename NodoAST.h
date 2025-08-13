#ifndef NODOAST_H
#define NODOAST_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

class NodoAST
{
private:
    string tipe;
    string value;
    vector<NodoAST*> childs;
public:
    NodoAST(const string& tipe, const string& value);
    void addChild(NodoAST* child);
    void print(int level=0);
    bool haveChilds() const {return !childs.empty();}
    ~NodoAST();
};

#endif