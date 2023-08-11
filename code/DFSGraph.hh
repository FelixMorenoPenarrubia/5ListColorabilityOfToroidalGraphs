#ifndef DFSGRAPH_HH_
#define DFSGRAPH_HH_

#include "Graph.hh"

#include<iostream>
#include<vector>
#include<map>
#include<algorithm>
#include<utility>



struct DFSGraph : public Graph {
    int n;
	
	std::vector<std::vector<int> > al;

	std::vector<std::vector<int> > dfs_children;
	std::vector<int> dfs_parent;
	std::vector<int> dfs_num;
	std::vector<int> dfs_low;
	int dfs_count;
    bool is_root_ac = false;

    DFSGraph();
    DFSGraph(const Graph& g);
    
    void dt_dfs(int v, int par);
    void dt_dfs_cedges(std::vector<std::vector<std::pair<int, int>>>& cedges, std::vector<std::pair<int, int>>& edge_stack, int v, int par);
    void generate_edge_partition(std::vector<std::vector<std::pair<int, int>>>& cedges, std::vector<std::pair<int, int>>& edge_stack, int root);
    void generate_dfs_tree(int root);
    std::vector<DFSGraph> partition_biconnected();
    bool is_odd_cycle();
    bool is_connected();
    bool is_clique();
};

#endif