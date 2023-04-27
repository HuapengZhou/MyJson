//
//  Testing.hpp
//  Assignment4
//
//  Created by rick gessner on 2/16/20.
//

#ifndef Testing_h
#define Testing_h

#include <string>
#include <fstream>
#include "CommandProcessor.hpp"
#include "JSONParser.hpp"
#include "Model.hpp"

namespace ECE141 {
	const bool kNoErrors = true;
	const bool kMatchError = false;

	class Testing {
	public:

		static bool runTests(std::istream &aJSONFile, std::istream &aCmdFile, std::istream &aValidationFile) {
			std::stringstream theOutput;
			Testing theTests(theOutput);
			bool theResult = theTests.run(aJSONFile, aCmdFile);
			if (theResult) {
				std::string theOutputString = theOutput.str();
				theResult = theTests.validateOutput(theOutputString, aValidationFile);
			}
			return theResult;
		}

	
		static bool doParseJSONTest(std::istream &aJSONFile) {
      		  ECE141::JSONParser theParser(aJSONFile);
      		  ECE141::Model theModel;
      		  return theParser.parse(&theModel);
		}
		
	protected:
		static ECE141::Model parseJSONFile(std::istream &aJSONFile) {
			ECE141::JSONParser theParser(aJSONFile);
			ECE141::Model theModel;
			theParser.parse(&theModel);
			return theModel;
		}

		//let's wire up the testing mechanism here.
		//testing involves reading commands from file, and running through command processor...
		bool run(std::istream &aJSONFile, std::istream &aCmdFile) {
			ECE141::Model theModel = parseJSONFile(aJSONFile);
			
			std::string theLine;
			CommandProcessor theProcessor;

			bool isValid = true;
			while (std::getline(aCmdFile, theLine) && isValid) {
				if (theLine.size()) {
					isValid = theProcessor.run(theLine, theModel, output);
				}
			}

			return isValid;
		}

		bool validateOutput(const std::string &anOutput, std::istream &aValidationFile) {
			std::stringstream theOutput(anOutput);
			std::string theOutputLine, theValidationLine;

			while (!aValidationFile.eof() && !theOutput.eof()) {
				theOutput >> theOutputLine;
				aValidationFile >> theValidationLine;

				if (theOutputLine != theValidationLine) {
					std::cout << "Expected: " << theValidationLine
							  << "\nFound: " << theOutputLine << "\n";
					return kMatchError;
				}
			}

			return kNoErrors;
		}

		Testing(std::ostream &anOutput) : output(anOutput) {}

		std::ostream &output;

	};

}

#endif /* Testing_h */
