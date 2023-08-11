#include<iostream>
#include<vector>
#include<string>
#include<set>
#include "Canvas.hh"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

int main() {
    
	std::set<string> v;

    string s;
    while (cin >> s) {
        v.insert(s);
    }

	cout << v.size() << endl;
	for (string s : v) {
		cout << s << endl;
	}
}