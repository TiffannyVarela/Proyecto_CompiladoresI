#include "NodoAST.h"
#include <iostream>

NodoAST::NodoAST(const string& tipe, const string& value){
    this->tipe=tipe;
    this->value=value;
}

void NodoAST::addChild(NodoAST* child){
    childs.push_back(child);
}

void NodoAST::print(int level){
    for (int i = 0; i < level; i++)
    {
        cout<<"     ";       
    }
    cout<<tipe<<": "<<value<<endl; 

    for (auto child : childs)
    {
        child->print(level+1);
    }
}

NodoAST::~NodoAST(){
    
}