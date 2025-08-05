#include "lectura.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    lectura fileReader("main.rs");
    vector<char> fileContent = fileReader.readFile();

    cout << "File content : " << endl;
    for (char c : fileContent) {
        cout << c;
    }
    return 0;
}