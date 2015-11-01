#pragma once
namespace serializer {

	class NodeValue : public Node {
	private:
		bool defaultValueNeeded;
	public:
		NodeValue(Json::Value& _value, bool _serialize, bool _defaultValueNeeded, Node* _parent, NodePath const& _path)
			: Node(_value, _serialize, _parent, _path)
			, defaultValueNeeded(_defaultValueNeeded) {}
		template<typename T>
		NodeValueDefault<T> operator%(T& x);
	};


	template<typename T>
	inline NodeValueDefault<T> NodeValue::operator%(T& x) {
		if (getNodePath().size() > 0) {
			addPossibleOwnerlessObject(x, getNodePath());
		}

		if (serialize) {
			Converter<T>::serialize(*this, x);
			defaultValueNeeded = false;
		} else if (not defaultValueNeeded) {
			Converter<T>::deserialize(*this, x);
		}
		return NodeValueDefault<T>(x, defaultValueNeeded);
	}

}
