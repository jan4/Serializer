#pragma once

#include <json/json.h>

#include "JsonDeserializer.h"
#include "standardTypes.h"


namespace serializer {
namespace json {

class Serializer;

template<typename T>
class SerializerDefault {
private:
	Serializer&  serializer;
	std::string  name;
	Json::Value& node;
	Json::Value  defaultNode;
	T&           value;
	NodePath     nodePath;

public:
	SerializerDefault(Serializer& _serializer, std::string const& _name, Json::Value& _node, T& _value, NodePath const& _nodePath)
		: serializer { _serializer  }
		, name       { _name }
		, node       { _node }
		, value      { _value }
		, nodePath   { _nodePath }
	{ }
	~SerializerDefault();
	void operator or(T const& t);
};

class SerializerNodeInput {
private:
	Serializer&  serializer;
	std::string  name;
	Json::Value& node;
	NodePath     nodePath;
public:
	SerializerNodeInput(Serializer& _serializer, std::string const& _name, Json::Value& _node, NodePath const& _path);

	template<typename T>
	SerializerDefault<T> operator%(T& t);
};

class SerializerNode {
private:
	Serializer&      serializer;
	std::vector<int> index;
	int              startPoint;
	Json::Value&     node;
	NodePath         nodePath;

public:
	SerializerNode(Serializer& _serialize, Json::Value& _node, NodePath const& _nodePath);
	~SerializerNode();
	SerializerNodeInput operator[](std::string const& _str);
};

struct SerializerAdapter {
	Serializer&  serializer;
	Json::Value& node;
	NodePath     nodePath;
	SerializerAdapter(Serializer& _serializer, Json::Value& _node, NodePath const& _nodePath)
		: serializer { _serializer }
		, node       { _node }
		, nodePath   { _nodePath }
	{}

	template<typename T>
	void serialize(T& _value);
	template<typename Iter>
	void serializeByIter(Iter iter, Iter end);
};


class Serializer {
	Json::Value node;

	SerializerNodeInput rootNode {*this, "", node, {}};

	struct KnownAddress {
		std::vector<NodePath>    bufferPos; // Position where this address was serialized
	};
	std::map<void const*, KnownAddress> knownAddresses;
	struct RawAddress {
		int32_t     ptrId;
	};
	std::map<void const*, RawAddress> rawAddresses;

public:
	void close();

	std::vector<uint8_t> getData() const {
		Json::StyledWriter jsonWriter;
		std::string jsonString = jsonWriter.write(node);
		std::vector<uint8_t> retList(jsonString.size()+1);
		memcpy(retList.data(), jsonString.c_str(), retList.size());
		return retList;
	}
	SerializerNodeInput const& getRootNode() const {
		return rootNode;
	}
	SerializerNodeInput& getRootNode() {
		return rootNode;
	}
	Json::Value const& getNode() const {
		return node;
	}

	void addKnownAddress(void const* _ptr, NodePath const& _bufferPos) {
		knownAddresses[_ptr].bufferPos.push_back(_bufferPos);
	}

	template<typename T, typename std::enable_if<std::is_same<T, bool>::value
	                                             or std::is_same<T, uint8_t>::value
	                                             or std::is_same<T, int8_t>::value
	                                             or std::is_same<T, uint16_t>::value
	                                             or std::is_same<T, int16_t>::value
	                                             or std::is_same<T, uint16_t>::value
	                                             or std::is_same<T, int32_t>::value
	                                             or std::is_same<T, uint32_t>::value
	                                             or std::is_same<T, float>::value
	                                             or std::is_same<T, double>::value>::type* = nullptr>
	void serialize(Json::Value& _node, T const& _value, NodePath const& _nodePath) {
		_node = Json::Value(_value);
		addKnownAddress(&_value, _nodePath);
	}
	template<typename T, typename std::enable_if<std::is_same<T, int64_t>::value>::type* = nullptr>
	void serialize(Json::Value& _node, T& _value, NodePath const& _nodePath) {
		_node = Json::Value::Int64(_value);
		addKnownAddress(&_value, _nodePath);
	}
	template<typename T, typename std::enable_if<std::is_same<T, uint64_t>::value>::type* = nullptr>
	void serialize(Json::Value& _node, T& _value, NodePath const& _nodePath) {
		_node = Json::Value::UInt64(_value);
		addKnownAddress(&_value, _nodePath);
	}
	template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
	void serialize(Json::Value& _node, T& _value, NodePath const& _nodePath) {
		_node = _value;

		addKnownAddress(&_value, _nodePath);
	}

	template<typename T, typename std::enable_if<has_serialize_function<T, SerializerNode>::value>::type* = nullptr>
	void serialize(Json::Value& _node, T& _value, NodePath const& _nodePath) {

		addKnownAddress(&_value, _nodePath);

		SerializerNode node(*this, _node, _nodePath);
		_value.serialize(node);
	}


	template<typename T>
	void serialize(Json::Value& _node, T*& _value, NodePath const& _nodePath) {
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
	void serialize(Json::Value& _node, std::unique_ptr<T>& _value, NodePath const& _nodePath) {
/*		if (_value != nullptr) {
			serialize(int32_t());
		} else {
			serialize(int32_t());
		}*/
	}


	template<typename T, typename std::enable_if<not std::is_fundamental<T>::value
	                                             and not std::is_same<T, std::string>::value
	                                             and not has_serialize_function<T, SerializerNode>::value>::type* = nullptr>
	void serialize(Json::Value& _node, T& _value, NodePath const& _nodePath) {

		addKnownAddress(&_value, _nodePath);
		SerializerAdapter adapter(*this, _node, _nodePath);
		Converter<T>::serialize(adapter, _value);
	}
};
}}


namespace serializer {
namespace json {


template<typename T>
SerializerDefault<T>::~SerializerDefault() {
	if (name == "") {
		serializer.serialize(node, value, nodePath);
		return;
	}

	serializer.serialize(node[name], value, nodePath);

	if (node[name] == defaultNode) {
		node.removeMember(name);
	}
}
template<typename T>
void SerializerDefault<T>::operator or(T const& t) {
	Serializer ser;
	T p = t;
	ser.getRootNode() % p;
	ser.close();
	defaultNode = ser.getNode();
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
	node = Json::Value(Json::arrayValue);
	int32_t index { 0 };
	for (; iter != end; ++iter) {
		auto newNodePath = nodePath;
		newNodePath.push_back(std::to_string(index++));

		Json::Value tnode;
		serializer.serialize(tnode, *iter, newNodePath);
		node.append(tnode);
	}
}

}
}


