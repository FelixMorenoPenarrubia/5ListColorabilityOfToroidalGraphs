#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<chrono>
#include "Canvas.hh"
#include "TwoTriangleGraph.hh"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

void process_canvas(CanvasCode cc) {
    
    Canvas c = Canvas(cc);
    vector<TwoTriangleGraph> ttg = TwoTriangleGraph::generate_from_canvas(c);
    for (auto g : ttg) {
        if (g.test_criticality()) {
            cout << g.compute_code().to_string() << endl;
        }
    }
    
}

int main() {

   string str;
	while (cin >> str) {
        process_canvas(CanvasCode(str));
    }

}