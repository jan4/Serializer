#pragma once

#include <cstring>
#include <functional>
#include <json/json.h>
#include <limits>
#include <memory>

#include "Converter.h"
#include "has_serialize_function.h"

namespace serializer {
namespace json {

using NodePath = std::vector<std::string>;

class Deserializer;

template<typename T>
class DeserializerDefault {
private:
	Deserializer& serializer;
	T&            value;
	Json::Value&  node;
	bool          available;
	bool          defaultValue;
	NodePath      nodePath;

public:
	DeserializerDefault(Deserializer& _serializer, T& _value, Json::Value& _node, bool _available, NodePath const& _nodePath)
		: serializer { _serializer }
		, value      { _value }
		, node       { _node }
		, available  { _available }
		, defaultValue { false }
		, nodePath   { _nodePath }
	{ }
	~DeserializerDefault();

	template<typename T2, typename std::enable_if<std::is_default_constructible<T2>::value>::type* = nullptr>
	void getDefault(T2& _value) const {
		_value = T2();
	}
	template<typename T2, typename std::enable_if<not std::is_default_constructible<T2>::value>::type* = nullptr>
	void getDefault(T2&) const {
		throw std::runtime_error("trying to construct object without default constructor");
	}


	void operator or(T const& t) {
		if (not available) {
			defaultValue = true;
			value = t;
		}
	}
};


class DeserializerNodeInput {
private:
	Deserializer& serializer;
	Json::Value&  node;
	bool          available;
	NodePath      nodePath;
public:
	DeserializerNodeInput(Deserializer& _serializer, Json::Value& _node, bool _available, NodePath const& _nodePath)
		: serializer { _serializer }
		, node       { _node }
		, available  { _available}
		, nodePath   { _nodePath }
	{
	}

	template<typename T>
	DeserializerDefault<T> operator%(T& t);
};

class DeserializerNode {
private:
	Deserializer&    serializer;
	std::vector<int> index;
	Json::Value&     node;
	NodePath         nodePath;

public:
	DeserializerNode(Deserializer& _serializer, Json::Value& _node, bool _available, NodePath const& _nodePath);
	~DeserializerNode();
	DeserializerNodeInput operator[](std::string const& _str);
};

struct DeserializerAdapter {
	Deserializer& serializer;
	Json::Value&  node;
	NodePath      nodePath;
	DeserializerAdapter(Deserializer& _serializer, Json::Value& _node, NodePath const& _nodePath)
		: serializer { _serializer }
		, node       { _node }
		, nodePath   { _nodePath }
	{}

	template<typename T>
	void deserialize(T& _value);
	template<typename T>
	void deserializeByInsert(std::function<void(T const& v)> _func);
};


class Deserializer {
	Json::Value node;

	DeserializerNodeInput rootNode {*this, node, true, {}};

	struct KnownAddress {
		void const* ptr;
		NodePath    bufferPos; // Position where this address was serialized
	};
	std::map<NodePath, KnownAddress> knownAddresses;
	struct RawAddress {
		std::vector<void*> needOverwrite;
	};
	std::map<int32_t, RawAddress> rawAddresses;

	Json::Value sharedObjectNode { Json::arrayValue };
	std::map<int32_t, std::shared_ptr<void>> idToShared;

public:
	Deserializer(std::vector<uint8_t> const& _data);
	Deserializer(std::string const& _data);

	void close();

	DeserializerNodeInput const& getRootNode() const {
		return rootNode;
	}
	DeserializerNodeInput& getRootNode() {
		return rootNode;
	}

	void addKnownAddress(void const* _ptr, NodePath const& _bufferPos) {
		knownAddresses[_bufferPos] = {_ptr, _bufferPos};
	}

	template<typename T>
	void getSharedObject(int32_t _ptrId, std::shared_ptr<T>& _value) {
		if (idToShared.find(_ptrId) == idToShared.end()) {
			std::shared_ptr<T> value = std::make_shared<T>();
			deserialize(sharedObjectNode[_ptrId], *value.get(), {"__sharedObjects"});
			idToShared[_ptrId] = value;
		}
		_value = std::static_pointer_cast<T, void>(idToShared.at(_ptrId));
	}


	Json::Value& getNode() { return node; }

	void deserialize(Json::Value& _node, bool& _value, NodePath const& _nodePath) {
		if (not _node.isBool()) throw std::runtime_error("is not a boolean: " + _node.toStyledString());
		_value = _node.asBool();
		addKnownAddress(&_value, _nodePath);
	}

	void deserialize(Json::Value& _node, uint8_t& _value, NodePath const& _nodePath) {
		if (not _node.isUInt()) throw std::runtime_error("expected uint8_t: " + _node.toStyledString());
		uint32_t t = _node.asUInt();
		if (t > std::numeric_limits<uint8_t>::max()) throw std::runtime_error(std::to_string(t) + " outside of uint8_t range: " + _node.toStyledString());
		_value = t;
		addKnownAddress(&_value, _nodePath);
	}

	void deserialize(Json::Value& _node, int8_t& _value, NodePath const& _nodePath) {
		if (not _node.isInt()) throw std::runtime_error("expected uint8_t: " + _node.toStyledString());
		int32_t t = _node.asInt();
		if (t > std::numeric_limits<int8_t>::max()
		    or t < std::numeric_limits<int8_t>::min()) throw std::runtime_error(std::to_string(t) + " outside of int8_t range: " + _node.toStyledString());
		_value = t;
		addKnownAddress(&_value, _nodePath);
	}

	void deserialize(Json::Value& _node, uint16_t& _value, NodePath const& _nodePath) {
		if (not _node.isUInt()) throw std::runtime_error("expected uint16_t: " + _node.toStyledString());
		uint32_t t = _node.asUInt();
		if (t > std::numeric_limits<uint16_t>::max()) throw std::runtime_error(std::to_string(t) + " outside of uint16_t range: " + _node.toStyledString());
		_value = t;
		addKnownAddress(&_value, _nodePath);
	}

	void deserialize(Json::Value& _node, int16_t& _value, NodePath const& _nodePath) {
		if (not _node.isInt()) throw std::runtime_error("expected int16_t: " + _node.toStyledString());
		int32_t t = _node.asInt();
		if (t > std::numeric_limits<int16_t>::max()) throw std::runtime_error(std::to_string(t) + " outside of int16_t range: " + _node.toStyledString());
		_value = t;
		addKnownAddress(&_value, _nodePath);
	}

	void deserialize(Json::Value& _node, uint32_t& _value, NodePath const& _nodePath) {
		if (not _node.isUInt()) throw std::runtime_error("expected uint32_t: " + _node.toStyledString());
		_value = _node.asUInt();
		addKnownAddress(&_value, _nodePath);
	}

	void deserialize(Json::Value& _node, int32_t& _value, NodePath const& _nodePath) {
		if (not _node.isInt()) throw std::runtime_error("expected int32_t: " + _node.toStyledString());
		_value = _node.asInt();
		addKnownAddress(&_value, _nodePath);
	}

	void deserialize(Json::Value& _node, uint64_t& _value, NodePath const& _nodePath) {
		if (not _node.isUInt64()) throw std::runtime_error("expected uint64_t: " + _node.toStyledString());
		_value = _node.asUInt64();
		addKnownAddress(&_value, _nodePath);
	}
	void deserialize(Json::Value& _node, int64_t& _value, NodePath const& _nodePath) {
		if (not _node.isInt64()) throw std::runtime_error("expected int64_t: " + _node.toStyledString());
		addKnownAddress(&_value, _nodePath);
		_value = _node.asInt64();
	}

	void deserialize(Json::Value& _node, float& _value, NodePath const& _nodePath) {
		if (not _node.isDouble()) throw std::runtime_error("expected float (double): " + _node.toStyledString());
		addKnownAddress(&_value, _nodePath);
		_value = _node.asFloat();
	}

	void deserialize(Json::Value& _node, double& _value, NodePath const& _nodePath) {
		if (not _node.isDouble()) throw std::runtime_error("expected double: " + _node.toStyledString());
		addKnownAddress(&_value, _nodePath);
		_value = _node.asDouble();
	}
	void deserialize(Json::Value& _node, std::string& _value, NodePath const& _nodePath) {
		if (not _node.isString()) throw std::runtime_error("expected string: " + _node.toStyledString());
		addKnownAddress(&_value, _nodePath);
		_value = _node.asString();
	}


	template<typename T, typename std::enable_if<has_serialize_function<T, DeserializerNode>::value>::type* = nullptr>
	void deserialize(Json::Value& _node, T& _value, NodePath const& _nodePath) {
		addKnownAddress(&_value, _nodePath);

		DeserializerNode node(*this, _node, true, _nodePath);
		_value.serialize(node);
	}

	template<typename T>
	void deserialize(Json::Value& _node, T*& _value, NodePath const& _nodePath) {
		int32_t ptrId;
		deserialize(_node, ptrId, _nodePath);
		_value = nullptr;
		rawAddresses[ptrId].needOverwrite.push_back((void*)&_value);
		addKnownAddress(&_value, _nodePath);

	}

	template<typename T>
	void deserialize(Json::Value& _node, std::shared_ptr<T>& _value, NodePath const& _nodePath) {
		_value.reset();

		int32_t ptrId;
		deserialize(_node, ptrId, _nodePath);
		if (ptrId >= 0) {
			getSharedObject(ptrId, _value);
		}
	}

	template<typename T, typename std::enable_if<not std::is_fundamental<T>::value
	                                             and not std::is_same<T, std::string>::value
	                                             and not has_serialize_function<T, DeserializerNode>::value>::type* = nullptr>
	void deserialize(Json::Value& _node, T& _value, NodePath const& _nodePath) {
		addKnownAddress(&_value, _nodePath);

		DeserializerAdapter adapter(*this, _node, _nodePath);
		Converter<T>::deserialize(adapter, _value);

	}

};

template<typename T>
DeserializerDefault<T>::~DeserializerDefault() {
	if (not available and not defaultValue) {
		getDefault<T>(value);
	} else if (available) {
		serializer.deserialize(node, value, nodePath);
	}
}

template<typename T>
DeserializerDefault<T> DeserializerNodeInput::operator%(T& t) {
	return DeserializerDefault<T>(serializer, t, node, available, nodePath);
}

template<typename T>
void DeserializerAdapter::deserialize(T& _value) {
	serializer.deserialize(node, _value, nodePath);
}

template<typename T>
void DeserializerAdapter::deserializeByInsert(std::function<void(T const& v)> _func) {
	if (not node.isArray()) throw std::runtime_error("expected array: " + node.toStyledString());
	int32_t index { 0 };
	for (auto& v : node) {
		T value;
		NodePath newNodePath = nodePath;
		newNodePath.push_back(std::to_string(index++));
		serializer.deserialize(v, value, newNodePath);
		_func(value);
	}
}



}
}

