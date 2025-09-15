#include "NodoAST.h"
#include <iostream>

NodoAST::NodoAST(const string& tipe, const string& value){
    this->tipe=tipe;
    this->value=value;
}

void NodoAST::addChild(unique_ptr<NodoAST> child){
    childs.push_back(move(child));
}

void NodoAST::print(ostream& out, int level) const{
    for (int i = 0; i < level; i++)
        out << "  ";
    out << tipe << (value.empty() ? "" : ": " + value) << "\n";
    
    for (const auto& child : childs)
    {
        if(child)
        {
            child->print(out, level + 1);
        }
    }
}

NodoAST::~NodoAST(){
    
}