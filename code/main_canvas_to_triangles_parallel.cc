#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<chrono>
#include "Canvas.hh"
#include "TwoTriangleGraph.hh"
#include "Parallelism.hh"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

const int BLOCK_SIZE = 100;

void process_list(vector<CanvasCode> v) {
    for (auto cc : v) {
        Canvas c = Canvas(cc);
        vector<TwoTriangleGraph> ttg = TwoTriangleGraph::generate_from_canvas(c);
        for (auto g : ttg) {
            if (g.test_criticality()) {
                Parallelism::two_triangle_list_mutex.lock();
                cout << g.compute_code().to_string() << endl;
                Parallelism::two_triangle_list_mutex.unlock();
            }
        }
    }
}

int main() {
    
    vector<CanvasCode> canvas_list;

    string str;
	while (cin >> str) {
        canvas_list.push_back(CanvasCode(str));
        if ((int)canvas_list.size() == BLOCK_SIZE) {
            Parallelism::spawn_thread([canvas_list] () {
                process_list(canvas_list);
            });
            canvas_list.clear();
            
        }  
    }

    Parallelism::spawn_thread([canvas_list] () {
                process_list(canvas_list);
            });

    Parallelism::wait_for_threads();
}