#pragma once

#include "Converter.h"

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>


namespace serializer {
	template<typename T>
	class Converter<std::vector<T>> {
	public:
		template<typename Adapter>
		static void serialize(Adapter& adapter, std::vector<T>& x) {
			adapter.serializeByIter(x.begin(), x.end());
		}
		template<typename Adapter>
		static void deserialize(Adapter& adapter, std::vector<T>& x) {
			std::function<void(T const&)> func = [&x](T const& v) {
				x.push_back(v);
			};
			adapter.deserializeByInsert(func);
		}
	};

	template<typename T>
	class Converter<std::list<T>> {
	public:
		template<typename Adapter>
		static void serialize(Adapter& adapter, std::list<T>& x) {
			adapter.serializeByIter(x.begin(), x.end());
		}
		template<typename Adapter>
		static void deserialize(Adapter& adapter, std::list<T>& x) {
			std::function<void(T const&)> func = [&x](T const& v) {
				x.push_back(v);
			};
			adapter.deserializeByInsert(func);
		}
	};

	template<typename T>
	class Converter<std::set<T>> {
	public:
		template<typename Adapter>
		static void serialize(Adapter& adapter, std::set<T>& x) {
			adapter.serializeByIterCopy(x.begin(), x.end());
		}
		template<typename Adapter>
		static void deserialize(Adapter& adapter, std::set<T>& x) {
			std::function<void(T const&)> func = [&x](T const& v) {
				x.insert(v);
			};
			adapter.deserializeByInsert(func);
		}
	};

	template<typename Key, typename Value>
	class Converter<std::pair<Key, Value>> {
	public:
		template<typename PKey, typename PValue>
		struct Pair {
			std::pair<PKey, PValue>& pair;
			template<typename Node>
			void serialize(Node& node) {
				node["first"]  % pair.first;
				node["second"] % pair.second;
			}
		};
		template<typename Adapter>
		static void serialize(Adapter& adapter, std::pair<Key, Value>& x) {
			Pair<Key, Value> pair {x};
			adapter.serialize(pair);
		}
		template<typename Adapter>
		static void deserialize(Adapter& adapter, std::pair<Key, Value>& x) {
			Pair<Key, Value> pair {x};
			adapter.deserialize(pair);
		}
	};

	template<typename Key, typename Value>
	class Converter<std::map<Key, Value>> {
	public:
		template<typename Adapter>
		static void serialize(Adapter& adapter, std::map<Key, Value>& x) {
			adapter.serializeByIter(x.begin(), x.end());
		}
		template<typename Adapter>
		static void deserialize(Adapter& adapter, std::map<Key, Value>& x) {
			std::function<void(std::pair<Key, Value> const&)> func = [&x]
			(std::pair<Key, Value> const& v) {
				x.insert(v);
			};
			adapter.deserializeByInsert(func);
		}
	};

	template<>
	class Converter<std::string> {
	public:
		template<typename Adapter>
		static void serialize(Adapter& adapter, std::string& x) {
			std::vector<uint8_t> v(x.size()+1, '\0');
			memcpy(&v[0], x.c_str(), x.size());
			adapter.serialize(v);
		}
		template<typename Adapter>
		static void deserialize(Adapter& adapter, std::string& x) {
			std::vector<uint8_t> v;
			adapter.deserialize(v);
			x = (char*)v.data();
		}
	};



}

/*namespace jsonSerializer {
	template<>
	class Converter<std::string> {
	public:
		static void   serialize(Node& node, std::string& x) {
			node.getValue() = x;
		}
		static void deserialize(Node& node, std::string& x) {
			if (not node.getValue().isString()) throw WrongType(node.getValue(), "expected string");
			x = node.getValue().asString();
		}
	};

	template<typename T>
	class Converter<std::vector<T>> {
	public:
		static void   serialize(Node& node, std::vector<T>& x) {
			node.getValue() = Json::arrayValue;
			int index {0};
			for (auto& e : x) {
				Json::Value value;
				auto path = node.getNodePath();
				path.push_back(std::to_string(index++));
				NodeValue newNode(value, node.isSerializing(), false, &node, path);
				newNode % e;
				node.getValue().append(value);
			}
		}
		static void deserialize(Node& node, std::vector<T>& x) {
			if (not node.getValue().isArray()) throw WrongType(node.getValue(), "expected array");
			x.clear();
			x.reserve(node.getValue().size());
			for (uint i(0); i<node.getValue().size(); ++i) {
				x.push_back(T());
				auto path = node.getNodePath();
				path.push_back(std::to_string(i));
				NodeValue newNode(node.getValue()[i], node.isSerializing(), false, &node, path);
				newNode % x.back();
			}
		}
	};
	template<typename T>
	class Converter<std::list<T>> {
	public:
		static void   serialize(Node& node, std::list<T>& x) {
			node.getValue() = Json::arrayValue;
			int index {0};
			for (auto& e : x) {
				Json::Value value;
				auto path = node.getNodePath();
				path.push_back(std::to_string(index++));
				NodeValue newNode(value, node.isSerializing(), false, &node, path);
				newNode % e;
				node.getValue().append(value);
			}
		}
		static void deserialize(Node& node, std::list<T>& x) {
			if (not node.getValue().isArray()) throw WrongType(node.getValue(), "expected array");
			x.clear();
			for (uint i(0); i<node.getValue().size(); ++i) {
				x.push_back(T());
				auto path = node.getNodePath();
				path.push_back(std::to_string(i));

				NodeValue newNode(node.getValue()[i], node.isSerializing(), false, &node, path);
				newNode % x.back();
			}
		}
	};
	template<typename T>
	class Converter<std::set<T>> {
	public:
		static void   serialize(Node& node, std::set<T>& x) {
			node.getValue() = Json::arrayValue;
			int index {0};
			for (auto e : x) {
				Json::Value value;

				auto path = node.getNodePath();
				path.push_back(std::to_string(index++));

				NodeValue newNode(value, node.isSerializing(), false, &node, path);
				newNode % e;
				node.getValue().append(value);
			}
		}
		static void deserialize(Node& node, std::set<T>& x) {
			if (not node.getValue().isArray()) throw WrongType(node.getValue(), "expected array");
			x.clear();
			for (uint i(0); i<node.getValue().size(); ++i) {
				T t;
				auto path = node.getNodePath();
				path.push_back(std::to_string(i));

				NodeValue newNode(node.getValue()[i], node.isSerializing(), false, &node, path);
				newNode % t;
				x.insert(std::move(t));
			}
		}
	};

	template<typename T, std::size_t N>
	class Converter<std::array<T, N>> {
	public:
		static void   serialize(Node& node, std::array<T, N>& x) {
			node.getValue() = Json::arrayValue;
			int index {0};
			for (auto& e : x) {
				Json::Value value;
				auto path = node.getNodePath();
				path.push_back(std::to_string(index++));

				NodeValue newNode(value, node.isSerializing(), false, &node, path);
				newNode % e;
				node.getValue().append(value);
			}
		}
		static void deserialize(Node& node, std::array<T, N>& x) {
			if (not node.getValue().isArray()) throw WrongType(node.getValue(), "expected array");
			for (uint i(0); i<node.getValue().size(); ++i) {
				auto path = node.getNodePath();
				path.push_back(std::to_string(i));

				NodeValue newNode(node.getValue()[i], node.isSerializing(), false, &node, path);
				newNode % x[i];
			}
		}
	};

	template<typename T1, typename T2>
	class Converter<std::pair<T1, T2>> {
	public:
		static void   serialize(Node& node, std::pair<T1, T2>& x) {
			node["first"]  % x.first;
			node["second"] % x.second;
		}
		static void deserialize(Node& node, std::pair<T1, T2>& x) {
			node["first"]  % x.first;
			node["second"] % x.second;
		}
	};


	template<typename T1, typename T2>
	class Converter<std::map<T1, T2>> {
	public:
		static void   serialize(Node& node, std::map<T1, T2>& x) {
			node.getValue() = Json::arrayValue;
			for (auto& e : x) {
				T1  key   = e.first;
				T2& value = e.second;
				Json::Value jsonValue;

				auto pathKey = node.getNodePath();
				auto pathValue = node.getNodePath();
				pathKey.push_back("key");
				pathValue.push_back("value");

				NodeValue(jsonValue["key"], true, false, &node, pathKey)   % key;
				NodeValue(jsonValue["value"], true, false, &node, pathValue) % value;
				node.getValue().append(jsonValue);
			}
		}
		static void deserialize(Node& node, std::map<T1, T2>& x) {
			if (not node.getValue().isArray()) throw WrongType(node.getValue(), "expected array");
			x.clear();
			for (uint i(0); i<node.getValue().size(); ++i) {
				T1 key;
				T2 value;

				auto pathKey = node.getNodePath();
				auto pathValue = node.getNodePath();
				pathKey.push_back("key");
				pathValue.push_back("value");

				NodeValue(node.getValue()[i]["key"], false, false, &node, pathKey)   % key;
				NodeValue(node.getValue()[i]["value"], false, false, &node, pathValue) % value;
				x[key] = std::move(value);
			}
		}
	};
}*/
