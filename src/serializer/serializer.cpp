#include "serializer.h"
/*
namespace serializer {

	void Node::writeSharedObjects() {
		if (mappingSharedObjects.size() > 0) {
			value["__sharedObjects"] = sharedObjects;
		}
		if (mappingOwnerlessObjects.size() > 0) {
			std::map<int, NodePath> nodePaths;
			// fill ownerlessObjects
			for (auto const& p : mappingOwnerlessObjects) {
				if (possibleOwnerlessObjects.find(p.first) != possibleOwnerlessObjects.end()) {
					nodePaths[p.second] = possibleOwnerlessObjects[p.first];
				} else {
				}
			}
			NodeValue newNode(ownerlessObjects, true, false, nullptr, {});
			newNode % nodePaths;

			value["__ownerlessObjects"] = ownerlessObjects;
		}
	}
	void Node::readSharedObjectsPost() {
		if (not ownerlessObjects.isArray()) return;
		std::map<int, NodePath> nodePaths;
		NodeValue newNode(ownerlessObjects, false, false, nullptr, {});
		newNode % nodePaths;
		for (auto& e : mappingOwnerlessObjectsRev) {
			auto path = nodePaths[e.first];
			for (auto& e2 : possibleOwnerlessObjects) {
				if (e2.second == path) {
					for (auto e3 : e.second) {
						*e3 = e2.first;
					}
					break;
				}
			}
		}
	}



}*/
