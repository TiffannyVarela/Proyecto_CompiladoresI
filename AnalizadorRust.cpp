#include "AnalizadorRust.h"
#include <iostream>
#include <algorithm>

using namespace std;

AnalizadorRust::AnalizadorRust(const vector<string>& tokens) {
    this->tokens = tokens;
}

void AnalizadorRust::contFunciones() const {
    int func=0;
    for (const auto& token : tokens) {
        if (token == "fn") {
            func++;
        }
    }
    cout << "Total functions (fn): " << func << endl;
}

void AnalizadorRust::contVariables() const {
    int var = 0;
    for (const auto& token : tokens) {
        if (token == "let" || token == "mut") {
            var++;
        }
    }
    cout << "Total variables (let/mut): " << var << endl;
}

void AnalizadorRust::listPalabrasReservadas() const {
    vector<string> palabrasReservadas = {"fn", "let", "mut", "if", "else", "match", "struct", "impl", "enum"};
    cout << "Reserved words in Rust: ";
    for (const auto& palabra : palabrasReservadas) {
        cout << palabra << " ";
    }
    cout << endl;
}

void AnalizadorRust::contFuncionesDefinidas() const {
    int funcDef = 0;
    for (const auto& token : tokens) {
        if (token == "fn") {
            funcDef++;
        }
    }
    cout << "Total defined functions: " << funcDef << endl;
}

void AnalizadorRust::contEstructuras() const {
    int structs = 0;
    for (const auto& token : tokens) {
        if (token == "struct") {
            structs++;
        }
    }
    cout << "Total structures (struct): " << structs << endl;
}

AnalizadorRust::~AnalizadorRust()
{
    tokens.clear();
    vector<string>().swap(tokens); // Clear and deallocate memory
}