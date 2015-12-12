#include "Deserializer.h"

#include <serializer/standardTypes.h>



namespace serializer {
namespace yaml {

DeserializerNode::DeserializerNode(Deserializer& _serializer, YAML::Node& _node, bool _available, NodePath const& _nodePath)
	: serializer ( _serializer )
	, node       ( _node )
	, nodePath   { _nodePath }
{
	(void)_available; // Unused
}

DeserializerNode::~DeserializerNode() {
}

DeserializerNodeInput DeserializerNode::operator[](std::string const& _str) {
	NodePath newNodePath = nodePath;
	newNodePath.push_back(_str);
	if (not node.IsMap() or node[_str].IsNull()) {
		return DeserializerNodeInput(serializer, node, false, newNodePath);
	}

	_nodes.push_back(node[_str]);
//	auto tnode = node[_str];
	return DeserializerNodeInput(serializer, _nodes.back(), true, newNodePath);
}

Deserializer::Deserializer(std::vector<uint8_t> const& _data) {
	node = YAML::Load((char*)_data.data());
	if (node.IsMap() and not node["__sharedObjects"].IsNull()) {
		sharedObjectNode = node["__sharedObjects"];
	}
}
Deserializer::Deserializer(std::string const& _data) {
	node = YAML::Load(_data);
	if (node.IsMap() and not node["__sharedObjects"].IsNull()) {
		sharedObjectNode = node["__sharedObjects"];
	}
}

void Deserializer::close() {
	// Check if ownerless objects (raw pointers table) are available
	std::map<int32_t, std::vector<NodePath>> nodePaths;

	if (node.IsMap() and not node["__ownerlessObjects"].IsNull()) {
		YAML::Node values = node["__ownerlessObjects"];
		deserialize(values, nodePaths, {"__ownerlessObjects"});
	}

	for (auto& raw : rawAddresses) {
		if (nodePaths.find(raw.first) == nodePaths.end()) continue;
		auto const& paths = nodePaths[raw.first];

		for (auto const& path : paths) {
			if (knownAddresses.find(path) != knownAddresses.end()) {
				void const* value = knownAddresses[path].ptr;
				for (void* p : raw.second.needOverwrite) {
					memcpy(p, &value, sizeof(void*));
				}
//				break;
			}
		}
	}
}




}
}
