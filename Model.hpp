//
//  Model.hpp
//  Assignment4
//
//  Created by rick gessner on 2/15/20.
//

#ifndef Model_hpp
#define Model_hpp

#include <cstdio>
#include <string>
#include <vector>
#include <optional>
#include <variant>

#include "JSONParser.hpp"


namespace ECE141 {

	class ModelQuery; //forward declare

	//STUDENT: Your Model is built from a bunch of these...
	class ModelNode {
	public:
		ModelNode(){}
		ModelNode(Element type, std::string name = "") : nodeType(type), nodeName(name) {}
		ModelNode(ModelNode& modelNode) : nodeName(modelNode.nodeName), nodeType(modelNode.nodeType) {}

		ModelNode& operator=(const ModelNode& modelNode) {
			nodeName = modelNode.nodeName;
			nodeType = modelNode.nodeType;
			return *this;
		}
		virtual void addOneNode(ModelNode* modelNode) {}
		virtual void print(std::ostream& anOutput) {}
		virtual std::vector<ModelNode*> getNodes() const { return std::vector<ModelNode*>(); }
		bool isArray() const { return nodeType == Element::array; }
		bool isObject() const { return nodeType == Element::object; }
		bool isConstantOrString() const { return nodeType == Element::constant || nodeType == Element::quoted; }
		Element getNodeType() const { return nodeType; }
		std::string getName() const { return nodeName; }
		void setNodeName(std::string newName) { nodeName = newName; }
		//sometimes a node holds a simple value.
		//sometimes a node is named
		//sometimes a node holds a collection of elements (or even other nodes)
		//would subclasses help or hurt here?

	protected:
		Element nodeType;
		std::string nodeName;
	};

	// array type which can nest value
	class ArrayNode : public ModelNode {
	public:
		ArrayNode(std::string name = "") : ModelNode(Element::array, name) {
			}
		ArrayNode& operator=(const ArrayNode& arrayNode) {
			nodeName = arrayNode.nodeName;
			nodeType = arrayNode.nodeType;
			return *this;
		}
		virtual void addOneNode(ModelNode* modelNode) override {
			arrayNodes.push_back(modelNode);
		}
		virtual std::vector<ModelNode*> getNodes() const override { return arrayNodes; }
		void print(std::ostream& anOutput) {
			anOutput << getName() << ":[" << std::endl;
			for (auto n : getNodes()) {
				n->print(anOutput);
				/*if (*n != (*arrayNodes.end())) {
					anOutput << ",";
					std::cout << ",";
				}*/
			}
			std::cout << "]" << std::endl;
		}
	protected:
		std::vector<ModelNode*> arrayNodes;
	};
	
	// object type which can nest key-value
	class ObjectNode : public ModelNode {
	public:
		ObjectNode(std::string name = "") : ModelNode(Element::object, name) {
		}
		ObjectNode& operator=(const ObjectNode& objectNode) {
			nodeName = objectNode.nodeName;
			nodeType = objectNode.nodeType;
			objectNodes = objectNode.objectNodes;
			return *this;
		}
		virtual void addOneNode(ModelNode* modelNode) override {
			objectNodes.push_back(modelNode);
		}
		virtual std::vector<ModelNode*> getNodes() const override { return objectNodes; }
		void print(std::ostream& anOutput) {
			anOutput << "\"" << getName() << "\"" << ":{";
			for (int i = 0; i < getNodes().size(); i++) {
				getNodes()[i]->print(anOutput);
				if (i != getNodes().size() - 1) {
					anOutput << ",";
				}
			}
			/*for (auto n : getNodes()) {
				n->print(anOutput);
			}*/
			anOutput << "}";
			//std::cout << "}";
		}
	protected:
		std::vector<ModelNode*> objectNodes;
	};
	
	// type which can not nest, and has different types: key - value in object and single value in array
	class NumberNode : public ModelNode {
	public:
		NumberNode(double value, std::string numberName = "number") : 
			ModelNode(Element::constant, nodeName = numberName) {}
	};

	class BooleanNode : public ModelNode {
	public:
		BooleanNode(bool value, std::string booleanName = "boolean") :
			ModelNode(Element::constant, nodeName = booleanName) {}
	};

	class StringNode : public ModelNode {
	public:
		StringNode(std::string value, std::string stringName = "") :
			ModelNode(Element::constant, stringName), stringValue(value) {}
		std::string getValue() const {
			return stringValue ;
		}

		bool isNumberOrBool() const {
			return stringValue[0] >= '0' && stringValue[0] <= '9' ||
				strcmp(stringValue.c_str(), "true") == 0 ||
				strcmp(stringValue.c_str(), "false") == 0;
		}
		void print(std::ostream& anOutput) {
			if (getName() != "")
			{
				if (isNumberOrBool())
					anOutput << "\"" << getName() << "\"" << ":" << stringValue;
				else {
					anOutput << "\"" << getName() << "\"" << ":" << "\"" << stringValue << "\"";
				}
			}
			else {
				if (stringValue[0] >= '0' && stringValue[0] <= '9' ||
						strcmp(stringValue.c_str(), "true") == 0 ||
						strcmp(stringValue.c_str(), "false") == 0)
					anOutput << stringValue;
				else
					anOutput << "\"" << stringValue << "\"";
			}
		}
		virtual std::vector<ModelNode*> getNodes() const override { 
			std::vector<ModelNode*>stringVector; 
			stringVector.push_back((ModelNode*)this);
			return stringVector;
		}
	protected:
		std::string stringValue;
	};

	class Model : public JSONListener {
	public:

		Model();

		Model(const Model &aCommand);

		Model &operator=(const Model &aCommand);

		ModelQuery query();

		ModelNode* getRootNode() const ;

	protected:

		virtual bool addKeyValuePair(const std::string &aKey, const std::string &aValue);

		virtual bool addItem(const std::string &aValue);

		virtual bool openContainer(const std::string &aKey, Element aType);

		virtual bool closeContainer(const std::string &aKey, Element aType);

		// STUDENT: Your model will contain a collection of ModelNode*'s
		//          Choose your container(s) wisely

		ModelNode* rootNode;
		std::stack<ModelNode*> modelStack;

	};

	//this is what gets returned when you query a model for a list of elements
	class ModelCollection {
	public:
		//STUDENT: choose an STL container to hold ModelNode* elements from your Model after a query...
		ModelCollection() {}
		size_t getSize() const { return collectionNodes.size(); }
		std::vector<ModelNode*> getNodes() const { return collectionNodes; }
		void addOneNode(ModelNode* newNode) { collectionNodes.push_back(newNode); }
		void addNodes(std::vector<ModelNode*> nodes) { 
			collectionNodes.insert(collectionNodes.end(), nodes.begin(), nodes.end());
		}
		void clear() { collectionNodes.clear(); }
		void deleteNodes(int beginIndex, int endIndex) { 
			collectionNodes.erase(collectionNodes.begin() + beginIndex, collectionNodes.begin() + endIndex); }
		
		std::vector<ModelNode*> getCollections() const { return collectionNodes; }

		friend std::ostream& operator << (std::ostream& anOutput, const ModelCollection& aModelCollect);
	protected:
		std::vector<ModelNode*> collectionNodes;
	};

	class ModelQuery {
	public:

		ModelQuery &all(const std::string &aTarget);

		ModelQuery &first(const std::string &aTarget);

		ModelQuery &last(const std::string &aTarget);

		ModelQuery &nth(const std::string &aTarget, size_t anIndex);

		ModelQuery &before(const std::string &aTarget);

		ModelQuery &after(const std::string &aTarget);

		ModelQuery &within(const std::string &aTarget);

		ModelQuery &withinDfs(const std::string& aTarget);

		size_t count();

		double sum(const std::string &aField);

		double avg(const std::string &aField);

		ModelCollection *get();

		std::string get(const std::string &aKey); // return value or empty string

		ModelQuery(Model &aModel);

		friend class Model;

		Model &model;
	protected:
		enum Direction {forward = 0, back};
		Direction move;
		ModelNode* curNode;
		ModelCollection* collectionNodes;
		std::vector<ModelNode*> searchTarget(std::string aTarget);
		void backTrack(const std::string& aTarget, ModelNode* curNode, std::vector<ModelNode*>& res);
		void reset();
	};


}

#endif /* Model_hpp */
