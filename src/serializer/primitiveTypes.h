#pragma once

//#include "NodeImplementation.h"
/*namespace serializer {
	template<>
	class Converter<bool> {
	public:
		static void serialize(NodeImplementation & impl, bool x) {
			impl.serialize(x);
		}
		static void deserialize(NodeImplementation & impl, bool& x) {
			impl.deserialize(x);
		}
	};
	template<>
	class Converter<uint8_t> {
	public:
		static void serialize(NodeImplementation & impl, uint8_t x) {
			impl.serialize(x);
		}
		static void deserialize(NodeImplementation & impl, uint8_t& x) {
			impl.deserialize(x);
		}
	};
	template<>
	class Converter<int8_t> {
	public:
		static void serialize(NodeImplementation & impl, int8_t x) {
			impl.serialize(x);
		}
		static void deserialize(NodeImplementation & impl, int8_t& x) {
			impl.deserialize(x);
		}
	};

	template<typename T>
	class Converter<std::vector<T>> {
	public:
		static void serialize(NodeImplementation& impl, std::vector<T> const& v) {
		}
		static void deserialize(NodeImplementation& impl, std::vector<T>& v) {

		}
	};
*/
/*
	template<>
	class Converter<uint8_t> {
	public:
		static void   serialize(Node& node, uint8_t& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, uint8_t& x) {
			if (not node.getValue().isUInt()) throw WrongType(node.getValue(), "expected uint8_t");
			uint32_t t = node.getValue().asUInt();
			if (t > (1<<8)-1) throw WrongType(node.getValue(), "expected uint8_t");

			x = t;
		}
	};
	template<>
	class Converter<int8_t> {
	public:
		static void   serialize(Node& node, int8_t& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, int8_t& x) {
			if (not node.getValue().isInt()) throw WrongType(node.getValue(), "expected int8_t");
			int32_t t = node.getValue().asInt();
			if (t > (1<<7)-1 || t < -128) throw WrongType(node.getValue(), "expected int8_t");

			x = t;
		}
	};
	template<>
	class Converter<uint16_t> {
	public:
		static void   serialize(Node& node, uint16_t& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, uint16_t& x) {
			if (not node.getValue().isUInt()) throw WrongType(node.getValue(), "expected uint16_t");
			uint32_t t = node.getValue().asUInt();
			if (t > (1<<16)-1) throw WrongType(node.getValue(), "expected uint16_t");

			x = t;
		}
	};
	template<>
	class Converter<int16_t> {
	public:
		static void   serialize(Node& node, int16_t& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, int16_t& x) {
			if (not node.getValue().isInt()) throw WrongType(node.getValue(), "expected int16_t");
			int32_t t = node.getValue().asInt();
			if (t > (1<<15)-1 || t < -32768 ) throw WrongType(node.getValue(), "expected int16_t");

			x = t;
		}
	};
	template<>
	class Converter<uint32_t> {
	public:
		static void   serialize(Node& node, uint32_t& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, uint32_t& x) {
			if (not node.getValue().isUInt()) throw WrongType(node.getValue(), "expected uint32_t");
			x = node.getValue().asUInt();
		}
	};

	template<>
	class Converter<int32_t> {
	public:
		static void   serialize(Node& node, int32_t& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, int32_t& x) {
			if (not node.getValue().isInt()) throw WrongType(node.getValue(), "expected int32_t");
			x = node.getValue().asInt();
		}
	};
	template<>
	class Converter<float> {
	public:
		static void   serialize(Node& node, float& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, float& x) {
			if (not node.getValue().isDouble()) throw WrongType(node.getValue(), "expected float");
			x = node.getValue().asFloat();
		}
	};

	template<>
	class Converter<double> {
	public:
		static void   serialize(Node& node, double& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, double& x) {
			if (not node.getValue().isDouble()) throw WrongType(node.getValue(), "expected double");
			x = node.getValue().asDouble();
		}
	};
*/
//}
