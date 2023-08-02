// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "UObject/Object.h"
#include "ChessTeamComponent.generated.h"

class AGridChessBase;
/**
 * ETeamType
 *
 * 定义队伍标识
 */
UENUM(BlueprintType)
enum class PROJECTVGAME_API ETeamType : uint8
{
	Player UMETA(DisplayName = "Player Team"),
	Enemy UMETA(DisplayName = "Enemy Team")
};

/**
 * UChessTeamComponent
 *
 * 管理场景中队伍组件
 */
UCLASS()
class PROJECTVGAME_API UChessTeamComponent : public UGameStateComponent
{
	GENERATED_UCLASS_BODY()
	
	//~ Begin UActorComponent Interface.
	virtual void OnRegister() override;
	//~ Begin UActorComponent Interface.

private:
	// 队伍
	TMap<const ETeamType, TArray<TWeakObjectPtr<AGridChessBase>>> Teams;

public:

	// 添加队伍成员
	void AddTeamMember(const ETeamType Team, AGridChessBase* Member);
	void AddTeamMember(const ETeamType Team, const TArray<AGridChessBase*>& Members);
};
