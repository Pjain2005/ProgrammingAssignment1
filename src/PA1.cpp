#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <cstdio>

using namespace std;

// MATCHER FUNCTIONS
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

// VERIFIER FUNCTIONS
bool checkValidity(const map<int, int>& matches, int n) {
    // Check if all hospitals exist and are matched
    if (matches.size() != n) {
        cout << "INVALID - not all hospitals are matched\n";
        return false;
    }
    
    set<int> matchedStudents;
    
    for (const auto& [hospital, student] : matches) {
        // Check if hospital is in valid range
        if (hospital < 1 || hospital > n) {
            cout << "INVALID - hospital " << hospital << " is out of range\n";
            return false;
        }
        
        // Check if student is in valid range
        if (student < 1 || student > n) {
            cout << "INVALID - student " << student << " is out of range\n";
            return false;
        }
        
        // Check for duplicate students
        if (matchedStudents.count(student) > 0) {
            cout << "INVALID - student " << student << " is matched more than once\n";
            return false;
        }
        
        matchedStudents.insert(student);
    }
    
    // Check if all students are matched
    if (matchedStudents.size() != n) {
        cout << "INVALID - not all students are matched\n";
        return false;
    }
    
    return true;
}

bool checkStability(const map<int, int>& matches, const map<int, vector<int>>& hospitalPrefs, 
                    const map<int, vector<int>>& studentPrefs, int n) {
    
    // For each hospital and student pair
    for (int hospital = 1; hospital <= n; hospital++) {
        // Get hospital's current match
        int currentStudentMatch = matches.at(hospital);
        
        // Check each student the hospital prefers
        for (int preferredStudent : hospitalPrefs.at(hospital)) {
            // Stop if we've reached the current match
            if (preferredStudent == currentStudentMatch) break;
            
            // Get the student's current match
            int studentCurrentMatch = -1;
            for (const auto& [h, s] : matches) {
                if (s == preferredStudent) {
                    studentCurrentMatch = h;
                    break;
                }
            }
            
            // Check if the preferred student would prefer this hospital over their current match
            for (int preferredHospital : studentPrefs.at(preferredStudent)) {
                if (preferredHospital == hospital) {
                    cout << "UNSTABLE, Hospital " << hospital 
                         << " and Student " << preferredStudent 
                         << " prefer each other over their current matches.\n";
                    return false;
                }
                // Stop if it's student's current match
                if (preferredHospital == studentCurrentMatch) break;
            }
        }
    }
    return true;
}

int verifyOutput(const string& input, const string& output) {
    ifstream inputFile(input);
    ifstream outputFile(output);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        return -1;
    }

    map<int, int> matches;
    string line;

    while (getline(outputFile, line)) {
        int hospital, student;
        int parseResult = sscanf(line.c_str(), "%d %d", &hospital, &student);

        if (parseResult != 2) {
            // Not the expected format
            cout << "INVALID - line format is incorrect\n";
            return -2;
        }

        if (matches.count(hospital) > 0) {
            // Hospital already matched
            cout << "INVALID - hospital " << hospital << " is matched more than once\n";
            return -2;
        }
        
        matches[hospital] = student;    
    }
    
    // Read number of hospitals/students
    getline(inputFile, line);
    int n = line[0] - '0';

    map<int, vector<int>> hospitalPrefs;
    map<int, vector<int>> studentPrefs; 

    for (int i = 1; i < n + 1; i++) {
        if (!getline(inputFile, line)) {
            cout << "INVALID - not enough lines in input file\n";
            return -2;
        }

        // Parse hospital preferences
        vector<int> prefs;
        istringstream iss(line);

        int pref;
        while (iss >> pref) {
            prefs.push_back(pref);
        }

        hospitalPrefs[i] = prefs;
    }

    for (int i = 1; i < n + 1; i++) {
        if (!getline(inputFile, line)) {
            cout << "INVALID - not enough lines in input file\n";
            return -2;
        }
        
        // Parse student preferences
        vector<int> prefs;
        istringstream iss(line);

        int pref;
        while (iss >> pref) {
            prefs.push_back(pref);
        }

        studentPrefs[i] = prefs;
    }

    // Check validity first
    if (!checkValidity(matches, n)) {
        return -2;
    }

    if (!checkStability(matches, hospitalPrefs, studentPrefs, n)) {
        return -3;
    }

    return 1;
}

// MAIN FUNCTION
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: No command provided.\n" <<
        "Usage: program <match|verify> <input_file> [output_file]\n";
        return -1;
    }
    
    string command = argv[1];
    string input = (argc > 2) ? argv[2] : "";
    string output = (argc == 4) ? argv[3] : "";

    if (command == "match") {
        if (argc != 3) {
            cout << "Error: 'match' command requires exactly 2 arguments.\n";
            return -1;
        }

        // Determine output file name based on input file name
        string outputFile = input;
        size_t lastDot = outputFile.find_last_of(".");
        if (lastDot != string::npos) {
            outputFile = outputFile.substr(0, lastDot) + ".out";
        } else {
            outputFile = outputFile + ".out";
        }
        
        string errorMessage;
        bool ok = runMatcherFromFiles(input, outputFile, errorMessage);
        
        if (!ok) {
            cout << "INVALID (" << errorMessage << ")\n";
            return -1;
        }
        
        cout << "Wrote matches to " << outputFile << "\n";
    
    } else if (command == "verify") {
        if (argc != 4) {
            cout << "Error: 'verify' command requires exactly 3 arguments.\n";
            return -1;
        }

        int result = verifyOutput(input, output);
        if (result == -1) {
            cout << "Error: Could not open one of the files.\n";
            return -1;
        } else if (result == -2) {
            return -1;
        } else if (result == -3) {
            return -1;
        }
        else if (result == 1) {
            cout << "VALID STABLE\n";
        }
    } else {
        cout << "Error: Unknown command '" << command << "'.\n";
        return -1;
    }
    
    return 0;
}