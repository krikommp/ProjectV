#pragma once

/**
 * ETeamType
 *
 * 定义队伍标识
 */
UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Player = 0 UMETA(DisplayName = "Player Team"),
	Enemy = 1 UMETA(DisplayName = "Enemy Team"),

	MAX,
};
