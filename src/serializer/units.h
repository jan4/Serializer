#pragma once

#include <units/units.h>

namespace jsonSerializer {
	template<>
	class Converter<Second> {
	public:
		static void   serialize(Node& node, Second& x) {
			node.getValue() = x.value();
		}
		static void deserialize(Node& node, Second& x) {
			x = node.getValue().asDouble() * 1_s;
		}
	};
}
