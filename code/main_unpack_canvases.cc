#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include "Canvas.hh"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

int main() {
    string str;
    while (cin >> str) {
        Canvas(CanvasCode(str)).write(cout);
    }

    
}