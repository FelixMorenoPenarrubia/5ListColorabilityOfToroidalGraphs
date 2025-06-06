#include "ListGraph.hh"
#include "debug_utility.hh"


using std::string;
using std::vector;
using std::endl;

void ListGraphCode::push_r(int x) {
    code.push_back(1+x);
}

void ListGraphCode::push_f(int list_size) {
    code.push_back(F_OFFSET+list_size);
}

void ListGraphCode::push_b() {
    code.push_back(0);
}

int ListGraphCode::size() const {
    return code.size();
}

string ListGraphCode::to_string() const { 
    string s = "";
    for (int i=0; i < size(); ++i) {
        if (code[i] < 0) {
            s.push_back('F');
            int ls = code[i]-F_OFFSET;
            if (ls >= 0) {
                s.push_back('0'+ls);
            }
            else {
                s.push_back('-');
                s.push_back('0'-ls);
            }
        }
        else if (code[i] > 0) { 
            s.push_back('R');
            s.push_back('a'+code[i]-1);
        }
        else {
            s.push_back('B');
        }
    }
    return s;
}
int ListGraphCode::operator[](int i) const {
    return code[i];
}

bool ListGraphCode::operator<(const ListGraphCode& b) const {
	for (int i=0; i < std::min(size(), b.size()); ++i) {
		if (code[i] < b[i]) return true;
		if (code[i] > b[i]) return false;
	}
	return size() < b.size();
}


ListGraph::ListGraph() {
    list_sizes = vector<int>();
}


ListGraph::ListGraph(vector<vector<int>> _al, vector<int> _list_sizes) {
    n = _al.size();
    m = 0;
    al = _al;
    list_sizes = _list_sizes;
    generate_ral_and_m();
}

ListGraph ListGraph::remove_vertex(int w) const {
    vector<int> morph(n);
    for (int u = 0; u < w; ++u) {
        morph[u] = u;
    }
    for (int u = w+1; u < n; ++u) {
        morph[u] = u-1;
    }
    vector<int> nls (n-1, 0);
    vector< vector<int> > nal (n-1);
    for (int u = 0; u < n; ++u) {
        if (u != w) {
            nls[morph[u]] += list_sizes[u];
            for (int v : al[u]) {
                if (v != w) {
                    nal[morph[u]].push_back(morph[v]);
                }
            }
        }
    }
    return ListGraph(nal, nls);
}

ListGraph ListGraph::precolor_vertex(int w) const {
    vector<int> morph(n);
    for (int u = 0; u < w; ++u) {
        morph[u] = u;
    }
    for (int u = w+1; u < n; ++u) {
        morph[u] = u-1;
    }
    vector<int> nls (n-1, 0);
    vector< vector<int> > nal (n-1);
    for (int u = 0; u < n; ++u) {
        if (u == w) {
            for (int v : al[u]) {
                nls[morph[v]]--;
            }
        }
        else {
            nls[morph[u]] += list_sizes[u];
            for (int v : al[u]) {
                if (v != w) {
                    nal[morph[u]].push_back(morph[v]);
                }
            }
        }
    }
    return ListGraph(nal, nls);
}

ListGraph ListGraph::precolor_vertex_smart(int w, int wp) const {
    vector<int> morph(n);
    for (int u = 0; u < w; ++u) {
        morph[u] = u;
    }
    for (int u = w+1; u < n; ++u) {
        morph[u] = u-1;
    }
    vector<int> nls (n-1, 0);
    vector< vector<int> > nal (n-1);
    for (int u = 0; u < n; ++u) {
        if (u == w) {
            for (int v : al[u]) {
                if (v != wp) nls[morph[v]]--;
            }
        }
        else {
            nls[morph[u]] += list_sizes[u];
            for (int v : al[u]) {
                if (v != w) {
                    nal[morph[u]].push_back(morph[v]);
                }
            }
        }
    }
    return ListGraph(nal, nls);
}

ListGraph ListGraph::precolor_vertices_smart_twoneighbors(int wp, int w1, int w2) const {
    if (w1 > w2) std::swap(w1, w2);
    vector<int> morph(n);
    for (int u = 0; u < w1; ++u) {
        morph[u] = u;
    }
    for (int u = w1+1; u < w2; ++u) {
        morph[u] = u-1;
    }
    for (int u = w2+1; u < n; ++u) {
        morph[u] = u-2;
    }
    vector<int> nls (n-2, 0);
    vector< vector<int> > nal (n-2);
    for (int u = 0; u < n; ++u) {
        if (u == w1 || u == w2) {
            for (int v : al[u]) {
                nls[morph[v]]--;
            }
        }
        else {
            nls[morph[u]] += list_sizes[u];
            for (int v : al[u]) {
                if (v != w1 && v != w2) {
                    nal[morph[u]].push_back(morph[v]);
                }
            }
        }
    }
    nls[morph[wp]]++;
    return ListGraph(nal, nls);
}

bool ListGraph::nocolors() const {
    bool ans = true;
    for (int i = 0; i < n && ans; ++i) {
        if (list_sizes[i] > 0) ans = false;
    }
    return ans;
}

void ListGraph::dfs_code(int u, int idx, int& c, vector<int>& an, ListGraphCode& code) const {
    if (an[u] != -1) {
        code.push_r(an[u]);
        return;
    }

    code.push_f(list_sizes[u]);
    an[u] = c++;
    int als = (int)al[u].size();
    for (int i=1; i < als; ++i) {
        int v = al[u][(i+idx)%als];
        dfs_code(v, ral[v].at(u), c, an, code);
    }
    code.push_b();
}

ListGraphCode ListGraph::compute_list_code_edge(int u, int v) const {
    ListGraphCode code = ListGraphCode();
    vector<int> assigned_numbers (n, -1);
    assigned_numbers[u] = 0;
    code.push_f(list_sizes[u]);
    int c = 1;
    for (int i = 0; i < (int)al[u].size(); ++i) {
        int w = al[u][(i+ral[u].at(v))%al[u].size()];
        dfs_code(w, ral[w].at(u), c, assigned_numbers, code);
    }
    return code;
}

ListGraphCode ListGraph::compute_list_code() const {
    ListGraphCode code = ListGraphCode();

    if (n == 1) {
        code.push_f(list_sizes[0]);
        return code;
    }

    for (int u = 0; u < n; ++u) {
        for (int v : al[u]) {

            if (code.size() == 0) code = compute_list_code_edge(u, v);
            else code = std::min(code, compute_list_code_edge(u, v));
        }
    }
    return code;
}

void ListGraph::dfs_cc(int u, vector<int>& visited, vector<int>& vis_stack) const {
    if (visited[u]) return;
    visited[u] = 1;
    vis_stack.push_back(u);
    for (int v : al[u]) {
        dfs_cc(v, visited, vis_stack);
    }
}

vector<ListGraph> ListGraph::connected_components() const {
    vector<int> visited(n);
    vector<ListGraph> ans;
    for (int u = 0; u < n; ++u) {
        if (!visited[u]) {
            vector<int> vis_st;
            dfs_cc(u, visited, vis_st);
            int nn = vis_st.size();
            std::map<int, int> morph;
            vector<vector<int> > nal (nn);
            vector<int> nls (nn);

            for (int i=0; i < nn; ++i) {
                morph[vis_st[i]] = i;
            }

            for (int i=0; i < nn; ++i) {
                nls[i] = list_sizes[vis_st[i]];
                for (int v : al[vis_st[i]]) {
                    nal[i].push_back(morph[v]);
                }
            }

            ans.emplace_back(nal, nls);
        }
    }
    return ans;
}


ListGraph ListGraph::degree_assignment_subgraph() const {
    ListGraph lg = ListGraph(al, list_sizes);
    for (int u=0; u < n; ++u) {
        if ((int)al[u].size() > list_sizes[u]) {
            return lg.precolor_vertex(u).degree_assignment_subgraph();
        }
    }
    return lg;
}

bool ListGraph::subgraph_test(const ListGraph& h) const {
    vector<int> f (h.n, -1); //to which vertex from g are the vertices of h mapped to
    vector<int> used(n, 0); // which vertices are used
    int cv = 0; //current vertex of h that we are mapping

    // test three properties:
    // 1. vertex not already used
    // 2. lists of right size (incl. interior edges)
    // 3. neighbors from previously selected vertices
    auto test_vertex = [&] () -> bool {
        if (used[f[cv]]) return false;
        if (list_sizes[f[cv]] - (int) al[f[cv]].size() + (int)h.al[cv].size() < h.list_sizes[cv]) return false;

        for (int cu = 0; cu < cv; ++cu) {
            if (neighbors(f[cu], f[cv]) != h.neighbors(cu, cv)) return false;
        }

        return true;
    };
    while (cv != -1) {
        if (cv == h.n) {
            return true;
        }
        if (f[cv] > -1) {
            used[f[cv]] = 0;
        }
        f[cv]++;
        while (f[cv] < n && !test_vertex()) {
            f[cv]++;
        }
        if (f[cv] == n) {
            f[cv] = -1;
            cv--;
        }
        else {
            used[f[cv]] = 1;
            cv++;
        }

    }
    return false;
}

void ListGraph::write(std::ostream& os) const {
    os << n << " " << m << endl;
    for (int i=0; i < n; ++i) {
        if (i > 0) os << " ";
        os << list_sizes[i];
    }
    os << endl;
    for (int u=0; u < n; ++u) {
        for (int v : al[u]) {
            os << u << " " << v << endl;
        }
    }
}

ListGraph ListGraph::read(std::istream& is) {
    int n, m;
    is >> n >> m;
    vector<int> ls(n);
    for (int i=0; i < n; ++i) {
        is >> ls[i];
    }
    vector<vector<int>> a(n);
    for (int i=0; i < 2*m; ++i) {
        int u, v;
        is >> u >> v;
        a[u].push_back(v);
    }
    return ListGraph(a, ls);
}

ListGraph ListGraph::reverse_al_order_lg() const {
    vector<int> ls = list_sizes;
    vector<vector<int>> nal = al;
    for (int u=0; u < n; ++u) std::reverse(nal[u].begin(), nal[u].end());
    return ListGraph(nal, ls);
}

ListGraph ListGraph::shuffle_order(std::mt19937 gen) const {
    vector<int> morph(n);
    for (int i=0; i < n; ++i) {
        morph[i] = i;
    }
    std::shuffle(morph.begin(), morph.end(), gen);
    vector<int> ls(n);
    for (int i=0; i < n; ++i) {
        ls[morph[i]] = list_sizes[i];
    }
    vector<vector<int>> nal(n);
    for (int u=0; u < n; ++u) {
        for (int v : al[u]) {
            nal[morph[u]].push_back(morph[v]);
        }
        std::shuffle(nal[morph[u]].begin(), nal[morph[u]].end(), gen);
    }

    return ListGraph(nal, ls);
}

