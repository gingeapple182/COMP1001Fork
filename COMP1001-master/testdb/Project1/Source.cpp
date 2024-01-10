#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <map>
#include <string>
#include "testdb.h"
#include "studentrecord.h"
using namespace std;

// Main program here



int findArg(int argc, char* argv[], string pattern)
{
    for (int n = 1; n < argc; n++) {
        string s1(argv[n]);
        if (s1 == pattern) {
            return n;
        }
    }
    return 0;
}

vector<int> getSids(ifstream& stream) {
    vector<int> sids;
    string nextLine;

    while (getline(stream, nextLine))
    {
        if (nextLine == " #SID") {
            getline(stream, nextLine);
            string nextSid = regex_replace(nextLine, regex("^ +"), "");
            sids.push_back(stoi(nextSid));
        }

    }

    return sids;
}

int getMatchingSidRow(ifstream& stream, string sidToSearch) {
    int matchingSidRow = 0;
    string nextLine;

    while (getline(stream, nextLine))
    {
        matchingSidRow++;
        if (nextLine == "     " + sidToSearch) {
            return matchingSidRow;
        }
    }

    return -1;
}
void updateRecord(ifstream& stream, const string& sidToUpdate, const string& nameToUpdate,
    const string& phoneToUpdate, const string& moduleToUpdate, const string& gradeToUpdate) {
    vector<string> database; // Store the updated records here

    string nextLine;
    bool recordFound = false;
    bool recordStart = false;

    while (getline(stream, nextLine)) {
        if (nextLine == " #SID") {
            getline(stream, nextLine);
            string nextSid = regex_replace(nextLine, regex("^ +"), "");

            if (nextSid == sidToUpdate) {
                recordFound = true;
                recordStart = true;
                database.push_back(" #SID");
                database.push_back("     " + sidToUpdate);
            }
            else {
                recordStart = false;
            }
        }

        if (recordStart) {
            if (nextLine.find("#RECORD") != string::npos)
            {
                recordStart = false;
            }

            else if (nextLine.find(" #NAME") != string::npos)
            {
                if (!nameToUpdate.empty()) {
                    database.push_back(" #NAME");
                    database.push_back("     " + nameToUpdate);
                    cout << "Updated name: " << nameToUpdate << endl;
                }
                else {
                    cout << "No name provided to update." << endl;
                    database.push_back(nextLine); // Keep the existing name if no update is provided
                }
            }
            else if (nextLine.find(" #PHONE") != string::npos) {
                if (!phoneToUpdate.empty()) {
                    database.push_back(" #PHONE");
                    database.push_back("     " + phoneToUpdate);
                    cout << "Updated phone number: " << phoneToUpdate << endl;
                }
                else {
                    database.push_back(nextLine);
                }
            }
            else {
                database.push_back(nextLine);
            }
        }
    }

    ofstream outputFile("temp.txt");

    for (const string& line : database) {
        outputFile << line << "\n"; // Corrected to use "\n" for new lines
    }

    outputFile.close();
    stream.close();
}



int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Invalid number of arguments. Please provide at least -db and -sid.\n";
        return EXIT_FAILURE;
    }
    string dataBaseFileName;
    string sidToUpdate;



    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-db") {
            if (i + 1 < argc) {
                dataBaseFileName = argv[i + 1];
            }
            else {
                cout << "No database file provided after -db.\n";
                return EXIT_FAILURE;
            }
        }
        else if (string(argv[i]) == "-sid") {
            if (i + 1 < argc) {
                sidToUpdate = argv[i + 1];
            }
            else {
                cout << "No student ID provided after -sid.\n";
                return EXIT_FAILURE;
            }
        }
    }

    if (dataBaseFileName.empty() || sidToUpdate.empty()) {
        cout << "Please provide both database file (-db) and student ID (-sid).\n";
        return EXIT_FAILURE;
    }

    ifstream ip(dataBaseFileName);
    if (!ip.is_open()) {
        cout << "Cannot open file " << dataBaseFileName << " for reading.\n";
        return EXIT_FAILURE;
    }

    string nameToUpdate = "";
    string phoneToUpdate = "";
    string moduleToUpdate = "";
    string gradeToUpdate = "";

    // Parse additional optional arguments
    for (int i = 3; i < argc; ++i) {
        if (string(argv[i]) == "-name" && i + 1 < argc) {
            nameToUpdate = argv[i + 1];
        }
        else if (string(argv[i]) == "-phone" && i + 1 < argc) {
            phoneToUpdate = argv[i + 1];
        }
        else if (string(argv[i]) == "-modulecode" && i + 1 < argc) {
            moduleToUpdate = argv[i + 1];
        }
        else if (string(argv[i]) == "-grade" && i + 1 < argc) {
            gradeToUpdate = argv[i + 1];
        }
    }

    updateRecord(ip, sidToUpdate, nameToUpdate, phoneToUpdate, moduleToUpdate, gradeToUpdate);

    ip.close();

    return EXIT_SUCCESS;
}