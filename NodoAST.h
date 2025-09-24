#ifndef NODOAST_H
#define NODOAST_H

#include <string>
#include <vector>
#include <memory> //Para usar unique_ptr (punteros inteligentes)

using namespace std;

class NodoAST
{
private:
    string tipe;
    string value;
    vector<unique_ptr<NodoAST>> childs; //Lista de hijos usando punteros unicos
public:
    NodoAST(const string& tipe, const string& value);
    void addChild(unique_ptr<NodoAST> child);
    void print(ostream& out, int level = 0) const;
    bool haveChilds() const {return !childs.empty();}
    ~NodoAST();
};

#endif