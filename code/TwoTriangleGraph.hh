#ifndef TWOTRIANGLEGRAPH_HH_
#define TWOTRIANGLEGRAPH_HH_

#include "PrecoloredGraph.hh"
#include "Canvas.hh"

#include<iostream>
#include<vector>
#include<map>
#include<algorithm>
#include<string>



struct TwoTriangleGraphCode {
    std::vector<int> code;

    TwoTriangleGraphCode();

    TwoTriangleGraphCode(const std::string& s);

    void push(int x);
    
    void push_f(int triangle);

    void push_b();

    int size() const;

    std::string to_string() const;

    int operator[](int i) const;

    bool operator<(const TwoTriangleGraphCode& b) const;


};

struct TwoTriangleGraph : public PrecoloredGraph {


    int path_length;

    std::vector<std::vector<int>> triangles;

    Canvas original_canvas;

    void init_inherited_values();

    TwoTriangleGraph();

    TwoTriangleGraph(std::vector<std::vector<int>> _al, std::vector<std::vector<int>> _triangles);

    TwoTriangleGraph(const Canvas& g, int idx);

    TwoTriangleGraph(const TwoTriangleGraphCode& code);

    void set_list_sizes();

    static std::vector<TwoTriangleGraph> generate_from_canvas(const Canvas& g);

    //Returns all possible combinations of fusing the triangular faces of g1 and g2
    static std::vector<TwoTriangleGraph> fuse_triangles(TwoTriangleGraph g1, TwoTriangleGraph g2);

    //Fuses the second triangle of g1 with the first triangle of g2. TODO: may not respect orientation
    static TwoTriangleGraph fuse_triangles_fixed(const TwoTriangleGraph& g1, const TwoTriangleGraph& g2);

    //Fuses all subgraphs of a tetrahedral band with g
    static std::vector<TwoTriangleGraph> add_tetrahedral_band(const TwoTriangleGraph& g);

    //Fuses g1 with every connected subgraph of g2. PRE: g2 must be small
    static std::vector<TwoTriangleGraph> fuse_triangles_subgraph_connected(const TwoTriangleGraph& g1, const TwoTriangleGraph& g2);

    //Fuses all subgraphs of an octahedral band with g
    static std::vector<TwoTriangleGraph> add_octahedral_band(const TwoTriangleGraph& g);

    //Fuses all subgraphs of a hexadecahedral band with g
    static std::vector<TwoTriangleGraph> add_hexadecahedral_band(const TwoTriangleGraph& g);

    static TwoTriangleGraph read(std::istream& is);

    void write(std::ostream& os) const;

    void write_prolog(std::ostream& os) const;

    void dfs_code(int u, int idx, int& c, int rt, std::vector<int>& an, TwoTriangleGraphCode& code) const;

    TwoTriangleGraphCode compute_code_edge(int u, int v) const;

    TwoTriangleGraphCode compute_code() const;

    int distance_between_triangles() const;

    bool test_criticality() const;

    bool in_precolored_triangle(int u) const;

    bool is_separating_triangle(std::vector<int> tri) const;

    bool has_separating_triangle() const;

    void set_first_triangle_as_outer_face();

    void set_second_triangle_as_inner_face();

    bool triangles_have_same_orientation() const;

    //Generates toroidal 5-list graphs by identifying the two triangles in all possible ways
    std::vector<ListGraph> identify_triangles() const;

};

#endif