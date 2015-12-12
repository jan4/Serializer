#pragma once

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>

#include "Deserializer.h"
#include <serializer/standardTypes.h>

#ifdef ABUILD_GENERICFACTORY
	#include <genericFactory/genericFactory.h>
#endif

#include <serializer/is_copy_constructible_recursive.h>

namespace serializer {
namespace yaml {

class Serializer;

template<typename T>
class SerializerDefault {
private:
	Serializer&  serializer;
	std::string  name;
	YAML::Node&  node;
	YAML::Node   defaultNode;

	bool         defaultValueGiven;
	T&           value;
	NodePath     nodePath;

public:
	SerializerDefault(Serializer& _serializer, std::string const& _name, YAML::Node& _node, T& _value, NodePath const& _nodePath)
		: serializer ( _serializer  )
		, name       { _name }
		, node       ( _node )
		, defaultValueGiven { false }
		, value      ( _value )
		, nodePath   { _nodePath }
	{ }
	~SerializerDefault();

	void operator or(T const& t) {
		setDefault(t);
	}

	template<typename T2, typename std::enable_if<is_copy_constructible_recursive<T2>::value>::type* = nullptr>
	void setDefault(T2 const& t);

	template<typename T2, typename std::enable_if<not is_copy_constructible_recursive<T2>::value>::type* = nullptr>
	void setDefault(T2 const& t);

	template<typename T2, typename std::enable_if<std::is_default_constructible<T2>::value
	                                              and std::is_assignable<T2, T2>::value>::type* = nullptr>
	void setDefault() {
		*this or T2();
	}
	template<typename T2, typename std::enable_if<not std::is_default_constructible<T2>::value
	                                              or not std::is_assignable<T2, T2>::value>::type* = nullptr>
	void setDefault() {
	}

};

class SerializerNodeInput {
private:
	Serializer&  serializer;
	std::string  name;
	YAML::Node&  node;
	NodePath     nodePath;
public:
	SerializerNodeInput(Serializer& _serializer, std::string const& _name, YAML::Node& _node, NodePath const& _path);

	template<typename T>
	SerializerDefault<T> operator%(T& t);
};

class SerializerNode {
private:
	Serializer&      serializer;
	std::vector<int> index;
	YAML::Node&      node;
	NodePath         nodePath;

public:
	SerializerNode(Serializer& _serialize, YAML::Node& _node, NodePath const& _nodePath);
	~SerializerNode();
	SerializerNodeInput operator[](std::string const& _str);
};

struct SerializerAdapter {
	Serializer&  serializer;
	YAML::Node&  node;
	NodePath     nodePath;
	SerializerAdapter(Serializer& _serializer, YAML::Node& _node, NodePath const& _nodePath)
		: serializer ( _serializer )
		, node       ( _node )
		, nodePath   { _nodePath }
	{}

	template<typename T>
	void serialize(T& _value);

	template<typename Iter>
	void serializeByIter(Iter iter, Iter end);

	template<typename Iter>
	void serializeByIterCopy(Iter iter, Iter end);

};


class Serializer {
	YAML::Node  node;

	SerializerNodeInput rootNode {*this, "", node, {}};

	struct KnownAddress {
		std::vector<NodePath>    bufferPos; // Position where this address was serialized
	};
	std::map<void const*, KnownAddress> knownAddresses;
	struct RawAddress {
		int32_t     ptrId;
	};
	std::map<void const*, RawAddress> rawAddresses;

	YAML::Node  sharedObjectNode; //!TODO needs initialization?

	std::map<void const*, int32_t> sharedToId;

public:
	Serializer() {
		//node //!TODO needs initialization?
	}
	void close();

	std::vector<uint8_t> getData() const {
		YAML::Emitter emitter;
		emitter.SetFloatPrecision(std::numeric_limits<float>::digits10+2);
		emitter.SetDoublePrecision(std::numeric_limits<double>::digits10+2);
		emitter << node;
		emitter.c_str();
		std::vector<uint8_t> retList(emitter.size()+1);
		memcpy(retList.data(), emitter.c_str(), retList.size());
		return retList;
	}

	std::string getDataAsStr() const {
		YAML::Emitter emitter;
		emitter << node;
		return emitter.c_str();
	}

	SerializerNodeInput const& getRootNode() const {
		return rootNode;
	}
	SerializerNodeInput& getRootNode() {
		return rootNode;
	}
	YAML::Node const& getNode() const {
		return node;
	}

	void addKnownAddress(void const* _ptr, NodePath const& _bufferPos) {
		knownAddresses[_ptr].bufferPos.push_back(_bufferPos);
	}

	template<typename T>
	int32_t addSharedObject(std::shared_ptr<T>& _value) {
		if (sharedToId.find(_value.get()) == sharedToId.end()) {
			int32_t id = sharedObjectNode.size();
			sharedToId[_value.get()] = id;

			{ //Yaml conversion
				sharedObjectNode.push_back(YAML::Node{});
				YAML::Node value;

				// Extrem hacky!!! casting shared object to unique to use it's serialization
				std::unique_ptr<T> ptr (_value.get());
				serialize(value, ptr, {"__sharedObjects"});
				ptr.release();

				sharedObjectNode[id] = value;

			}

		}
		return sharedToId.at(_value.get());
	}

	template<typename T, typename std::enable_if<std::is_same<T, uint8_t>::value
	                                             or std::is_same<T, int8_t>::value>::type* = nullptr>
	void serialize(YAML::Node& _node, T const& _value, NodePath const& _nodePath) {
		_node = YAML::Node((int16_t)_value);
		addKnownAddress(&_value, _nodePath);
	}

	template<typename T, typename std::enable_if<std::is_same<T, bool>::value
	                                             or std::is_same<T, uint16_t>::value
	                                             or std::is_same<T, int16_t>::value
	                                             or std::is_same<T, uint16_t>::value
	                                             or std::is_same<T, int32_t>::value
	                                             or std::is_same<T, uint32_t>::value
	                                             or std::is_same<T, int64_t>::value
	                                             or std::is_same<T, uint64_t>::value
	                                             or std::is_same<T, float>::value
	                                             or std::is_same<T, double>::value>::type* = nullptr>
	void serialize(YAML::Node& _node, T const& _value, NodePath const& _nodePath) {
		_node = YAML::Node(_value);
		addKnownAddress(&_value, _nodePath);
	}
	template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
	void serialize(YAML::Node& _node, T& _value, NodePath const& _nodePath) {
		_node = _value;

		addKnownAddress(&_value, _nodePath);
	}

	template<typename T, typename std::enable_if<has_serialize_function<T, SerializerNode>::value>::type* = nullptr>
	void serialize(YAML::Node& _node, T& _value, NodePath const& _nodePath) {

		addKnownAddress(&_value, _nodePath);

		SerializerNode node(*this, _node, _nodePath);

#ifndef ABUILD_GENERICFACTORY
		_value.serialize(node);
#else
		if (genericFactory::hasType(&_value)) {
			genericFactory::serialize(&_value, node);
		} else {
			_value.serialize(node);
		}
#endif
	}


	template<typename T>
	void serialize(YAML::Node& _node, T*& _value, NodePath const& _nodePath) {
		auto iter = rawAddresses.find(_value);
		if (iter == rawAddresses.end()) {
			int32_t ptrId = rawAddresses.size();
			rawAddresses[_value] = {ptrId};
			iter = rawAddresses.find(_value);
		}
		int32_t ptrId = iter->second.ptrId;
		serialize(_node, ptrId, _nodePath);
		addKnownAddress(&_value, _nodePath);
	}

	template<typename T>
	void serialize(YAML::Node& _node, std::shared_ptr<T>& _value, NodePath const& _nodePath) {
		int32_t ptrId = {-1};
		if (_value.get() != nullptr) {
			ptrId = addSharedObject(_value);
		}
		serialize(_node, ptrId, _nodePath);
	}

	template<typename T, typename std::enable_if<not std::is_fundamental<T>::value
	                                             and not std::is_same<T, std::string>::value
	                                             and not has_serialize_function<T, SerializerNode>::value>::type* = nullptr>
	void serialize(YAML::Node& _node, T& _value, NodePath const& _nodePath) {

		addKnownAddress(&_value, _nodePath);
		SerializerAdapter adapter(*this, _node, _nodePath);
		Converter<T>::serialize(adapter, _value);
	}
};

template<typename T>
void read(std::string const& _file, T& _value);

template<typename T>
void write(std::string const& _file, T& _value);

template<typename T>
SerializerDefault<T>::~SerializerDefault() {
	if (name == "") {
		serializer.serialize(node, value, nodePath);
		return;
	}

	auto tnode = node[name];
	serializer.serialize(tnode, value, nodePath);
	if (not defaultValueGiven) {
		setDefault<T>();
	}

	if (defaultValueGiven and node[name] == defaultNode) {
		node.remove(name);
	}
}
template<typename T>
template<typename T2, typename std::enable_if<is_copy_constructible_recursive<T2>::value>::type*>
void SerializerDefault<T>::setDefault(T2 const& t) {
	try {
		Serializer ser;
		T p (t);
		ser.getRootNode() % p;
		ser.close();
		defaultNode = ser.getNode();
		defaultValueGiven = true;
	} catch (...) {
	}
}

template<typename T>
template<typename T2, typename std::enable_if<not is_copy_constructible_recursive<T2>::value>::type*>
void SerializerDefault<T>::setDefault(T2 const&) {
}



template<typename T>
SerializerDefault<T> SerializerNodeInput::operator%(T& t) {
	return SerializerDefault<T>(serializer, name, node, t, nodePath);
}

template<typename T>
void SerializerAdapter::serialize(T& _value) {
	serializer.serialize(node, _value, nodePath);
}

template<typename Iter>
void SerializerAdapter::serializeByIter(Iter iter, Iter end) {
	//node = Json::Value(Json::arrayValue);
	//!TODO node should be initialized as array?
	int32_t index { 0 };
	for (; iter != end; ++iter) {
		auto newNodePath = nodePath;
		newNodePath.push_back(std::to_string(index++));

		YAML::Node tnode;
		serializer.serialize(tnode, *iter, newNodePath);
		node.push_back(tnode);
	}
}

template<typename Iter>
void SerializerAdapter::serializeByIterCopy(Iter iter, Iter end) {
	//node = Json::Value(Json::arrayValue);
	//!TODO node should be initalized as array?
	int32_t index { 0 };
	for (; iter != end; ++iter) {
		auto newNodePath = nodePath;
		newNodePath.push_back(std::to_string(index++));

		YAML::Node tnode;
		auto t = *iter;
		serializer.serialize(tnode, t, newNodePath);
		node.push_back(tnode);
	}
}


template<typename T>
void read(std::string const& _file, T& _value) {

	// Read file from storage
	std::ifstream ifs(_file);
	if (ifs.fail()) {
		throw std::runtime_error("Opening file failed");
	}
	std::stringstream strStream;
	strStream << ifs.rdbuf();

	// parse file in serializer
	Deserializer serializer(strStream.str());
	serializer.getRootNode() % _value;
	serializer.close();
}

template<typename T>
void write(std::string const& _file, T& _value) {
	// Serialize data
	Serializer serializer;
	serializer.getRootNode() % _value;
	serializer.close();

	std::ofstream oFile(_file);
	if (oFile.fail()) {
		throw std::runtime_error("Opening file failed");
	}
	oFile << serializer.getDataAsStr();
	oFile.close();
	if (oFile.fail()) {
		throw std::runtime_error("Writing to file failed");
	}
}


}
}


