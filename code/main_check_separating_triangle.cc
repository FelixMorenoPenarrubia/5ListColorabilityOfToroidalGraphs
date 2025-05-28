#include<iostream>
#include<string>
#include "TwoTriangleGraph.hh"

using std::cout;
using std::cin;
using std::endl;
using std::string;


int main() {
    string str;
	while (cin >> str) {
        if (!TwoTriangleGraph(TwoTriangleGraphCode(str)).has_separating_triangle()) {
            cout << str << endl;
        }
    }
}