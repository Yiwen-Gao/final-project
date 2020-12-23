#include "conn.h"

using namespace std;

string vec_to_str(vector<string> &vec) {
    string output = "";
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        output += *it + "\n";
    } 

    return output;
}

vector<string> str_to_vec(string &str) {
    vector<string> output;
    istringstream input(str);
    string line;
    while (getline(input, line)) {
        output.push_back(line);
    }

    return output;
}
