#pragma once

#include "BinaryDeserializer.h"

namespace serializer {

namespace binary {

class Serializer;

template<typename T>
class SerializerDefault {
private:
	Serializer& serializer;
	int32_t     id;
	T&          value;
	bool        needToKnowAddress;

public:
	SerializerDefault(Serializer& _serializer, int32_t _id, T& _value, bool _needToKnowAddress)
		: serializer { _serializer  }
		, id         { _id }
		, value      { _value }
		, needToKnowAddress { _needToKnowAddress }
	{
	}
	~SerializerDefault();
	void operator or(T const& t) {
	}
};

class SerializerNodeInput {
private:
	Serializer& serializer;
	int32_t     id;
	bool        needToKnowAddress;
public:
	SerializerNodeInput(Serializer& _serializer, int32_t _id, bool _needToKnowAddress);

	template<typename T>
	SerializerDefault<T> operator%(T& t);
};


class SerializerNode {
private:
	Serializer&      serializer;
	std::vector<int> index;
	int              startPoint;
	bool             needToKnowAddress;
public:
	SerializerNode(Serializer& _serializer, bool _needToKnowAddress);
	~SerializerNode();
	SerializerNodeInput operator[](std::string const& _str);
};

struct SerializerAdapter {
	Serializer& serializer;
	bool needToKnowAddress;

	SerializerAdapter(Serializer& _serializer, bool _needToKnowAddress)
		: serializer        { _serializer }
		, needToKnowAddress { _needToKnowAddress }
	{}

	template<typename T>
	void serialize(T& _value);
	template<typename Iter>
	void serializeByIter(Iter iter, Iter end);
};


class Serializer {
private:
	std::map<std::string, int32_t> stringToInt;

	std::vector<uint8_t> buffer;
	int                  currentPosition { 0 };

	int                  startPoint;
	SerializerNodeInput  rootNode {*this, -1, true};
	struct KnownAddress {
		void const* ptr;
		int32_t     count;
		int32_t     size;
		std::type_info const& type_info;
		int32_t     bufferPos; // Position where this address was serialized
	};
	std::vector<KnownAddress> knownAddresses;
	struct RawAddress {
		void const* ptr;       // Pointing to
		std::type_info const& type_info;
		int32_t     bufferPos; // position in serialized buffer
	};
	std::vector<RawAddress> rawAddresses;
public:
	Serializer() {
		// serialize number which jumps directly to lookup table of string to int32
		serialize(int32_t(), false);

		startPoint = getCurrentPosition();
	}

	void close() {
		// Fill all rawpointers
		for (auto const& raw : rawAddresses) {
			for (auto const& known : knownAddresses) {
				if (raw.type_info != known.type_info) continue;
				if (raw.ptr < known.ptr) continue;
				if (raw.ptr >= (uint8_t*)known.ptr + (known.size*known.count)) continue;

				int32_t offset = 0;
				for (; offset < known.count*known.size; offset += known.size) {
					if ((uint8_t*)known.ptr + offset == raw.ptr) {
						break;
					}
				}
				int32_t value = known.bufferPos + offset;
				memcpy(&buffer[raw.bufferPos], &value, sizeof(known.bufferPos));
				break;
			}
		}


		// Write string index to the end
		int endPoint = getCurrentPosition();
		int32_t size = endPoint - startPoint;
		memcpy(&buffer[startPoint - sizeof(int32_t)], &size, sizeof(size));

		// serialize string to int
		int32_t stringSize = int32_t(stringToInt.size());
		serialize(stringSize, false);
		for (auto const& e : stringToInt) {
			serialize(e.second, false);
			serialize(e.first, false);
		}
	}

	SerializerNodeInput const& getRootNode() const {
		return rootNode;
	}
	SerializerNodeInput& getRootNode() {
		return rootNode;
	}


	uint8_t* getPtr() {
		return &buffer[0];
	}

	std::vector<uint8_t> const& getData() const {
		return buffer;
	}

	void addKnownAddress(void const* _ptr, int32_t  _count, int32_t _size, int32_t _bufferPos, std::type_info const& _type_info) {
		knownAddresses.push_back({_ptr, _count, _size, _type_info, _bufferPos});
	}


	int getCurrentPosition() const {
		return currentPosition;
	}
	void setCurrentPosition(int _pos) {
		currentPosition = _pos;
	}


	void serialize(void const* _data, int32_t _count, int32_t _size, bool _needToKnowAddress, std::type_info const& _type_info) {
		if (int(buffer.size()) < currentPosition + _count * _size) {
			buffer.resize(currentPosition + _count * _size);
		}
		memcpy(&buffer[currentPosition], _data, _count * _size);
		if (_needToKnowAddress) {
			addKnownAddress(_data, _count, _size, getCurrentPosition(), _type_info);
		}
		currentPosition += _count * _size;
	}

	int32_t mapStringToInt(std::string const& _str) {
		if (stringToInt.find(_str) == stringToInt.end()) {
			int32_t size = int32_t(stringToInt.size());
			stringToInt[_str] = size;
		}

		return stringToInt.at(_str);
	}

	void serialize(std::string const& _str, bool _needToKnowAddress) {
		if (_needToKnowAddress) {
			addKnownAddress(&_str, 1, sizeof(_str), getCurrentPosition(), typeid(std::string));
		}
		int32_t size = _str.length();
		serialize(size, false);
		serialize(_str.c_str(), size, sizeof(char), false, typeid(char));
	}

	template<typename T, typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr>
	void serialize(T const& _value, bool _needToKnowAddress) {

		serialize(&_value, 1, sizeof(T), _needToKnowAddress, typeid(T));
	}

	template<typename T, int N, typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr>
	void serialize(std::array<T, N> const& _value, bool _needToKnowAddress) {
		if (_needToKnowAddress) {
			addKnownAddress(&_value, 1, sizeof(std::vector<T>), getCurrentPosition(), typeid(std::array<T, N>));
		}

		serialize(_value.data(), N, sizeof(T), _needToKnowAddress, typeid(T));
	}

	template<typename T, typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr>
	void serialize(std::vector<T>& _value, bool _needToKnowAddress) {

		if (_needToKnowAddress) {
			addKnownAddress(&_value, 1, sizeof(std::vector<T>), getCurrentPosition(), typeid(std::vector<T>));
		}
		int32_t size = _value.size();
		serialize(size, false);
		serialize(_value.data(), size, sizeof(T), _needToKnowAddress, typeid(T));
	}

	template<typename T, typename std::enable_if<has_serialize_function<T, SerializerNode>::value>::type* = nullptr>
	void serialize(T& _value, bool _needToKnowAddress) {
		if (_needToKnowAddress) {
			addKnownAddress(&_value, 1, sizeof(T), getCurrentPosition(), typeid(T));
		}
		SerializerNode node(*this, _needToKnowAddress);

		_value.serialize(node);
	}
	template<typename T>
	void serialize(T*& _value, bool _needToKnowAddress) {
		if (_needToKnowAddress) {
			addKnownAddress(&_value, 1, sizeof(T*), getCurrentPosition(), typeid(T*));
		}

		if (_value != nullptr) {
			int32_t pos = getCurrentPosition();
			rawAddresses.push_back(RawAddress{_value, typeid(T), pos});
		}
		serialize(int32_t(), false);
	}
	template<typename T>
	void serialize(std::unique_ptr<T>& _value) {
		if (_value != nullptr) {
			serialize(int32_t(), false);
		} else {
			serialize(int32_t(), false);
		}
	}

	template<typename T, typename std::enable_if<not std::is_fundamental<T>::value
	                                             and not has_serialize_function<T, SerializerNode>::value>::type* = nullptr>
	void serialize(T& _value, bool _needToKnowAddress) {
		if (_needToKnowAddress) {
			addKnownAddress(&_value, 1, sizeof(T), getCurrentPosition(), typeid(T));
		}

		SerializerAdapter adapter(*this, _needToKnowAddress);
		Converter<T>::serialize(adapter, _value);

	}
};

template<typename T>
SerializerDefault<T>::~SerializerDefault() {
	// Serializing root object
	if (id == -1) {
		serializer.serialize(value, needToKnowAddress);
		return;
	}

	serializer.serialize(id, false);
	serializer.serialize(int32_t(), false);
	int32_t sizePos = serializer.getCurrentPosition();

	serializer.serialize(value, needToKnowAddress);

	int32_t endPos = serializer.getCurrentPosition();
	int32_t size = endPos - sizePos;
	memcpy(serializer.getPtr() + sizePos-sizeof(int32_t), &size, sizeof(size));
}

template<typename T>
SerializerDefault<T> SerializerNodeInput::operator%(T& t) {
	return SerializerDefault<T>(serializer, id, t, needToKnowAddress);
}

template<typename T>
void SerializerAdapter::serialize(T& _value) {
	serializer.serialize(_value, needToKnowAddress);
}

template<typename Iter>
void SerializerAdapter::serializeByIter(Iter iter, Iter end) {
	int32_t size = std::distance(iter, end);
	serialize(size);
	for (;iter != end; ++iter) {
		serialize(*iter);
	}
}




}
}
