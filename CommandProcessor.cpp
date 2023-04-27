//
//  CommandProcessor.cpp
//  Assignment4
//
//  Created by rick gessner on 2/15/20.
//

#include "CommandProcessor.hpp"

namespace ECE141 {

	CommandProcessor::CommandProcessor() {}

	CommandProcessor::CommandProcessor(const CommandProcessor& aCommand) { *this = aCommand; }

	CommandProcessor &CommandProcessor::operator=(const CommandProcessor &aCommand) {
		//return *this = aCommand;
		return *this;
	}

	bool CommandProcessor::run(std::string &aCommand, Model &aModel, std::ostream &anOutput) {
		//STUDENT -- Run your query and send your output to the output stream ...
		//anOutput << aCommand << "\n";
		ModelQuery runningQuery = aModel.query();
		std::string newCommands = aCommand; // deep copy
		int left = 0;
		int right = 0;
		int mid = 0;
		while ((right = newCommands.find(" ")) != -1) {
			std::string newCommand = newCommands.substr(left,right);
			newCommands = newCommands.substr(right + 1);

			mid = newCommand.find("(");
			std::string function = newCommand.substr(left, mid - left);
			std::string stringParam = newCommand.substr(mid + 1, newCommand.find(")") - mid - 1);
			if (stringParam[0] == '"') {
				stringParam = stringParam.substr(1, stringParam.length() - 2);
			}
			if (strcmp(function.c_str(), "within") == 0) {
				runningQuery.within(stringParam);
			}
			else if (strcmp(function.c_str(), "before") == 0) {
				runningQuery.before(stringParam);
			}
			else if (strcmp(function.c_str(), "after") == 0) {
				runningQuery.after(stringParam);
			}
			else if (strcmp(function.c_str(), "all") == 0) {
				runningQuery.all(stringParam);
			}
			else if (strcmp(function.c_str(), "first") == 0) {
				runningQuery.first(stringParam);
			}
			else if (strcmp(function.c_str(), "last") == 0) {
				runningQuery.last(stringParam);
			}
			else if (strcmp(function.c_str(), "nth") == 0) {
				runningQuery.nth("*", std::stoi(stringParam));
			}
			else {
				return false;
			}
		}
		// the last count / sum / avg
		std::string endCommand = newCommands.substr(0, newCommands.find("("));
		std::string getParam = "";
		if (newCommands.find("\"") != -1) {
			getParam = newCommands.substr(newCommands.find("\"") + 1, newCommands.find_last_of("\"") - (newCommands.find("\"") + 1));
		}
		if (strcmp(endCommand.c_str(), "count") == 0) {
			anOutput << runningQuery.count() << "\n";
			std::cout << runningQuery.count() << std::endl;
		}
		else if (strcmp(endCommand.c_str(), "sum") == 0) {
			std::cout << runningQuery.sum("") << std::endl;
			anOutput << runningQuery.sum("") << "\n";
		}
		else if (strcmp(endCommand.c_str(), "avg") == 0) {
			std::cout << runningQuery.avg("") << std::endl;
			anOutput << runningQuery.avg("") << "\n";
		}
		else if (strcmp(endCommand.c_str(), "get") == 0) {
			if (strcmp(getParam.c_str(), "") == 0) {
				ModelCollection* collection = runningQuery.get();
				//for (int i = 0; i < collection->getSize(); i++) {
				//	//std::cout << static_cast<StringNode*>(collection->getNodes()[i])->getValue();
				//	std::cout << collection->getNodes()[i]->getValue();
				//}
				//std::cout << std::endl;
				std::cout << (*collection)<< std::endl;
				anOutput << (*collection) << "\n";
			}
			else {
				std::string results = runningQuery.get(getParam);
				
				if (results[0] == '[') {
					std::cout << results << std::endl;
					anOutput << results << "\n";
				}
				else {
					std::cout << "\"" << results << "\"" << std::endl;
					anOutput << "\"" << results << "\"" << "\n";
				}
				
			}
		}
		//anOutput << "Write your results here..." << "\n";
		return true;
	}

}
