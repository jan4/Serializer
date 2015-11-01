#pragma once
namespace serializer {
	template<typename T>
	class NodeValueDefault {
	private:
		T& t;
		bool needDefaultValue;
	public:
		NodeValueDefault(T& _t, bool _needDefaultValue)
			: t(_t)
			, needDefaultValue(_needDefaultValue) {}

		void operator or(T const& _t) {
			if (needDefaultValue) {
				t = _t;
				needDefaultValue = false;
			}
		}

	};
}
