#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include "TwoTriangleGraph.hh"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

int main() {
    string str;
    while (cin >> str) {
        TwoTriangleGraph(TwoTriangleGraphCode(str)).write(cout);
    }

    
}