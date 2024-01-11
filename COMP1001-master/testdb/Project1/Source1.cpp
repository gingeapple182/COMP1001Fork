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

/*
* Updates an EXISTING user in an existing database file
*
* The user can pass the following parameters to this application:
*
* updaterecord -db <database file> -sid <id> [-name <name>] [-phone <phone-number>] [-modulecode <code> -grade <grade>]
*
* updaterecord
* Without any parameters, this creates an example database file computing.txt (already done for you)
*
* The -db <database file> parameters are ALWAYS required if you wish to update any records.
* If the file does not exist, it should be created (as an empty database)
*
* When -sid parameter is provided, you must also provide an integer <id>.
* The <id> must match a record in the database file.
*
* The following properties are optional, and may be provided to update data for a given student with id <sid>:
*
* o The student name can be updated with the -name tag. The value <name> is a string, composed of at least two words, separated by spaces. This should replace the existing name
*
* o The phone number can be added OR updated by specifying the -phone parameter followed by a new phone number.
*
* o An individual student grade can be added OR updated using the -modulecode and -grade parameters together.
*
* Note that the format of all data items should be consistent with those specified in the previous tasks.
* The same error checking should also apply.
*
* **********************
* *** VALID EXAMPLES ***
* **********************
*
*
* Create an empty database file computing.txt
* updaterecord -db computing.txt
*
* update the phone number of a record with a student ID 12345
* updaterecord -db computing.txt -sid 12345 -phone 00-12-34567
*
* update the name and phone number of a record with a student ID 12345
* updaterecord -db computing.txt -sid 12345 -phone 00-12-34567 -name Jo Kingly Blunte
*
* update (or add) the COMP1001 grade for student with id 12345
* updaterecord -db computing.txt -sid 12345 -modulecode COMP1001 -grade 78.4
*
* Enrol the student in COMP1001 for student with id 12345
* updaterecord -db computing.txt -sid 12345 -modulecode COMP1001
*
* **********************
* *** INVALID EXAMPLES *
* **********************
*
* Examples of invalid parameters include:
*
* Missing the database tag
* updaterecord -sid 12345
*
* ID does not exist (assuming there is no record with ID 99999)
* updaterecord -db computing.txt -sid 99999 -modulecode COMP1001 -grade 78.4
*
* Missing module code
* updaterecord -db computing.txt -sid 12345 -grade 78.4
*
* Spaces in the phone number
* updaterecord -db computing.txt -sid 12345 -phone 00 12 34567 -name Jo Kingly Blunte
*
* Name is missing
* updaterecord -db computing.txt -sid 12345 -phone 00-12-34567 -name
*
* Name must have at least 2 words
* updaterecord -db computing.txt -sid 12345 -phone 00-12-34567 -name Sam
*
* updaterecord -db computing.txt -sid 12345 -name Sammy Jungvirst
*
* *************
* *** NOTES ***
* *************
*
* o The verb update implies you are changing an existing object. The object in this context is a student record.
* o You cannot update a student record that does not exist
* o You cannot add a student record with update command
* o You can update OR add data within an existing record
* o For example, if you update a student phone number and no number is currently held on record,
* then in effect you are adding a phone number.
* o Errors should be communicated with the user
* o No tag should be repeated (e.g. you cannot specify -sid twice)
* o Open files should always be closed before the program exits
* o Code should be consistently indented and commented
* o Marks are awarded for reuse of code from Tasks A and/or B (where possible)
* o Marks are awarded for using Object Orientated Programming to make your code more modular and reusable
*/

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
		if (nextLine == " " + sidToSearch) {
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
				database.push_back(" " + sidToUpdate);
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
					database.push_back(" " + nameToUpdate);
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
					database.push_back(" " + phoneToUpdate);
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