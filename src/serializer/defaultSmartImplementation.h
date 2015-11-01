#pragma once

namespace serializer {

	template<typename T>
	class Converter<std::unique_ptr<T>> {
	public:
		static void   serialize(Node& node, std::unique_ptr<T>& x) {
			bool valid = x.get() != nullptr;
			node["valid"] % valid;
			if (valid) {
				node["data"] % *(x.get());
			}
			node.addPossibleOwnerlessObject(*x.get(), node.getNodePath());

		}
		static void deserialize(Node& node, std::unique_ptr<T>& x) {
			bool valid;
			node["valid"] % valid;
			if (not valid) {
				x.reset();
			} else {
				x.reset(new T);
				node["data"] % *(x.get());
			}
			node.addPossibleOwnerlessObject(*x.get(), node.getNodePath());
		}
	};
	template<typename T>
	class Converter<std::shared_ptr<T>> {
	public:
		static void   serialize(Node& node, std::shared_ptr<T>& x) {
			bool valid = x.get() != nullptr;
			node["valid"] % valid;
			if (valid) {
				int ref = node.addSharedObject(x);
				node["data"] % ref;
			}
			node.addPossibleOwnerlessObject(*x.get(), node.getNodePath());
		}
		static void deserialize(Node& node, std::shared_ptr<T>& x) {
			bool valid;
			node["valid"] % valid;
			if (not valid) {
				x.reset();
			} else {
				int ref = 0;
				node["data"] % ref;
				node.getSharedObject(ref, x);
			}
			node.addPossibleOwnerlessObject(*x.get(), node.getNodePath());
		}
	};
}
