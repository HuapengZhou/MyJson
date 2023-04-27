//
//  Model.cpp
//  Assignment4
//
//  Created by rick gessner on 2/15/20.
//

#include "Model.hpp"

namespace ECE141 {

	std::ostream& operator <<(std::ostream& anOutput, const ModelCollection& aModelCollect) {
		if (aModelCollect.getSize() == 0) {
			anOutput << "~~Empty~~";
			return anOutput;
		}
		//if aModelCollect[0]->
		//anOutput << "[";
		anOutput << "[";
		if (aModelCollect.getNodes().size() > 1) {
			anOutput << "{";
		}
		for (int i = 0; i < aModelCollect.getNodes().size(); i++) { //ModelNode * n : aModelCollect.getNodes()) {
			//if (i == 0 && !aModelCollect.getNodes()[i]->isConstantOrString()) {

			aModelCollect.getNodes()[i]->print(anOutput);
			//if (i == 0 && !aModelCollect.getNodes()[i]->isConstantOrString()) {
			if (i != aModelCollect.getNodes().size() - 1)
				anOutput << ",";
			//}
		}
		if (aModelCollect.getNodes().size() > 1) {
			anOutput << "}";
		}
		anOutput << "]";
		//anOutput << "]";
		return anOutput;
	}
	ModelQuery::ModelQuery(Model &aModel) : model(aModel) {
		curNode = aModel.getRootNode();
		collectionNodes = new ModelCollection();
		collectionNodes->addOneNode(curNode);
		move = Direction::forward;
	}

	void ModelQuery::backTrack(const std::string& aTarget, ModelNode* curNode, std::vector<ModelNode*>& res) {
		if (curNode->isConstantOrString()) { return; }
		for (ModelNode* nodeInCur : curNode->getNodes()) {
			if (strcmp(nodeInCur->getName().c_str(), aTarget.c_str()) == 0) {
				res.push_back(nodeInCur);
			}
			backTrack(aTarget, nodeInCur, res);
		}
	}

	std::vector<ModelNode*> ModelQuery::searchTarget(std::string aTarget) {
		std::vector<ModelNode*> res;
		backTrack(aTarget, curNode, res);
		return res;
	}

	void ModelQuery::reset() {
		collectionNodes->clear();
		collectionNodes->addNodes(curNode->getNodes());
	}

	ModelQuery &ModelQuery::all(const std::string &aTarget) {
		if (strcmp("*", aTarget.c_str()) == 0) {
			return *this;
		}
		auto tempRes = searchTarget(aTarget);
		if (tempRes.size() == 0) {
			curNode = nullptr;
			collectionNodes->clear();
			return *this;
		}
		curNode = tempRes[0];
		reset();
		return *this;
	}

	ModelQuery &ModelQuery::first(const std::string &aTarget) {
		int firstIndex = 0;
		if (move == Direction::back) { firstIndex = collectionNodes->getSize() - 1; }
		if (strcmp("*", aTarget.c_str()) == 0) {
			std::cout << "first(*)" << std::endl;
			if (curNode->isArray()) {
				curNode = curNode->getNodes()[firstIndex];
			}
			else {
				curNode = collectionNodes->getNodes()[firstIndex];
			}
			return *this;
		}
		else {
			auto res = searchTarget(aTarget);
			if (res.size() == 0) {
				std::cout << "wrong target, cannot search" << std::endl;
				collectionNodes->clear();
				return *this;
			}
			curNode = res[firstIndex];
			collectionNodes->clear();
			collectionNodes->addOneNode(curNode);
			//->getNodes()[0];
		}
		//reset();
		return *this;
	}

	ModelQuery &ModelQuery::last(const std::string &aTarget) {
		move == !move;
		return first(aTarget);
	}

	ModelQuery &ModelQuery::nth(const std::string &aTarget, size_t anIndex) {
		int resIndex = anIndex - 1;
		if (move == Direction::back) {
			resIndex = collectionNodes->getSize() - anIndex;
		}

		if (strcmp("*", aTarget.c_str()) == 0) {
			//std::cout << "nth(" << anIndex << ")" << std::endl;
			if (curNode->isArray()) {
				curNode = curNode->getNodes()[resIndex];
			}
			else {
				if (resIndex >= collectionNodes->getSize()) {
					std::cout << "index out of range" << std::endl;
					return *this;
				}
				curNode = collectionNodes->getNodes()[resIndex];
			}
			reset();
			return *this;
		}
		else {
			auto res = searchTarget(aTarget);
			if (res.size() == 0) {
				std::cout << "wrong target, cannot search" << std::endl;
				return *this;
			}
			curNode = res[resIndex];
		}
		reset();
		return *this;
	}

	ModelQuery &ModelQuery::before(const std::string &aTarget) {
		move = Direction::back;
		if (strcmp("*", aTarget.c_str()) == 0) {
			return *this;
		}
		else {
			std::vector<ModelNode*> findTarget = collectionNodes->getNodes();
			int endIndex = findTarget.size() - 1;
			for (; endIndex >= 0; endIndex--) {
				if (strcmp(findTarget[endIndex]->getName().c_str(), aTarget.c_str()) == 0) {
					break;
				}
			}
			collectionNodes->deleteNodes(endIndex, findTarget.size());
		}
		return *this;
	}

	ModelQuery &ModelQuery::after(const std::string &aTarget) {
		move = Direction::forward;
		if (strcmp("*", aTarget.c_str()) == 0) {
			return *this;
		}
		else {
			std::vector<ModelNode*> findTarget = collectionNodes->getNodes();
			int beginIndex = 0;
			for (; beginIndex <= findTarget.size() - 1; beginIndex++) {
				if (strcmp(findTarget[beginIndex]->getName().c_str(), aTarget.c_str()) == 0) {
					break;
				}
			}
			collectionNodes->deleteNodes(0, beginIndex + 1);
			collectionNodes->deleteNodes(1, collectionNodes->getSize());
		}
		return *this;
	}

	ModelQuery& ModelQuery::within(const std::string& aTarget) {
		int index = 0;
		std::string newTarget = aTarget;
		newTarget.insert(0, "root.");
		return withinDfs(newTarget);
	}

	ModelQuery& ModelQuery::withinDfs(const std::string & aTarget) {
		int index;
		if ((index = aTarget.find(".")) == -1) {
			//          std::cout << aTarget << std::endl;
			for (ModelNode* node : collectionNodes->getNodes()) {
				if (strcmp(node->getName().c_str(), aTarget.c_str()) == 0) {
					curNode = node;
					break;
				}
			}
			reset();
			if (strcmp(curNode->getName().c_str(), "root") == 0) {
				collectionNodes->clear();
				std::cout << "within cannot find" << std::endl;
			}
			else {
				reset();
			}
			//          std::cout << cur->getName() << std::endl;
			//reset();
		}
		else {
			std::string curString = aTarget.substr(0, index);
			for (ModelNode* node : collectionNodes->getNodes()) {
				//std::cout << "curnode:" << node->getName() << std::endl;
				if (strcmp(node->getName().c_str(), curString.c_str()) == 0) {
					curNode = node;
					break;
				}
			}
			reset();
			withinDfs(aTarget.substr(index + 1));
		}
		//      std::cout << "out" << std::endl;
		return *this;
	}

	ModelCollection *ModelQuery::get() {
		return collectionNodes;
	}

	std::string ModelQuery::get(const std::string &aKey) {
		if (strcmp(curNode->getName().c_str(), "root") == 0) {
			collectionNodes->clear();
			collectionNodes->addNodes(curNode->getNodes());
		}
		for (ModelNode* node : collectionNodes->getNodes()) {
			if (strcmp(node->getName().c_str(), aKey.c_str()) == 0) {
				if (node->isArray()) {
					std::string res = "[";

					for (int i = 0; i < node->getNodes().size(); i++) {
						StringNode* cur = static_cast<StringNode*>(node->getNodes()[i]);
						res += cur->getValue();
						if (i != node->getNodes().size() - 1) {
							res += ",";
						}
					}
					res += "]";
					return res;
					//std::cout<< node;
				}
				StringNode* cur = static_cast<StringNode*>(node);
				return cur->getValue();
			}
		}
		//return std::string(""); //retrieve the key from a value if query matches, otherwise return ""
	}

	size_t ModelQuery::count() {
		return collectionNodes->getSize();
	}

	double ModelQuery::avg(const std::string& aField) {
		return sum(aField) / (collectionNodes->getSize());
	}

	double ModelQuery::sum(const std::string &aField) {
		double res = 0.0;
		if (aField == "") {
			if (!curNode->isArray()) {
				std::cout << "the sum method processes a node which is not an array type" << std::endl;
				return res;
			}
			for (ModelNode* node : collectionNodes->getNodes()) {
				StringNode* cur = static_cast<StringNode*>(node);
				res += std::stod(cur->getValue());
			}
		}
		return res;
	}

	//---------------------- implement the model class... --------------------


	Model::Model() {
		rootNode = nullptr;
	}

	Model::Model(const Model &aCommand) {
		//this->rootNode = new ModelNode(*aCommand.rootNode);
		*this = aCommand;
	}

	Model &Model::operator=(const Model &aCommand) {
		this->rootNode = new ModelNode(*aCommand.rootNode);
		this->modelStack = aCommand.modelStack;
		return *this;
	}

	ModelQuery Model::query() {
		ModelQuery theQuery(*this);
		return theQuery;
	}

	ModelNode* Model::getRootNode() const { return rootNode; }
	enum valueType {string, number, boolean};
	// add key-value pair, in which key is the name and value is the string/number/boolean
	// the type only exists in object type 
	valueType typeCheck(const std::string& aValue) {
		if (strcmp(aValue.c_str(), "true") == 0 || strcmp(aValue.c_str(), "false") == 0) {
			return valueType::boolean;
		}
		else { return valueType::string; }
	}
	bool Model::addKeyValuePair(const std::string &aKey, const std::string &aValue) {
		// Print statement for debugging, remove after implementation
		std::cout << "\t|" << aKey << ":" << aValue << "| \n";
		if (modelStack.size() == 0) { return false; }
		if (!modelStack.top()->isObject()) { return false; }
		ModelNode* newNode = new StringNode(aValue, aKey);;
		modelStack.top()->addOneNode(newNode);
		return true;
	}

	// add single string/number/boolean which only exists in array type
	bool Model::addItem(const std::string &aValue) {
		// Print statement for debugging, remove after implementation
		std::cout << "\t" << aValue << " \n";
		if (modelStack.size() == 0) { return false; }
		if (!modelStack.top()->isArray()) { return false; }
		ModelNode* newNode = new StringNode(aValue);
		modelStack.top()->addOneNode(newNode);
		return true;
	}

	bool Model::openContainer(const std::string &aContainerName, ECE141::Element aType) {
		// Print statement for debugging, remove after implementation
		//open a new unnamed container in our model...
		ModelNode* newNode = nullptr;
		if (aType == Element::array) {
			std::cout << aContainerName << " [ \n";
			newNode = new ArrayNode(aContainerName);
		}
		else if (aType == Element::object) {
			std::cout << aContainerName << " { \n";
			newNode = new ObjectNode(aContainerName);
		}
		else {
			return false;
		}
		//std::cout << aContainerName << " { \n";
		//if (aType != Element::object && aType != Element::array) { return false; }
		//ModelNode* newNode = new ModelNode();
		//if (aType == Element::array) { newNode = new ArrayNode(aContainerName); }
		//if (aType == Element::object) { newNode = new ObjectNode(aContainerName); }
		if (modelStack.size() == 0) { 
			rootNode = newNode;
			rootNode->setNodeName("root");
			modelStack.push(newNode); 
			return true; }
		ModelNode* lastNode = modelStack.top();
		if (!lastNode->isArray() && !lastNode->isObject()) { return false; }
		lastNode->addOneNode(newNode);
		modelStack.push(newNode);
		return true;
	}

	bool Model::closeContainer(const std::string &aContainerName, ECE141::Element aType) {
		// Print statement for debugging, remove after implementation
		//std::cout << " } \n";
		if (aType == Element::array) {
			std::cout <<  " ] \n";
		}
		else if (aType == Element::object) {
			std::cout <<  " } \n";
		}
		ModelNode* lastNode = modelStack.top();
		modelStack.pop(); // pop the most top node, the node is the one we want to close
		if (lastNode->getNodeType() != aType) {
			return false;
		}
		else { return true; }
	}
}
