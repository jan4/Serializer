#pragma once

#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

//#include "Node.h"
#include "BinarySerializer.h"
#include "JsonSerializer.h"
#include "primitiveTypes.h"
#include "standardTypes.h"
/*
#ifdef ABUILD_GENERICFACTORY
	#include <genericFactory/genericFactory.h>
#endif


namespace serializer {

	template<typename ...T>
	class Converter;

	class NodeValue;

	template<typename T>
	class NodeValueDefault;

	using NodePath = std::vector<std::string>;
}

#include "Node.h"
#include "NodeValue.h"
namespace serializer {

	template<typename T>
	class Converter<T*> {
	public:
		static void serialize(Node& node, T*& x) {
			bool valid = x != nullptr;
			node["valid"] % valid;
			if (valid) {
				int ref = node.addOwnerlessObject(x);
				node["data"] % ref;
			}
		}
		static void deserialize(Node& node, T*& x) {
			bool valid;
			node["valid"] % valid;
			if (not valid) {
				x = nullptr;
			} else {
				int ref = 0;
				node["data"] % ref;
				node.getOwnerlessObject(ref, x);
			}
		}
	};



	template<typename ...T>
	class ConverterSplit;

	template<typename T>
	class Converter<T> {
	public:
		static void serialize(Node& node, T& x) {
			static_assert(has_serialize_function<T>::value, "Data type has no serialization function");
			x.serialize(node);
		}
		static void deserialize(Node& node, T& x) {
			 _deserialize(node, x, 0, 0);
		}
	};

	template<typename T>
	auto _deserialize(Node& node, T& x, int, int)
		-> decltype(x.deserialize(node)) {
		x.deserialize(node);
	}
	template<typename T>
	auto _deserialize(Node& node, T& x, int, long)
		-> decltype(x.serialize(node)){
		x.serialize(node);
	}


	template<typename T>
	auto _deserialize(Node&, T&, long, long)
		-> void {
		static_assert(has_serialize_function<T>::value, "Data type has no serialization function");
	}

}
*/
/*namespace serializer {
	template<typename T>
	void read(std::string const& _file, T& _data) {
		Json::Value root;
		NodeValue node(root, false, false, nullptr, {});
		std::ifstream ifs(_file);
		if (ifs.fail()) {
			throw Exception("Opening file failed");
		}
		std::stringstream strStream;
		strStream << ifs.rdbuf();
		Json::Reader reader;
		if (not reader.parse(strStream.str(), root)) {
			throw Exception("Parsing file failed: "+reader.getFormattedErrorMessages());
		}
		try {
			node.readSharedObjects();
			node % _data;
			node.readSharedObjectsPost();

		} catch(WrongType const& type) {
			reader.pushError(type.getValue(), type.what(), type.getValue());
			throw Exception("Parsing failed: "+ reader.getFormattedErrorMessages());
		}

	}
	template<typename T>
	void write(std::string const& _file, T& _data) {
		Json::Value root;
		NodeValue node(root, true, false, nullptr, {});
		node % _data;
		node.writeSharedObjects();
		Json::StyledWriter jsonWriter;
		std::string jsonString = jsonWriter.write(root);
		std::ofstream oFile(_file);
		if (oFile.fail()) {
			throw Exception("Opening file failed");
		}
		oFile << jsonString;
		oFile.close();
		if (oFile.fail()) {
			throw Exception("Writing to file failed");
		}
	}
}*/
/*
#include "primitiveTypes.h"
#include "standardTypes.h"
#ifdef ABUILD_ARMADILLO_BITS
	#include "armadillo.h"
#endif
#ifdef ABUILD_GENERICFACTORY
	#include "genericFactorySmartImplementation.h"
#else
	#include "defaultSmartImplementation.h"
#endif
#ifdef ABUILD_UNITS
	#include "units.h"
#endif
*/

