#include "CanvasSearch.hh"
#include "debug_utility.hh"
#include "Parallelism.hh"
#include "TwoTriangleGraph.hh"
#include <queue>
#include<iostream>
#include<thread>
#include<unistd.h>


using ll = long long; //for bitmasks




bool CanvasSearch::DFS_MODE = false;
bool CanvasSearch::HALFMEMORY_MODE = false;
CanvasHashList CanvasSearch::ch;



CanvasSearch::CanvasSearch() {
    critical_chordless = vector<CanvasList>(3);
    critical_with_chords = vector<CanvasList>(3);
    critical_with_size34_chords = vector<CanvasList>(3);
}

bool CanvasSearch::test_canvas(const Canvas& g) {
    
    bool res = g.test_criticality(); 
    
    return res;
}

void CanvasSearch::add_canvas_real(const Canvas& g, CanvasList& cl) {
    CanvasCode cd = g.compute_code();
    #ifdef PARALLEL
    Parallelism::canvas_list_queue_mutex.lock();
    #endif
    bool nf = cl.find(cd) == cl.end();
    #ifdef PARALLEL
    Parallelism::canvas_list_queue_mutex.unlock();
    #endif
    if (nf) {
        if (test_canvas(g)) {
            #ifdef PARALLEL
            Parallelism::canvas_list_queue_mutex.lock();
            #endif
            cl.insert(cd);
            #ifdef PARALLEL
            Parallelism::canvas_list_queue_mutex.unlock();
            #endif
        }
    }
}

void CanvasSearch::add_canvas_q_and_cl_real(const Canvas& g, CanvasList& cl, std::queue<CanvasCode>& q) {
     
   
    CanvasCode cd = g.compute_code();
    #ifdef PARALLEL
    Parallelism::canvas_list_queue_mutex.lock();
    #endif
    bool nf = cl.find(cd) == cl.end();
    #ifdef PARALLEL
    Parallelism::canvas_list_queue_mutex.unlock();
    #endif
    if (nf) {
        if (test_canvas(g)) {
            #ifdef PARALLEL
            Parallelism::canvas_list_queue_mutex.lock();
            #endif
            q.push(cd);
            cl.insert(cd);
            #ifdef PARALLEL
            Parallelism::canvas_list_queue_mutex.unlock();
            #endif
        }
    }
    
}

void CanvasSearch::add_canvas_q_real(const Canvas& g, std::queue<CanvasCode>& q) {
     
   
    
    if (test_canvas(g)) {
        CanvasCode cd = g.compute_code();
        #ifdef PARALLEL
        Parallelism::canvas_list_queue_mutex.lock();
        #endif
        q.push(cd);
        #ifdef PARALLEL
        Parallelism::canvas_list_queue_mutex.unlock();
        #endif
    }
    
}

void CanvasSearch::add_canvas_dfs_parallel(const Canvas& g) {
    CanvasCode c = g.compute_code();
    cchash h = c.hash();
    Parallelism::canvas_hash_list_mutex.lock();
    bool found = ch.find(h) != ch.end();
    Parallelism::canvas_hash_list_mutex.unlock();
    if (!found) {
        if (test_canvas(g)) {
            Parallelism::canvas_hash_list_mutex.lock();
            std::cout << c.to_string() << '\n';
            ch.insert(h);
            Parallelism::canvas_hash_list_mutex.unlock();
            std::vector<CanvasCode> st;
            st.push_back(c);

            while(true) {

                Parallelism::canvas_list_queue_mutex.lock();
                Canvas g = Canvas(st.back());
                st.pop_back();
                Parallelism::canvas_list_queue_mutex.unlock();
                
                for(int j=0; j < g.l; ++j) { //for each outer vertex of the canvas
                    Canvas ng = g.add_tripod(1, j, 1);
                    cchash h = ng.compute_code().hash();
                    Parallelism::canvas_hash_list_mutex.lock();
                    bool found = ch.find(h) != ch.end();
                    Parallelism::canvas_hash_list_mutex.unlock();
                    if(!found) {
                        Parallelism::spawn_thread_addcanvas_st(ng, st);
                    }
                }
                
                Parallelism::canvas_list_queue_mutex.lock();
                bool empt = st.empty();
                Parallelism::canvas_list_queue_mutex.unlock();
                if (empt) {
                    Parallelism::wait_for_threads();
                    Parallelism::canvas_list_queue_mutex.lock();
                    empt = st.empty();
                    Parallelism::canvas_list_queue_mutex.unlock();
                    if (empt) break;
                }
            }
        }
    }
}

void CanvasSearch::add_canvas_hash(const Canvas& g) {
    CanvasCode c = g.compute_code();
    cchash h = c.hash();

    #ifdef PARALLEL
    Parallelism::canvas_hash_list_mutex.lock();
    #endif
    bool found = ch.find(h) != ch.end();
    #ifdef PARALLEL
    Parallelism::canvas_hash_list_mutex.unlock();
    #endif

    if (!found) {
        if (test_canvas(g)) {
            #ifdef PARALLEL
            Parallelism::canvas_hash_list_mutex.lock();
            #endif
            std::cout << c.to_string() << '\n';
            ch.insert(h);
            #ifdef PARALLEL
            Parallelism::canvas_hash_list_mutex.unlock();
            #endif
        }
    }
}

void CanvasSearch::add_canvas_to_triangles(const Canvas& g) {
    if (test_canvas(g)) {
        for (auto tr : TwoTriangleGraph::generate_from_canvas(g)) {
            if(tr.test_criticality()) {
                #ifdef PARALLEL
                Parallelism::canvas_hash_list_mutex.lock();
                #endif
                std::cout << tr.compute_code().to_string() << '\n';
                #ifdef PARALLEL
                Parallelism::canvas_hash_list_mutex.unlock();
                #endif
            }
        }
    }
}


void CanvasSearch::add_canvas_st_real(const Canvas& g, vector<CanvasCode>& st) {

    
    
    if (test_canvas(g)) {
        CanvasCode c = g.compute_code();
        cchash h = c.hash();
        Parallelism::canvas_hash_list_mutex.lock();
        bool found = ch.find(h) != ch.end();
        Parallelism::canvas_hash_list_mutex.unlock();
        if (!found) {
            Parallelism::canvas_hash_list_mutex.lock();
            std::cout << c.to_string() << '\n';
            ch.insert(h);
            Parallelism::canvas_hash_list_mutex.unlock();
            Parallelism::canvas_list_queue_mutex.lock();
            st.push_back(c);
            Parallelism::canvas_list_queue_mutex.unlock();
        }
    }
    
}

void CanvasSearch::add_canvas_dfs_serial(const Canvas& g) {

    CanvasCode c = g.compute_code();
    cchash h = c.hash();
    
   
    bool found = ch.find(h) != ch.end();

    
    if (!found) {
        if (test_canvas(g)) {
            std::cout << c.to_string() << '\n';
            ch.insert(h);
            for(int j=0; j < g.l; ++j) { 
                Canvas ng = g.add_tripod(1, j, 1);
                add_canvas_dfs(ng);    
            }
        }
    }
}

void CanvasSearch::process_canvas_dfs_parallel(const Canvas& g) {
    std::queue<CanvasCode> q;
    q.push(g.compute_code());
    while (!q.empty()) {
        Canvas g = Canvas(q.front());
        q.pop();
        for(int j=0; j < g.l; ++j) { 
            Canvas ng = g.add_tripod(1, j, 1);
            CanvasCode nc = ng.compute_code();
            cchash h = nc.hash();
            Parallelism::canvas_hash_list_mutex.lock();
            bool found = ch.find(h) != ch.end();
            Parallelism::canvas_hash_list_mutex.unlock();
            if (!found) {
                if (test_canvas(ng)) {
                    Parallelism::canvas_hash_list_mutex.lock();
                    found = ch.find(h) != ch.end();
                    if (!found) {
                        ch.insert(h);
                        std::cout << nc.to_string() << '\n';
                    }
                    Parallelism::canvas_hash_list_mutex.unlock();
                    if (!found) q.push(nc);
                }
            }    
        }
    }
}

void CanvasSearch::add_canvas_dfs_parallel_2(const Canvas& g) {
    CanvasCode c = g.compute_code();
    cchash h = c.hash();
    
    Parallelism::canvas_hash_list_mutex.lock();
    bool found = ch.find(h) != ch.end();
    Parallelism::canvas_hash_list_mutex.unlock();
    
    if (!found) {
        if (test_canvas(g)) {
            Parallelism::canvas_hash_list_mutex.lock();
            found = ch.find(h) != ch.end();
            if (!found) {
                ch.insert(h);
                std::cout << c.to_string() << '\n';
            }
            Parallelism::canvas_hash_list_mutex.unlock();
            if (!found) process_canvas_dfs_parallel(g);
        }
    }
}

void CanvasSearch::add_canvas_dfs(const Canvas& g) {
    #ifdef PARALLEL
    Parallelism::spawn_thread([g] () {add_canvas_dfs_parallel_2(g);});
    #else
    add_canvas_dfs_serial(g);
    #endif
}

void CanvasSearch::add_canvas(const Canvas& g, CanvasList& cl) {
    
    if (DFS_MODE) {
        add_canvas_dfs(g);
    }
    else {
        #ifdef PARALLEL
        Parallelism::spawn_thread_addcanvas(g, cl);
        #else
        add_canvas_real(g, cl);
        #endif
    }
    
}

void CanvasSearch::add_canvas_q(const Canvas& g, std::queue<CanvasCode>& q) {
    #ifdef PARALLEL
    Parallelism::spawn_thread_addcanvas_q(g, q);
    #else
    add_canvas_q_real(g, q);
    #endif
}

void CanvasSearch::add_canvas_q_and_cl(const Canvas& g, CanvasList& cl, std::queue<CanvasCode>& q) {
    #ifdef PARALLEL
    Parallelism::spawn_thread_addcanvas_q_and_cl(g, cl, q);
    #else
    add_canvas_q_and_cl_real(g, cl, q);
    #endif
}

void CanvasSearch::add_smaller_chords(int l, const vector<CanvasList>& prev, CanvasList& curr) const {
    for(int a=3; a < l; ++a) {
        int b = l-a+2;
        for(auto p1 : prev[a]) {
            for(auto p2 : prev[b]) {
                Canvas g1 = Canvas(p1);
                Canvas g2 = Canvas(p2);
                for(int j1=0; j1 < a; ++j1) {
                    for(int j2=0; j2 < b; ++j2) {
                        Canvas ng1 = Canvas::fuse_chord(g1, g2, j1, j2, false);
                        Canvas ng2 = Canvas::fuse_chord(g1, g2, j1, j2, true);
                        add_canvas(ng1, curr);
                        add_canvas(ng2, curr);
                    }
                }
            }
        }
    }
}

void CanvasSearch::add_smaller_size34_chords(int l, const vector<CanvasList>& prev, CanvasList& curr) const {
    for(int a=3; a < std::min(l, 5); ++a) {
        int b = l-a+2;
        for(auto p1 : prev[a]) {
            for(auto p2 : prev[b]) {
                Canvas g1 = Canvas(p1);
                Canvas g2 = Canvas(p2);
                for(int j1=0; j1 < a; ++j1) {
                    for(int j2=0; j2 < b; ++j2) {
                        Canvas ng1 = Canvas::fuse_chord(g1, g2, j1, j2, false);
                        Canvas ng2 = Canvas::fuse_chord(g1, g2, j1, j2, true);
                        add_canvas(ng1, curr);
                        add_canvas(ng2, curr);
                    }
                }
            }
        }
    }
}




void CanvasSearch::add_smaller_tripods(int l, const vector<CanvasList>& prev, CanvasList& curr) const {
    for(int k=3; k < l; ++k) {  //for each canvas size up to l-1
		ll ts = l-k+1;
		for(auto p : prev[k]) { //for each canvas of that size
			Canvas g = Canvas(p);
			for(int j=0; j < k; ++j) { //for each outer vertex of the canvas
				for(ll bm=1; bm < (1LL<<ts); ++bm) { //for each disposition of the chords of the tripod (bitmask)
					Canvas ng = g.add_tripod(ts, j, bm);
					add_canvas(ng, curr);
				}
			}
		}
	}
    
}

void CanvasSearch::add_same_size_tripods(int l, CanvasList& curr) const {
    #ifdef PARALLEL
    add_same_size_tripods_parallel(l, curr);
    return;
    #endif

    std::queue<CanvasCode> q;
    for (auto p : curr) {
        q.push(p);
    }
    while(!q.empty()) {
		Canvas g = Canvas(q.front());
		q.pop();
		for(int j=0; j < l; ++j) { //for each outer vertex of the canvas
			Canvas ng = g.add_tripod(1, j, 1);
			add_canvas_q_and_cl(ng, curr, q);
		}
	}
}



void CanvasSearch::add_same_size_tripods_parallel(int l, CanvasList& curr) const {
   
    Parallelism::wait_for_threads();

    std::queue<CanvasCode> q;
    for (auto p : curr) {
        q.push(p);
    }

    while(true) {

        Parallelism::canvas_list_queue_mutex.lock();
		Canvas g = Canvas(q.front());
		q.pop();
        Parallelism::canvas_list_queue_mutex.unlock();
		for(int j=0; j < l; ++j) { //for each outer vertex of the canvas
			Canvas ng = g.add_tripod(1, j, 1);
			add_canvas_q_and_cl(ng, curr, q);
		}
        Parallelism::canvas_list_queue_mutex.lock();
        bool empt = q.empty();
        Parallelism::canvas_list_queue_mutex.unlock();
        if (empt) {
            Parallelism::wait_for_threads();
             Parallelism::canvas_list_queue_mutex.lock();
            empt = q.empty();
            Parallelism::canvas_list_queue_mutex.unlock();
            if (empt) break;
        }
       
	}
}



void CanvasSearch::add_same_size_tripods_halfmemory(int l, CanvasList& curr) const {
    #ifdef PARALLEL
    add_same_size_tripods_parallel_halfmemory(l, curr);
    return;
    #endif

    std::queue<CanvasCode> q;
     while (!curr.empty()) {
        q.push(*curr.begin());
        curr.erase(curr.begin());
    }
    while(!q.empty()) {
		Canvas g = Canvas(q.front());
        bool found = curr.find(q.front()) != curr.end();
        if (!found) curr.insert(q.front());
		q.pop();
        if (!found) {
            
            for(int j=0; j < l; ++j) { //for each outer vertex of the canvas
                Canvas ng = g.add_tripod(1, j, 1);
                add_canvas_q(ng, q);
            }
        }
	}
}



void CanvasSearch::add_same_size_tripods_parallel_halfmemory(int l, CanvasList& curr) const {
   
    Parallelism::wait_for_threads();

    std::queue<CanvasCode> q;
    while (!curr.empty()) {
        q.push(*curr.begin());
        curr.erase(curr.begin());
    }

    while(true) {

        Parallelism::canvas_list_queue_mutex.lock();
		Canvas g = Canvas(q.front());
        bool found = curr.find(q.front()) != curr.end();
        if (!found) curr.insert(q.front());
		q.pop();
        Parallelism::canvas_list_queue_mutex.unlock();
        if (!found) {
            for(int j=0; j < l; ++j) { //for each outer vertex of the canvas
                Canvas ng = g.add_tripod(1, j, 1);
                add_canvas_q(ng, q);
            }
        }
        Parallelism::canvas_list_queue_mutex.lock();
        bool empt = q.empty();
        Parallelism::canvas_list_queue_mutex.unlock();
        if (empt) {
            Parallelism::wait_for_threads();
            Parallelism::canvas_list_queue_mutex.lock();
            empt = q.empty();
            Parallelism::canvas_list_queue_mutex.unlock();
            if (empt) break;
        }
        
	}
}

CanvasList CanvasSearch::generate_chordless(int l) const {
    CanvasList ans;
    add_canvas(Canvas::empty_cycle(l), ans);
    add_smaller_tripods(l, critical_chordless, ans);
    if (!DFS_MODE) {
        if(HALFMEMORY_MODE) {
            add_same_size_tripods_halfmemory(l, ans);
        }
        else {
            add_same_size_tripods(l, ans);
        }
    }
    return ans;
}

CanvasList CanvasSearch::generate_with_chords(int l) const {
    CanvasList ans;
    add_canvas(Canvas::empty_cycle(l), ans);
    add_smaller_chords(l, critical_with_chords, ans);
    add_smaller_tripods(l, critical_with_chords, ans);
    add_same_size_tripods(l, ans);
    return ans;
}

CanvasList CanvasSearch::generate_with_size34_chords(int l) const {
    CanvasList ans;
    add_canvas(Canvas::empty_cycle(l), ans);
    add_smaller_size34_chords(l, critical_with_size34_chords, ans);
    add_smaller_tripods(l, critical_with_size34_chords, ans);
    add_same_size_tripods(l, ans);
    #ifdef PARALLEL
    Parallelism::wait_for_threads();
    #endif
    return ans;
}

vector<Canvas> CanvasSearch::get_chordless(int l) {
    for (int i=(int)critical_chordless.size(); i <= l; ++i) {
        critical_chordless.push_back(generate_chordless(i));
    }
    vector<Canvas> ans;
    for (auto p : critical_chordless[l]) {
        ans.push_back(Canvas(p));
    }
    return ans;
}

vector<Canvas> CanvasSearch::get_with_chords(int l) {
    for (int i=(int)critical_with_chords.size(); i <= l; ++i) {
        critical_with_chords.push_back(generate_with_chords(i));
    }
    vector<Canvas> ans;
    for (auto p : critical_with_chords[l]) {
        ans.push_back(Canvas(p));
    }
    return ans;
}

CanvasList& CanvasSearch::get_with_size34_chords(int l) {
    for (int i=(int)critical_with_size34_chords.size(); i <= l-1; ++i) {
        critical_with_size34_chords.push_back(generate_with_size34_chords(i));
    }
    critical_with_size34_chords.resize(l+1);
    add_smaller_size34_chords(l, critical_with_size34_chords, critical_with_size34_chords[l]);
    #ifdef PARALLEL
    Parallelism::wait_for_threads();
    #endif
    return critical_with_size34_chords[l];
}

void CanvasSearch::process_canvas_chords(const vector<CanvasCode> va, const vector<CanvasCode> vb) {
     for(auto p1 : va) {
        for(auto p2 : vb) {
            Canvas g1 = Canvas(p1);
            Canvas g2 = Canvas(p2);
            int a = g1.l;
            int b = g2.l;
            for(int j1=0; j1 < a; ++j1) {
                for(int j2=0; j2 < b; ++j2) {
                    Canvas ng1 = Canvas::fuse_chord(g1, g2, j1, j2, false);
                    Canvas ng2 = Canvas::fuse_chord(g1, g2, j1, j2, true);
                    add_canvas_to_triangles(ng1);
                    add_canvas_to_triangles(ng2);
                }
            }
        }
    }
}

void CanvasSearch::read_previous_and_print_w34c(int l) {
    #ifdef PARALLEL
    read_previous_and_print_w34c_parallel(l);
    #endif

    for (int i=(int)critical_with_size34_chords.size(); i <= 4; ++i) {
        critical_with_size34_chords.push_back(generate_with_size34_chords(i));
    }

    vector<CanvasCode> v3;
    vector<CanvasCode> v4;
    for (auto c : critical_with_size34_chords[3]) v3.push_back(c);
    for (auto c : critical_with_size34_chords[4]) v4.push_back(c);
    
    string str;
    while (std::cin >> str) {
        vector<CanvasCode> v (1, CanvasCode(str));
        Canvas g = Canvas(v[0]);
        if (g.l == l) {
            add_canvas_hash(g);
        }
        if (g.l == l-1) {
            process_canvas_chords(v3, v);
        }
        if (g.l == l-2) {
            process_canvas_chords(v4, v);
        }
    }
    
}

void CanvasSearch::read_previous_and_print_w34c_parallel(int l) {

   for (int i=(int)critical_with_size34_chords.size(); i <= 4; ++i) {
        critical_with_size34_chords.push_back(generate_with_size34_chords(i));
    }

    const int BLOCK_SIZE = 1000;
    const int REPORT_INTERVAL = 1000000;

    vector<CanvasCode> v3;
    vector<CanvasCode> v4;
    for (auto c : critical_with_size34_chords[3]) v3.push_back(c);
    for (auto c : critical_with_size34_chords[4]) v4.push_back(c);

    
    string str;
    vector<CanvasCode> vm4;
    vector<CanvasCode> vm3;
    vector<CanvasCode> vm;

    int cnt = 0;

    auto process_vec = [&] (bool flag) {
        if (vm.size()%BLOCK_SIZE == 0 || flag) {
            Parallelism::spawn_thread([vm] () {
                Parallelism::canvas_hash_list_mutex.lock();
                for (auto c : vm) {
                    auto h = c.hash();
                    if (ch.find(h) == ch.end()) {
                        std::cout << c.to_string() << '\n';
                        ch.insert(h);
                    }
                }
                Parallelism::canvas_hash_list_mutex.unlock();
            });
            vm.clear();
        }
        if (vm3.size()%BLOCK_SIZE == 0 || flag) {
            Parallelism::spawn_thread([v3, vm3] () {
                process_canvas_chords(v3, vm3);
            });
            vm3.clear();
        }
        if (vm4.size()%BLOCK_SIZE == 0 || flag) {
            Parallelism::spawn_thread([v4, vm4] () {
                process_canvas_chords(v4, vm4);
            });
            vm4.clear();
        }
    };

    while (std::cin >> str) {
        auto g = Canvas(CanvasCode(str));
        if (g.l == l) {
            vm.push_back(CanvasCode(str));
        }
        if (g.l == l-1) {
            cnt++;
            vm3.push_back(CanvasCode(str));
        }
        if (g.l == l-2) {
            cnt++;
            vm4.push_back(CanvasCode(str));
        }
        
        if (g.l < l && cnt % REPORT_INTERVAL == 0) {
            process_vec(true);
            Parallelism::wait_for_threads();
            std::cerr << cnt << " " << std::flush;
        }
        else {
            process_vec(false);
        }
    }

    process_vec(true);
    Parallelism::wait_for_threads();
}

void CanvasSearch::read_previous_and_print_w34c_triangles_parallel(int l) {

   for (int i=(int)critical_with_size34_chords.size(); i <= 4; ++i) {
        critical_with_size34_chords.push_back(generate_with_size34_chords(i));
    }

    const int BLOCK_SIZE = 200;
    const int REPORT_INTERVAL = 5;

    vector<CanvasCode> v3;
    vector<CanvasCode> v4;
    for (auto c : critical_with_size34_chords[3]) v3.push_back(c);
    for (auto c : critical_with_size34_chords[4]) v4.push_back(c);

    
    vector<CanvasCode> vm4;
    vector<CanvasCode> vm3;
    vector<CanvasCode> vm;


    auto process_vec = [&] (bool flag) {
        if (vm.size()%BLOCK_SIZE == 0 || flag) {
            Parallelism::spawn_thread([vm] () {
                
                for (auto c : vm) {
                    add_canvas_to_triangles(c);
                }
                
            });
            vm.clear();
        }
        if (vm3.size()%BLOCK_SIZE == 0 || flag) {
            Parallelism::spawn_thread([v3, vm3] () {
                process_canvas_chords(v3, vm3);
            });
            vm3.clear();
        }
        if (vm4.size()%BLOCK_SIZE == 0 || flag) {
            Parallelism::spawn_thread([v4, vm4] () {
                process_canvas_chords(v4, vm4);
            });
            vm4.clear();
        }
    };

    int T;
    std::cin >> T;

    for (int t=0; t < T; ++t) {
        string str;
        std::cin >> str;
        auto g = Canvas(CanvasCode(str));
        if (g.l == l) {
            vm.push_back(CanvasCode(str));
        }
        if (g.l == l-1) {
            vm3.push_back(CanvasCode(str));
        }
        if (g.l == l-2) {
            vm4.push_back(CanvasCode(str));
        }
        
        if (t%((REPORT_INTERVAL*T)/100) == 0) {
            process_vec(true);
            Parallelism::wait_for_threads();
            std::cerr << (100*t)/T << "% " << std::flush;
        }
        else {
            process_vec(false);
        }
    }

    process_vec(true);
    Parallelism::wait_for_threads();
    std::cerr << std::endl;
}

std::set<CanvasCode>& CanvasSearch::get_chordless_code(int l) {
    for (int i=(int)critical_chordless.size(); i <= l; ++i) {
        critical_chordless.push_back(generate_chordless(i));
    }
    return critical_chordless[l];
}

std::set<CanvasCode>& CanvasSearch::get_chordful_code(int l) {
    for (int i=(int)critical_with_chords.size(); i <= l; ++i) {
        critical_with_chords.push_back(generate_with_chords(i));
    }
    return critical_with_chords[l];
}

void CanvasSearch::print_chordless_lessmemory(int l) {
    DFS_MODE = false;
    HALFMEMORY_MODE = false;
    for (int i=(int)critical_chordless.size(); i <= l-2; ++i) {
        critical_chordless.push_back(generate_chordless(i));
    }

    if ((int)critical_chordless.size() <= l-1) {
        HALFMEMORY_MODE = true;
        critical_chordless.push_back(generate_chordless(l-1));
        HALFMEMORY_MODE = false;
    }

    DFS_MODE = true;
    generate_chordless(l);
    DFS_MODE = false;

    #ifdef PARALLEL
    Parallelism::wait_for_threads();
    #endif

    

}

void CanvasSearch::read_previous_and_print_lessmemory() {
    string str;
    int l = 0;
    while (std::cin >> str) {
        Canvas c = Canvas(CanvasCode(str));
        if (c.l == 14) {
            ch.insert(CanvasCode(str).hash());
            std::cout << str << '\n';
        }
        else {
            while((int)critical_chordless.size() < c.l+1) critical_chordless.emplace_back();
            critical_chordless[c.l].insert(CanvasCode(str));
            l = std::max(l, c.l+1);
        }
    }
    print_chordless_lessmemory(l);
}

