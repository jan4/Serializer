#pragma once

#include "Converter.h"

#include <list>
#include <map>
#include <memory>
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
			x.clear();
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
			x.clear();
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
			x.clear();
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
			x.clear();
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

	template<typename T>
	class Converter<std::unique_ptr<T>> {
	public:
		struct SerUPtr {
			std::unique_ptr<T>& ptr;
			template<typename Node>
			void serialize(Node& node) {
				bool valid = ptr.get() != nullptr;
				node["valid"] % valid;
				if (valid) {
					node["data"] % *(ptr.get());
				}
			}
		};
		struct DeserUPtr {
			std::unique_ptr<T>& ptr;
			template<typename Node>
			void serialize(Node& node) {
				bool valid;
				node["valid"] % valid;
				if (valid) {
					ptr.reset(new T{});
					node["data"] % *(ptr.get());
				} else {
					ptr.reset();
				}
			}
		};

		template<typename Adapter>
		static void serialize(Adapter& adapter, std::unique_ptr<T>& x) {
			SerUPtr ptr {x};
			adapter.serialize(ptr);
		}
		template<typename Adapter>
		static void deserialize(Adapter& adapter, std::unique_ptr<T>& x) {
			DeserUPtr ptr {x};
			adapter.deserialize(ptr);
		}
	};

}


