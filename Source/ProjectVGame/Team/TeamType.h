#pragma once

/**
 * ETeamType
 *
 * 定义队伍标识
 */
UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Player UMETA(DisplayName = "Player Team"),
	Enemy UMETA(DisplayName = "Enemy Team")
};
