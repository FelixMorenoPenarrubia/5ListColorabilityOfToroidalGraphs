#include "TwoTriangleGraph.hh"
#include "ReducibilityTests.hh"
#include "debug_utility.hh"

#include<iostream>
#include<algorithm>
#include<queue>

using std::vector;
using std::endl;
using std::string;



TwoTriangleGraphCode::TwoTriangleGraphCode() {
	
}
	
void TwoTriangleGraphCode::push(int x) {
    code.push_back(x);
}

void TwoTriangleGraphCode::push_f(int triangle) {
    if(!triangle) code.push_back(-1);
    else code.push_back(-2-triangle);
}

void TwoTriangleGraphCode::push_b() {
    code.push_back(-2);
}

int TwoTriangleGraphCode::size() const {
    return code.size();
}

string TwoTriangleGraphCode::to_string() const { //can be made better
    string s = "";
    for (int i=0; i < size(); ++i) {
        if (code[i] == -1) s.push_back('F');
        else if (code[i] == -2) s.push_back('B');
        else if (code[i] == -3) s.push_back('P');
        else if (code[i] == -4) s.push_back('Q');
        else if (code[i] <= 9) s.push_back('0'+code[i]);
        else s.push_back('a'+code[i]-10);
    }
    return s;
}

int TwoTriangleGraphCode::operator[](int i) const {
    return code[i];
}

TwoTriangleGraphCode::TwoTriangleGraphCode(const string& s) {
    for (int i=0; i < (int)s.length(); ++i) {
        if (s[i] == 'F') code.push_back(-1);
        else if (s[i] == 'B') code.push_back(-2);
        else if (s[i] == 'P') code.push_back(-3);
        else if (s[i] == 'Q') code.push_back(-4);
        else if ('0' <= s[i] && s[i] <= '9') code.push_back(s[i]-'0');
        else code.push_back(s[i]-'a'+10);
    }
}

bool TwoTriangleGraphCode::operator<(const TwoTriangleGraphCode& b) const {
	for (int i=0; i < std::min((int)code.size(), b.size()); ++i) {
		if (code[i] < b[i]) return true;
		if (code[i] > b[i]) return false;
	}
	return (int)code.size() < b.size();
}



TwoTriangleGraph::TwoTriangleGraph() {
    n = 0;
    path_length = 0;
}

TwoTriangleGraph::TwoTriangleGraph(vector<vector<int>> _al, vector<vector<int>> _triangles) {

    n = _al.size();
    al = _al;
    triangles = _triangles;
    precolored = vector<int>(n);
    for (int u : triangles[0]) precolored[u] = 1;
    for (int u : triangles[1]) precolored[u] = 1;

    generate_ral_and_m();  
    set_list_sizes();

    path_length = distance_between_triangles();
}

TwoTriangleGraph::TwoTriangleGraph(const Canvas& g, int s) {
    original_canvas = g;

    path_length = (g.l-6)/2;
    
    n = g.n-(path_length+1);
    al = vector<vector<int> >(n);


    vector<int> morph(g.n);
    vector<int> corr(g.n, -1);

    for (int i=0; i < path_length+1; ++i) {
        morph[(s+i)%g.l] = i;
        morph[(s+2*path_length+3-i)%g.l] = i;
        corr[((s+2*path_length+3-i)%g.l)] = (s+i)%g.l;
    }
    morph[(s+path_length+1)%g.l] = path_length+1;
    morph[(s+path_length+2)%g.l] = path_length+2;
    morph[(s-2+g.l)%g.l] = path_length+3;
    morph[(s-1+g.l)%g.l] = path_length+4;
    for (int i=g.l; i < g.n; ++i) {
        morph[i] = i-g.l+path_length+5;
    }

    precolored = vector<int>(n);
    precolored[0] = 1;
    precolored[path_length] = 1;
    precolored[path_length+1] = 1;
    precolored[path_length+2] = 1;
    precolored[path_length+3] = 1;
    precolored[path_length+4] = 1;

    triangles.push_back({0, path_length+3, path_length+4});
    triangles.push_back({path_length, path_length+1, path_length+2});

    vector<vector<int>> am(g.n, vector<int>(g.n));

    for (int u = 0; u < g.n; ++u) {
        for (int v : g.al[u]) {
            am[u][v] = 1;
        }
    }

    for (int i = 0; i < path_length+1; ++i) {
        int u = (s+i)%g.l;
        int nu = (s+i+1)%g.l;
        int us = g.al[u].size();
        for (int j=0;  j < us; ++j) {
            int v = g.al[u][(j+g.ral[u].at(nu))%us];
            al[morph[u]].push_back(morph[v]); //TODO: Assuming no chords
        }
        std::reverse(al[morph[u]].begin(), al[morph[u]].end());
        int cu = (s+2*path_length+3-i)%g.l;
        int cnu = (cu+1)%g.l;
        int cus = g.al[cu].size();
        vector<int> nalu;
        for (int j=(i==0 ? 0 : 1);  j+(i==path_length ? 0 : 1) < cus; ++j) {
            int v = g.al[cu][(j+g.ral[cu].at(cnu))%cus];
            if (!am[u][v]) nalu.push_back(morph[v]); 
        }
        reverse(nalu.begin(), nalu.end());
        for (int x : nalu) al[morph[u]].push_back(x);

    }

    for (int u = 0; u < g.l; ++u) {
        if (morph[u] == -1 || !precolored[morph[u]] || morph[u] == 0 || morph[u] == path_length) continue;
         for (int j = (int)g.al[u].size()-1; j > -1; --j) {
            int v = g.al[u][j];
            if (corr[v] != -1 && am[u][corr[v]]) continue;
            al[morph[u]].push_back(morph[v]);
        }
    }

    for (int u=g.l; u < g.n; ++u) {
        for (int j = (int)g.al[u].size()-1; j > -1; --j) {
            int v = g.al[u][j];
            if (corr[v] != -1 && am[u][corr[v]]) continue;
            al[morph[u]].push_back(morph[v]);
        }
    }

    generate_ral_and_m();  
    set_list_sizes();
}

TwoTriangleGraph::TwoTriangleGraph(const TwoTriangleGraphCode& code) {
    n = 1;
    for (int i = 0; i < code.size(); ++i) {
        if (code[i] == -1 || code[i] == -3 || code[i] == -4) n++;
    }
    al = vector<vector<int>>(n);
    precolored = vector<int>(n);
    precolored[0] = 1;
    int cn = 0;
    vector<int> cv_stack;
    cv_stack.push_back(0);
    vector<vector<int>> prelim_triangles(2);
    prelim_triangles[0].push_back(0);
    for (int i = 0; i < code.size(); ++i) {
        if (code[i] == -1 || code[i] == -3 || code[i] == -4) {
            al[cv_stack.back()].push_back(++cn);
            cv_stack.push_back(cn);
            if (code[i] == -3 || code[i] == -4) {
                precolored[cn] = 1;
                if (code[i] == -3) prelim_triangles[0].push_back(cn);
                else prelim_triangles[1].push_back(cn);
            }
        }
        else if (code[i] == -2) {
            int a = cv_stack.back();
            cv_stack.pop_back();
            int b = cv_stack.back();
            al[a].push_back(b);
        }
        else {
            al[cv_stack.back()].push_back(code[i]);
        }
    }

    

    generate_ral_and_m();
    set_list_sizes();

    if ((int)prelim_triangles[0].size() == 3) {
        triangles = prelim_triangles;
    }
    else {
        vector<int> pcv;
        for (int u = 0; u < n; ++u) {
            if(precolored[u]) pcv.push_back(u);
        }
        int t2, t3;
        t2 = t3 = -1;
        for (int i=1; i < 6 && t2 == -1; ++i) {
            for (int j=i+1; j < 6 && t2 == -1; ++j) {
                if (neighbors(pcv[0], pcv[i]) && neighbors(pcv[0], pcv[j]) && neighbors(pcv[i], pcv[j])) {
                    for (int k=1; k < 6; ++k) {
                        if (k != t2 && k != t3) {
                            for (int l=k+1; l < 6; ++l) {
                                if (l != t2 && l != t3) {
                                    for (int s=l+1; s < 6; ++s) {
                                        if (s != t2 && s != t3) {
                                            if (neighbors(pcv[k], pcv[l]) && neighbors(pcv[l], pcv[s]) && neighbors(pcv[s], pcv[k])) {
                                                t2 = i;
                                                t3 = j;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        triangles = vector<vector<int>>();
        triangles.push_back({pcv[0], pcv[t2], pcv[t3]});
        triangles.push_back({});

        for (int i=1; i < 6; ++i) {
            if (i != t2 && i != t3) triangles[1].push_back(pcv[i]);
        }
    }

    path_length = distance_between_triangles();

}

TwoTriangleGraph TwoTriangleGraph::read(std::istream& is) {
    int n, m, path_length;
    is >> n >> m >> path_length;
    vector<vector<int> > triangles (2, vector<int>(3));
    is >> triangles[0][0] >> triangles[0][1] >> triangles[0][2] >> triangles[1][0] >> triangles[1][1] >> triangles[1][2];
    vector<vector<int>> al(n);
    for (int i=0; i < 2*m; ++i) {
        int u, v;
        is >> u >> v;
        al[u].push_back(v);
    }
    return TwoTriangleGraph(al, triangles);
}


void TwoTriangleGraph::write(std::ostream& os) const {
    os << n << " " << m << " " << path_length << endl;
    for (int u : triangles[0]) os << u << " ";
    for (int u : triangles[1]) os << u << " ";
    os << endl;
    for (int u = 0; u < n; ++u) {
        for (int v : al[u]) {
            os << u << " " << v << endl;
        }
    }
}

//TODO: only works for d = 1 (precolored vertices are prefix)
void TwoTriangleGraph::write_prolog(std::ostream& os) const {
    os << "numVertices(" << n << ")." << endl;
    os << "numVerticesTSubgraph(6)." << endl;
    for (int i=0; i < n; ++i) {
        for (int j : al[i]) {
            if (i < j) {
                if(precolored[i] && precolored[j]) {
                    os << "tEdge(" << i << "," << j << ")." << endl;
                }
            }
        }
    }
    for (int i=0; i < n; ++i) {
        for (int j : al[i]) {
            if (i < j) {
                if(!(precolored[i] && precolored[j])) {
                    os << "iEdge(" << i << "," << j << ")." << endl;
                }
            }
        }
    }
    
}

vector<TwoTriangleGraph> TwoTriangleGraph::generate_from_canvas(const Canvas& g) {
    vector<TwoTriangleGraph> ans;
    for (int i=0; i < g.l; ++i) {
        auto ttg = TwoTriangleGraph(g, i);
        if (!ttg.has_repeated_edges()) {
            ans.push_back(ttg);
        }
    }
    return ans;
}

void TwoTriangleGraph::dfs_code(int u, int idx, int& c, int rt, vector<int>& an, TwoTriangleGraphCode& code) const {
    if (an[u] != -1) {
        code.push(an[u]);
        return;
    }
    if (!precolored[u])
        code.push_f(0);
    else {
        int tr = 0;
        for (int i=0; i < 3; ++i) {
            if (triangles[1][i] == u) tr = 1;
        }
        code.push_f(2-(rt==tr));
    }
    an[u] = c++;
    int als = (int)al[u].size();
    for (int i=1; i < als; ++i) {
        int v = al[u][(i+idx)%als];
        dfs_code(v, ral[v].at(u), c, rt, an, code);
    }
    code.push_b();
}

TwoTriangleGraphCode TwoTriangleGraph::compute_code_edge(int u, int v) const {
    TwoTriangleGraphCode code = TwoTriangleGraphCode();
    vector<int> assigned_numbers (n, -1);
    assigned_numbers[u] = 0;
    int c = 1;
    int rt = 0;
    for (int i=0; i < 3; ++i) {
        if (triangles[1][i] == u) rt = 1;
    }
    int als = (int)al[u].size();
    int idx = ral[u].at(v);
    for (int i=0; i < als; ++i) {
        int w = al[u][(i+idx)%als];
        dfs_code(w, ral[w].at(u), c, rt, assigned_numbers, code);
    }
    return code;
}

//Note: may give different codes for isomorphic graphs if the outer orientation is reversed 
TwoTriangleGraphCode TwoTriangleGraph::compute_code() const {
    TwoTriangleGraphCode code;

    for (int u=0; u < n; ++u) {
        if (precolored[u]) {
            for (int v = u+1; v < n; ++v) {
                if (precolored[v] && ral[v].find(u) != ral[v].end()) {
                    if (code.size() == 0) code = std::min(compute_code_edge(u, v), compute_code_edge(v, u));
                    else code = std::min(code, std::min(compute_code_edge(u, v), compute_code_edge(v, u)));
                }
            }
        }
    }
    
    return code;
}

void TwoTriangleGraph::set_list_sizes() {
    list_sizes = vector<int>(n, 5);
    for (int i=0; i < n; ++i) {
        if (precolored[i]) list_sizes[i] = 1;
    }
}

int TwoTriangleGraph::distance_between_triangles() const {
    vector<int> dist(n, -1);
    std::queue<int> q;
    for (int u : triangles[0]) {
        dist[u] = 0;
        q.push(u);
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : al[u]) {
            if (dist[v] == -1) {
                dist[v] = dist[u]+1;
                q.push(v);
            }
        }
    }
    int ans = dist[triangles[1][0]];
    for (int u : triangles[1]) {
        ans = std::min(ans, dist[u]);
    }
    return ans;
}

bool TwoTriangleGraph::in_precolored_triangle(int u) const {
    return std::find(triangles[0].begin(), triangles[0].end(), u) != triangles[0].end() || std::find(triangles[1].begin(), triangles[1].end(), u) != triangles[1].end();
}

bool TwoTriangleGraph::is_separating_triangle(vector<int> tri) const {
    vector<int> dist(n, -1);
    std::queue<int> q;
    for (int u : triangles[0]) {
        dist[u] = 0;
        q.push(u);
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        if (std::find(tri.begin(), tri.end(), u) != tri.end()) continue;
        for (int v : al[u]) {
            if (dist[v] == -1) {
                dist[v] = dist[u]+1;
                q.push(v);
            }
        }
    }

    int ans = dist[triangles[1][0]];
    for (int u : triangles[1]) {
        ans = std::max(ans, dist[u]);
    }
    return ans == -1;
}

bool TwoTriangleGraph::has_separating_triangle() const {
    vector<int> tri(3);
    for (tri[0] = 0; tri[0] < n; ++tri[0]) {
        for (tri[1] = tri[0] + 1; tri[1] < n; ++tri[1]) {
            if (!neighbors(tri[0], tri[1])) continue;
            for (tri[2] = tri[1] + 1; tri[2] < n; ++tri[2]) {
                if (!neighbors(tri[0], tri[2]) || !neighbors(tri[1], tri[2])) continue;
                if (in_precolored_triangle(tri[0]) && in_precolored_triangle(tri[1]) && in_precolored_triangle(tri[2])) continue;
                if(is_separating_triangle(tri)) return true;
            }
        }
    }
    return false;
}

bool TwoTriangleGraph::test_criticality() const {
    if (n == 6) return true;
    if (distance_between_triangles() < path_length) return false;
    return !batch_test(compute_list_graph());
}

vector<TwoTriangleGraph> TwoTriangleGraph::fuse_triangles(TwoTriangleGraph g1, TwoTriangleGraph g2) {
    vector<TwoTriangleGraph> ans;
    for (int t_idx_2=0; t_idx_2 <= 1; ++t_idx_2) {
        for (int t_ord_2=0; t_ord_2 < 3; ++t_ord_2) {
            ans.push_back(fuse_triangles_fixed(g1, g2));
            g2.triangles[t_idx_2] = {g2.triangles[t_idx_2][1], g2.triangles[t_idx_2][2], g2.triangles[t_idx_2][0]};
        }
    }
    return ans;
}

TwoTriangleGraph TwoTriangleGraph::fuse_triangles_fixed(const TwoTriangleGraph& g1, const TwoTriangleGraph& g2) {
    vector<vector<int>> nal(g1.n + g2.n - 3);
   
    vector<int> morph1(g1.n);
    for (int i=0; i < g1.n; ++i) {
        morph1[i] = i;
    }
    vector<int> morph2(g2.n);
    int cm2 = g1.n;
    for (int i=0; i < g2.n; ++i) {
        if (std::find(g2.triangles[0].begin(), g2.triangles[0].end(), i) == g2.triangles[0].end()) {
            morph2[i] = cm2++;
        }
    }
    for (int i=0; i < 3; ++i) {
        morph2[g2.triangles[0][i]] = morph1[g1.triangles[1][i]];
        nal[morph1[g1.triangles[1][i]]].push_back(morph1[g1.al[g1.triangles[1][i]][0]]);
    }
    

    for (int u=0; u < g2.n; ++u) {
        for (int v : g2.al[u]) {
            if (std::find(g2.triangles[0].begin(), g2.triangles[0].end(), u) == g2.triangles[0].end() || std::find(g2.triangles[0].begin(), g2.triangles[0].end(), v) == g2.triangles[0].end()) {
                nal[morph2[u]].push_back(morph2[v]);
            }
        }
    }
    
    for (int u=0; u < g1.n; ++u) {
        for (int i=0; i < (int)g1.al[u].size(); ++i) {
            if (i == 0 && (u == g1.triangles[1][0] || u == g1.triangles[1][1] || u == g1.triangles[1][2])) continue; 
            int v = g1.al[u][i];
            nal[morph1[u]].push_back(morph1[v]);
        }
    }

    

    vector<vector<int>> ntri = {{morph1[g1.triangles[0][0]], morph1[g1.triangles[0][1]], morph1[g1.triangles[0][2]]},
    {morph2[g2.triangles[1][0]], morph2[g2.triangles[1][1]], morph2[g2.triangles[1][2]]}};
    return TwoTriangleGraph(nal, ntri);
    
}



vector<TwoTriangleGraph> TwoTriangleGraph::add_tetrahedral_band(const TwoTriangleGraph& g) {
    vector<TwoTriangleGraph> ans;
    for (int t_idx = 0; t_idx < 2; ++t_idx) {
        for (int v_idx = 0; v_idx < 3; ++v_idx) {
            vector<vector<int>> nal = g.al;
            nal.push_back({g.triangles[t_idx][v_idx], g.triangles[t_idx][(v_idx+2)%3]});
            ans.push_back(TwoTriangleGraph(nal, {{g.triangles[t_idx][v_idx], g.triangles[t_idx][(v_idx+2)%3], g.n}, g.triangles[1-t_idx]}));
            nal.pop_back();
            nal.push_back({g.triangles[t_idx][v_idx], g.triangles[t_idx][(v_idx+1)%3], g.triangles[t_idx][(v_idx+2)%3]});
            ans.push_back(TwoTriangleGraph(nal, {{g.triangles[t_idx][v_idx], g.triangles[t_idx][(v_idx+2)%3], g.n}, g.triangles[1-t_idx]}));
        }
    }
    return ans;
}

vector<ListGraph> TwoTriangleGraph::identify_triangles() const {
    vector<ListGraph> ans;
    vector<int> p(3);
    p[0] = 0;
    p[1] = 1;
    p[2] = 2;
    if (!triangles_have_same_orientation()) {
        std::swap(p[1], p[2]);
    }
    for (int j=0; j < 3; ++j) {
        ans.emplace_back(identify_vertices(triangles[0], {triangles[1][p[0]], triangles[1][p[1]], triangles[1][p[2]]}).al, vector<int>(n-3, 5));
        p[0]++;
        p[1]++;
        p[2]++;
        p[1]%=3;
        p[2]%=3;
    } 

    return ans;
}

void TwoTriangleGraph::set_first_triangle_as_outer_face()  {
    if ((ral[triangles[0][0]][triangles[0][1]]+1)%((int)al[triangles[0][0]].size()) != ral[triangles[0][0]][triangles[0][2]]) {
        for (int i=0; i < n; ++i) {
            reverse(al[i].begin(), al[i].end());
        }
        generate_ral_and_m();
    }
    for (int i=0; i < 3; ++i) {
        int s = (int)al[triangles[0][i]].size();
        int u = triangles[0][i];
        int stj = ral[u][triangles[0][(i+2)%3]];
        vector<int> nal;
        nal.push_back(triangles[0][(i+2)%3]);
        for (int j=1; j+1 < s; ++j) {
            nal.push_back(al[u][(stj+j)%s]);
        }
        nal.push_back(triangles[0][(i+1)%3]);
        al[u] = nal;

    }
    for (int i=0; i < n; ++i) {
            reverse(al[i].begin(), al[i].end());
        
    }
    m=0;
    generate_ral_and_m();




}

void TwoTriangleGraph::set_second_triangle_as_inner_face() {
    if ((ral[triangles[1][0]][triangles[1][1]]+1)%((int)al[triangles[1][0]].size()) != ral[triangles[1][0]][triangles[1][2]]) {
        reverse(triangles[1].begin(), triangles[1].end());
    }
    set_outer_face(triangles[1]);
}

bool TwoTriangleGraph::triangles_have_same_orientation() const {
    bool i1 = false;
    if ((ral[triangles[1][0]].at(triangles[1][1])+1)%((int)al[triangles[1][0]].size()) != ral[triangles[1][0]].at(triangles[1][2])) {
        i1 = true;
    }
    bool i0 = false;
    if ((ral[triangles[0][0]].at(triangles[0][1])+1)%((int)al[triangles[0][0]].size()) != ral[triangles[0][0]].at(triangles[0][2])) {
        i0 = true;
    }
    return i0 != i1;
}