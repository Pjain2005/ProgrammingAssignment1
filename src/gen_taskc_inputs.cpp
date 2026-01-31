#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <numeric>

using namespace std;

//makes a random permutation of 1..n using a seed (so it's repeatable)
static vector<int> randPerm(int n, mt19937 &rng) {
    vector<int> v(n);
    iota(v.begin(), v.end(), 1);
    shuffle(v.begin(), v.end(), rng);
    return v;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "usage: gen_taskc_inputs <n> <seed> <output_file>\n";
        return 1;
    }

    int n = stoi(argv[1]);
    unsigned int seed = (unsigned int)stoul(argv[2]);
    string outPath = argv[3];

    ofstream out(outPath);
    if (!out) {
        cerr << "could not open output file\n";
        return 1;
    }

    mt19937 rng(seed);

    //first line is n
    out << n << "\n";

    //next n lines: hospital prefs
    for (int h = 1; h <= n; h++) {
        auto p = randPerm(n, rng);
        for (int i = 0; i < n; i++) {
            if (i) out << " ";
            out << p[i];
        }
        out << "\n";
    }

    //next n lines: student prefs
    for (int s = 1; s <= n; s++) {
        auto p = randPerm(n, rng);
        for (int i = 0; i < n; i++) {
            if (i) out << " ";
            out << p[i];
        }
        out << "\n";
    }

    return 0;
}
