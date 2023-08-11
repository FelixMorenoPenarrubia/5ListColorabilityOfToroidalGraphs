#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<set>
#include "TwoTriangleGraph.hh"
#include "GlobalSettings.hh"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

int main() {
    vector<TwoTriangleGraph> v;
    std::set<TwoTriangleGraphCode> s;
    string str;
    while (cin >> str) {
        auto g = TwoTriangleGraph(TwoTriangleGraphCode(str));
        g.set_first_triangle_as_outer_face();
        g.set_second_triangle_as_inner_face();
        v.push_back(g);
        s.insert(g.compute_code());
        std::swap(g.triangles[0], g.triangles[1]);
        g.set_first_triangle_as_outer_face();
        g.set_second_triangle_as_inner_face();
        v.push_back(g);
        s.insert(g.compute_code());
    }

    int cnt = -1;

    for (int idx1=0; idx1 < (int)v.size(); ++idx1) {
        auto g1 = v[idx1];
        for (auto g2 : v) {
            vector<TwoTriangleGraph> fus = TwoTriangleGraph::fuse_triangles(g1, g2);
            for (auto g3 : fus) {
                if (s.find(g3.compute_code()) == s.end()) {
                    if (g3.test_criticality()) {
                        cout << "---" << endl;
                        g1.write(cout);
                        g2.write(cout);
                        cout << "-" << endl;
                        g3.write(cout);
                        cout << g3.compute_code().to_string() << endl;
                        TwoTriangleGraph(g3.compute_code()).write(cout);
                        cout << "---" << endl;
                    }
                }
            }
        }
        if (((20*idx1)/(int)v.size()) > cnt) {
            cnt++;
            cerr << 5*cnt << "% " << std::flush;
            if (cnt > 0) return 0;
        }
    }
    cerr << endl;
}