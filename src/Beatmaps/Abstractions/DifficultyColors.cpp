#include "Beatmaps/Abstractions/DifficultyColors.hpp"
using namespace LiteNetLib::Utils;

// DEFINE_TYPE(MultiplayerCore::Beatmaps::Abstractions, DifficultyColors);

namespace MultiplayerCore::Beatmaps::Abstractions {

	void DifficultyColors::Serialize(NetDataWriter* writer)
	{
		uint8_t colors = (uint8_t)(_colorLeft ? 1 : 0);
		colors |= (uint8_t)((_colorRight ? 1 : 0) << 1);
		colors |= (uint8_t)((_envColorLeft ? 1 : 0) << 2);
		colors |= (uint8_t)((_envColorRight ? 1 : 0) << 3);
		colors |= (uint8_t)((_envColorLeftBoost ? 1 : 0) << 4);
		colors |= (uint8_t)((_envColorRightBoost ? 1 : 0) << 5);
		colors |= (uint8_t)((_obstacleColor ? 1 : 0) << 6);
		writer->Put(colors);

		if (_colorLeft)
			SerializeMapColor(writer, *_colorLeft);
		if (_colorRight)
			SerializeMapColor(writer, *_colorRight);
		if (_envColorLeft)
			SerializeMapColor(writer, *_envColorLeft);
		if (_envColorRight)
			SerializeMapColor(writer, *_envColorRight);
		if (_envColorLeftBoost)
			SerializeMapColor(writer, *_envColorLeftBoost);
		if (_envColorRightBoost)
			SerializeMapColor(writer, *_envColorRightBoost);
		if (_obstacleColor)
			SerializeMapColor(writer, *_obstacleColor);
	}

	void DifficultyColors::Deserialize(NetDataReader* reader)
	{
		uint8_t colors = reader->GetByte();
		if ((colors & 0x1) != 0)
			_colorLeft = DeserializeMapColor(reader);
		if (((colors >> 1) & 0x1) != 0)
			_colorLeft = DeserializeMapColor(reader);
		if (((colors >> 2) & 0x1) != 0)
			_colorLeft = DeserializeMapColor(reader);
		if (((colors >> 3) & 0x1) != 0)
			_colorLeft = DeserializeMapColor(reader);
		if (((colors >> 4) & 0x1) != 0)
			_colorLeft = DeserializeMapColor(reader);
		if (((colors >> 5) & 0x1) != 0)
			_colorLeft = DeserializeMapColor(reader);
		if (((colors >> 6) & 0x1) != 0)
			_colorLeft = DeserializeMapColor(reader);
	}
}