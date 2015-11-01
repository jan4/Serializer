#include "Node.h"

namespace serializer {

	Node::Node(NodeImplementation& _implementation)
		: parent         { nullptr }
		, implementation { _implementation } {
	}
	Node::Node(NodeImplementation& _implementation, Node const* _parent)
		: parent { _parent }
		, implementation { _implementation } {
	}

	NodeInput Node::operator[](std::string const& _s) {
//		implementation.serializeString(_s);
		return NodeInput(implementation);
	}
}
