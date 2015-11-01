#pragma once

namespace serializer {
	enum class Mode { Serializing, Deserializing };

	class NodeImplementation {
	private:
		const Mode mode;
	public:
		NodeImplementation(Mode _mode) : mode { _mode } {}
		virtual ~NodeImplementation() {}

		virtual void   serialize(bool  _value) = 0;
		virtual void deserialize(bool& _value) = 0;

		virtual void   serialize(uint8_t  _value) = 0;
		virtual void deserialize(uint8_t& _value) = 0;

		virtual void   serialize(int8_t  _value) = 0;
		virtual void deserialize(int8_t& _value) = 0;

		virtual void   serialize(uint16_t  _value) = 0;
		virtual void deserialize(uint16_t& _value) = 0;

		virtual void   serialize(int16_t  _value) = 0;
		virtual void deserialize(int16_t& _value) = 0;

		virtual void   serialize(uint32_t  _value) = 0;
		virtual void deserialize(uint32_t& _value) = 0;

		virtual void   serialize(int32_t  _value) = 0;
		virtual void deserialize(int32_t& _value) = 0;

		virtual void   serialize(float  _value) = 0;
		virtual void deserialize(float& _value) = 0;

		virtual void   serialize(double  _value) = 0;
		virtual void deserialize(double& _value) = 0;

		Mode getMode() const { return mode; }
	};
}

