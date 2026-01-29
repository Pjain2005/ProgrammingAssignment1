#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

// [INSERT MATCHING FUNCTION HERE]

int verifyOutput(const string& input, const string& output) {
    ifstream inputFile(input);
    ifstream outputFile(output);
    if (!inputFile.is_open() || !outputFile.is_open()) {
        return -1;
    }

    map<int, int> preferences;

    string line;
    while (getline(outputFile, line)) {
        
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

        if (verifyOutput(input, output) == -1) {
            cout << "Error: Could not open one of the files.\n";
            return -1;
        }

        cout << "Verified file: " << output << endl;
    } else {
        cout << "Error: Unknown command '" << command << "'.\n";
        return -1;
    }
    
    return 0;
}