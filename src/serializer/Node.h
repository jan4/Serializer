#pragma once

#include <string.h>
#include <string>

#include "NodeImplementation.h"


namespace serializer {

template<typename ...T>
class Converter;

class NodeInput {
private:
	NodeImplementation& implementation;
public:
	NodeInput(NodeImplementation& _implementation)
		: implementation { _implementation } {
	}
	template<typename T>
	void operator%(T& t) {
		if (implementation.getMode() == Mode::Serializing) {
			Converter<T>::serialize(implementation, t);
		} else {
			Converter<T>::deserialize(implementation, t);
		}
	};
};

class Node {
private:
	Node const* parent { nullptr };
	NodeImplementation& implementation;
public:
	Node(NodeImplementation& _implementation);
	Node(NodeImplementation& _implementation, Node const* _parent);
	NodeInput operator[](std::string const& _s);
};





/*	class Node {
	protected:
		Json::Value& value;
		bool serialize;
		Node* parent { nullptr };
		NodePath nodePath;

		Json::Value sharedObjects { Json::arrayValue };
		std::map<void*, int> mappingSharedObjects;
		std::map<int, std::shared_ptr<void>> mappingSharedObjectsRev;

		std::map<void*, NodePath> possibleOwnerlessObjects;

		Json::Value ownerlessObjects { Json::arrayValue };
		std::map<void*, int>               mappingOwnerlessObjects;
		std::map<int, std::vector<void**>> mappingOwnerlessObjectsRev;


	public:
		Node(Json::Value& _value, bool _serialize, Node* _parent, NodePath const& _nodePath)
			: value(_value)
			, serialize(_serialize)
			, parent {_parent}
			, nodePath {_nodePath} {
		}
		NodePath const& getNodePath() const {
			return nodePath;
		}
		void writeSharedObjects();
		void readSharedObjects() {
			if (not value.isObject()) return;
			if (value.isMember("__sharedObjects")) {
				sharedObjects    = value["__sharedObjects"];
			}
			if (value.isMember("__ownerlessObjects")) {
				ownerlessObjects = value["__ownerlessObjects"];
			}
		}
		void readSharedObjectsPost();

		bool isSerializing() const {
			return serialize;
		}
		Json::Value& getValue() { return value; }

		template<typename T>
		int addSharedObject(std::shared_ptr<T>& x);
		template<typename T>
		void getSharedObject(int ptr, std::shared_ptr<T>& x);

		template<typename T>
		int addOwnerlessObject(T*& x);
		template<typename T>
		void getOwnerlessObject(int ptr, T*& x);

		template<typename T>
		void addPossibleOwnerlessObject(T& x, NodePath const& nodePath) {
			if (parent != nullptr) {
				return parent->addPossibleOwnerlessObject(x, nodePath);
			}
			possibleOwnerlessObjects[(void*)&x] = nodePath;
		}

		template<typename T2, typename std::enable_if<std::is_abstract<T2>::value>::type* = nullptr>
		static std::shared_ptr<T2> createNewPointer() {
			return {nullptr};
		}

		template<typename T2, typename std::enable_if<not std::is_abstract<T2>::value>::type* = nullptr>
		static std::shared_ptr<T2> createNewPointer() {
			return std::make_shared<T2>();
		}




		NodeValue operator[](std::string const& _s);
	};



	template<typename T>
	int Node::addSharedObject(std::shared_ptr<T>& x) {
		if (parent != nullptr) {
			return parent->addSharedObject(x);
		}

		// adding shared object
		if (mappingSharedObjects.find(x.get()) == mappingSharedObjects.end()) {
		    int newSize = mappingSharedObjects.size();
			mappingSharedObjects[x.get()] = newSize;
			Json::Value value;
			NodeValue newNode(value, this->isSerializing(), false, this, {});
#ifdef ABUILD_GENERICFACTORY
			if (genericFactory::GenericFactory::getInstance().hasType<T>()) {
				auto typeName = genericFactory::GenericFactory::getInstance().getType(x.get());
				newNode["type"] % typeName;
				newNode["data"] % *x.get();
			} else
#endif
			{
				newNode % *x.get();
			}

			sharedObjects.append(value);
		}
		return mappingSharedObjects[x.get()];
	}

	template<typename T>
	void Node::getSharedObject(int ptrID, std::shared_ptr<T>& x) {
		if (parent != nullptr) {
			return parent->getSharedObject(ptrID, x);
		}
		if (not sharedObjects.isArray()) return;

		if (mappingSharedObjectsRev.find(ptrID) == mappingSharedObjectsRev.end()) {
			//!TODO something is not correct with this path
			NodePath path = nodePath;
			path.push_back(std::to_string(ptrID));
			NodeValue newNode(sharedObjects[ptrID], this->isSerializing(), false, this, path);
#ifdef ABUILD_GENERICFACTORY
			if (genericFactory::GenericFactory::getInstance().hasType<T>()) {
				std::string typeName;
				newNode["type"] % typeName;
				std::shared_ptr<T> ptr = genericFactory::make_shared<T>(typeName);
				newNode["data"] % *ptr.get();
				mappingSharedObjectsRev[ptrID] = std::static_pointer_cast<void, T>(ptr);
			} else
#endif
			{
				std::shared_ptr<T> ptr = createNewPointer<T>();
				if (ptr) {
					newNode % *ptr.get();
				}
				mappingSharedObjectsRev[ptrID] = std::static_pointer_cast<void, T>(ptr);
			}
		}
		x = std::static_pointer_cast<T, void>(mappingSharedObjectsRev[ptrID]);
	}
	template<typename T>
	int Node::addOwnerlessObject(T*& x) {
		if (parent != nullptr) {
			return parent->addOwnerlessObject(x);
		}

		// adding object ref
		if (mappingOwnerlessObjects.find(x) == mappingOwnerlessObjects.end()) {
		    int newSize = mappingOwnerlessObjects.size();
			mappingOwnerlessObjects[x] = newSize;
		}
		return mappingOwnerlessObjects[x];
	}

	template<typename T>
	void Node::getOwnerlessObject(int ptrID, T*& x) {
		if (parent != nullptr) {
			return parent->getOwnerlessObject(ptrID, x);
		}
		if (not ownerlessObjects.isArray()) return;
		mappingOwnerlessObjectsRev[ptrID].push_back(reinterpret_cast<void**>(&x));
	}



	inline NodeValue Node::operator[](std::string const& _s) {
		bool defaultValueNeeded = not value.isMember(_s);
		NodePath path = nodePath;
		path.push_back(_s);
		return NodeValue(value[_s], serialize, defaultValueNeeded, this, path);
	}
*/
}
