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

// [INSERT MATCHING FUNCTION HERE]

bool checkValidity(const map<int, int>& matches, int n) {
    // Check if all hospitals exist and are matched
    if (matches.size() != n) {
        cout << "INVALID\n";
        return false;
    }
    
    set<int> matchedStudents;
    
    for (const auto& [hospital, student] : matches) {
        // Check if hospital is in valid range
        if (hospital < 1 || hospital > n) {
            cout << "INVALID\n";
            return false;
        }
        
        // Check if student is in valid range
        if (student < 1 || student > n) {
            cout << "INVALID\n";
            return false;
        }
        
        // Check for duplicate students
        if (matchedStudents.count(student) > 0) {
            cout << "INVALID\n";
            return false;
        }
        
        matchedStudents.insert(student);
    }
    
    // Check if all students are matched
    if (matchedStudents.size() != n) {
        cout << "INVALID\n";
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
            cout << "INVALID\n";
            return -2;
        }

        if (matches.count(hospital) > 0) {
            // Hospital already matched
            cout << "INVALID\n";
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
            cout << "INVALID\n";
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
            cout << "INVALID\n";
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
        cout << "Matched file: " << input << endl;
        // [CALL MATCH FUNCTION HERE]
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