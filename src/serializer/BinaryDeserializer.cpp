#include "BinaryDeserializer.h"
namespace serializer {
namespace binary {

DeserializerNode::DeserializerNode(Deserializer& _serializer, bool _available, bool _needToKnowAddress)
	: serializer ( _serializer )
	, available  { _available }
	, needToKnowAddress { _needToKnowAddress }
{
	serializer.deserialize(size, false);
	startPoint = serializer.getCurrentPosition();
}

DeserializerNode::~DeserializerNode() {
	serializer.setCurrentPosition(startPoint + size);
}

DeserializerNodeInput DeserializerNode::operator[](std::string const& _str) {
	serializer.setCurrentPosition(startPoint);

	while (serializer.getCurrentPosition() < startPoint + size) {
		int32_t id;
		serializer.deserialize(id, false);
		int32_t member_size;
		serializer.deserialize(member_size, false);

		auto str = serializer.mapIntToString(id);
		if (str == _str) {
			DeserializerNodeInput input(serializer, true, needToKnowAddress);
			return input;
		}
		serializer.setCurrentPosition(serializer.getCurrentPosition() + member_size);
	}

	DeserializerNodeInput input(serializer, false, needToKnowAddress);
	return input;
}


}
}
