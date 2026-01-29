#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>

using namespace std;

//reads one preference line and checks it's a permutation of 1..n
bool readPermutationLine(istream &in, int n, vector<int> &line, const string &who, int index, string &errorMessage) {
    vector<int> seen(n + 1, 0); //seen[x]=1 means x already appeared in this line

    for (int pos = 0; pos < n; pos++) {
        int x;
        if (!(in >> x)) {
            errorMessage = "not enough values for " + who + " " + to_string(index);
            return false;
        }
        if (x < 1 || x > n) {
            errorMessage = who + " " + to_string(index) + " has out-of-range value " + to_string(x);
            return false;
        }
        if (seen[x] == 1) {
            errorMessage = who + " " + to_string(index) + " has duplicate value " + to_string(x);
            return false;
        }
        seen[x] = 1;
        line[pos] = x;
    }
    return true;
}

//reads the full preference instance from a file (or stdin), beginner-friendly
bool readPreferencesFromStream(
    istream &in,
    int &n,
    vector<vector<int>> &hospitalPreference,
    vector<vector<int>> &studentPreference,
    string &errorMessage
) {
    if (!(in >> n)) {
        errorMessage = "missing or invalid n";
        return false;
    }
    if (n < 0) {
        errorMessage = "n cannot be negative";
        return false;
    }

    hospitalPreference.assign(n + 1, vector<int>(n));
    studentPreference.assign(n + 1, vector<int>(n));

    for (int h = 1; h <= n; h++) {
        if (!readPermutationLine(in, n, hospitalPreference[h], "hospital", h, errorMessage)) return false;
    }

    for (int s = 1; s <= n; s++) {
        if (!readPermutationLine(in, n, studentPreference[s], "student", s, errorMessage)) return false;
    }

    return true;
}

//this runs hospital-proposing gale-shapley and returns matchHospital where matchHospital[h]=student
vector<int> runMatcher(
    int n,
    const vector<vector<int>> &hospitalPreference,
    const vector<vector<int>> &studentPreference
) {
    //studentRank[s][h]=how much student s likes hospital h (0 best, bigger worse)
    vector<vector<int>> studentRank(n + 1, vector<int>(n + 1, 0));
    for (int s = 1; s <= n; s++) {
        for (int pos = 0; pos < n; pos++) {
            int h = studentPreference[s][pos];
            studentRank[s][h] = pos;
        }
    }

    //nextChoice[h]=index of next student hospital h will propose to
    vector<int> nextChoice(n + 1, 0);

    //matchHospital[h]=student matched to hospital h (0 means free)
    vector<int> matchHospital(n + 1, 0);

    //matchStudent[s]=hospital matched to student s (0 means free)
    vector<int> matchStudent(n + 1, 0);

    //start with all hospitals free
    queue<int> freeHospitals;
    for (int h = 1; h <= n; h++) freeHospitals.push(h);

    while (!freeHospitals.empty()) {
        int h = freeHospitals.front();
        freeHospitals.pop();

        //if h already got matched, skip it (just in case it got re-added earlier)
        if (matchHospital[h] != 0) continue;

        //if h has no one left to propose to, we give up (shouldn't happen with valid input)
        if (nextChoice[h] >= n) continue;

        //h proposes to its next favorite student
        int s = hospitalPreference[h][ nextChoice[h] ];
        nextChoice[h]++;

        //if student is free, easy accept
        if (matchStudent[s] == 0) {
            matchHospital[h] = s;
            matchStudent[s] = h;
        } else {
            int currentHospital = matchStudent[s];

            //student chooses between currentHospital and new h
            if (studentRank[s][h] < studentRank[s][currentHospital]) {
                //student likes the new hospital better, so switch
                matchHospital[h] = s;
                matchStudent[s] = h;

                //old hospital becomes free again
                matchHospital[currentHospital] = 0;
                freeHospitals.push(currentHospital);
            } else {
                //student says nope, so h stays free and tries again later
                freeHospitals.push(h);
            }
        }
    }

    return matchHospital;
}

//writes the matching to a file in the required "h s" per line format
bool writeMatchingToFile(const string &path, int n, const vector<int> &matchHospital) {
    ofstream out(path);
    if (!out) return false;

    for (int h = 1; h <= n; h++) {
        out << h << " " << matchHospital[h] << "\n";
    }
    return true;
}

//this is the "big matcher function" that does everything matcher-related in one call;
//it returns true if it worked, false if something went wrong (and fills errorMessage)
bool runMatcherFromFiles(const string &inputFile, const string &matchingFile, string &errorMessage) {
    ifstream in(inputFile); //try to open the input file
    if (!in) {
        errorMessage = "could not open input file";
        return false;
    }

    int n = 0;
    vector<vector<int>> hospitalPreference, studentPreference;

    //read and validate the preferences from the input file
    if (!readPreferencesFromStream(in, n, hospitalPreference, studentPreference, errorMessage)) {
        //errorMessage already got set inside readPreferencesFromStream
        return false;
    }

    //run gale-shapley and get final matches
    vector<int> matchHospital = runMatcher(n, hospitalPreference, studentPreference);

    //write matching to the output file
    if (!writeMatchingToFile(matchingFile, n, matchHospital)) {
        errorMessage = "could not write matching file";
        return false;
    }

    return true; //everything worked!
}

int main() {
    string inputFile = "data/input.in";
    string matchingFile = "data/example.out";

    string errorMessage;

    //call the big matcher unit
    bool ok = runMatcherFromFiles(inputFile, matchingFile, errorMessage);

    if (!ok) {
        cout << "INVALID (" << errorMessage << ")\n";
        return 0;
    }

    cout << "wrote matching to " << matchingFile << "\n";
    return 0;
}

