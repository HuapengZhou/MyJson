//
//  main.cpp
//  Assignment4
//
//  Created by rick gessner on 2/12/20.
//

#include <iostream>
#include <sstream>
#include <map>
#include <functional>
#include <string>

#include "Testing.hpp"

const bool kFailedTest = false; 

static const std::string getTestFolderPath() {
	//return std::string("/Users/rickg/Code/2023/141a/rickg/PA3/PA3/Testing"); // REMOVE ME!
	return std::string(R"(C:\2023\UCSD_Courses\ECE141A\assignment-3-radioer-main\Testing)"); //change to your local test folder
}

bool doCompileTest(const std::string &aPath) {
	return true;
}

bool doParseTest(const std::string &aPath) {
	std::fstream theJSONFile(aPath + "/basicTest.json");
	if (!theJSONFile.is_open()) {
		std::cout << "Basic JSON filepath incorrect!" << std::endl;
		return kFailedTest;
	}
	return ECE141::Testing::doParseJSONTest(theJSONFile);
}

bool filesAreValid(std::fstream &aJSON, std::fstream &aCmds, std::fstream &aValidate) {
	if (!aJSON.is_open()) {
		std::cout << "JSON filepath incorrect! \n";
		return false;
	}
	if (!aCmds.is_open()) {
		std::cout << "Test commands filepath incorrect!" << std::endl;
		return false;
	}
	if (!aValidate.is_open()) {
		std::cout << "Test validation filepath incorrect!" << std::endl;
		return false;
	}
	return true;
}

bool doBasicTest(const std::string &aPath) {
	std::fstream theJSONFile(aPath + "/basicTest.json");
	std::fstream theCommands(aPath + "/basicTest.txt");
	std::fstream theValidationFile(aPath + "/basicValidation.txt");

	std::cout << aPath + "/basicTest.json" << std::endl;
	if (filesAreValid(theJSONFile, theCommands, theValidationFile)) {
		return ECE141::Testing::runTests(theJSONFile, theCommands, theValidationFile);
	}
	return kFailedTest;
}

bool doNestedTest(const std::string &aPath) {
	std::fstream theJSONFile(aPath + "/nestedTest.json");
	std::fstream theCommands(aPath + "/nestedTest.txt");
	std::fstream theValidationFile(aPath + "/nestedValidation.txt");
	std::stringstream theOutput;

	if (filesAreValid(theJSONFile, theCommands, theValidationFile)) {
		return ECE141::Testing::runTests(theJSONFile, theCommands, theValidationFile);
	}
	return kFailedTest;
}

bool doLongTest(const std::string &aPath) {
	std::fstream theJSONFile(aPath + "/longTest.json");
	std::fstream theCommands(aPath + "/longTest.txt");
	std::fstream theValidationFile(aPath + "/longValidation.txt");
	std::stringstream theOutput;

	if (filesAreValid(theJSONFile, theCommands, theValidationFile)) {
		return ECE141::Testing::runTests(theJSONFile, theCommands, theValidationFile);
	}
	return kFailedTest;
}

//--------------------------------------------

int main(int argc, const char *argv[]) {
	if (argc > 1) {

		std::string theTest(argv[1]);
		std::string thePath(getTestFolderPath());
		if (argc > 2) thePath = argv[2];

		using TestCall = std::function<bool(const std::string &)>;
		static std::map<const std::string, TestCall> theCalls{
				{"Compile", doCompileTest},
				{"Parse",   doParseTest},
				{"Basic",   doBasicTest},
				{"Nested",  doNestedTest},
				{"Long",    doLongTest}
		};

		if (theCalls.count(theTest)) {
			bool theResult = theCalls[theTest](thePath);
			const char *theStatus[] = {"FAIL", "PASS"};
			std::cout << theTest << " test " << theStatus[theResult] << ".\n";
		} else std::cout << "Unknown test\n";
	}

	return 0;
}
