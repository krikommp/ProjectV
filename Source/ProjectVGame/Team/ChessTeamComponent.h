// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "UObject/Object.h"
#include "TeamType.h"
#include "ChessTeamComponent.generated.h"

class AGridChessBase;
struct FTilemapSpawnParameters;

/**
 * UChessTeamComponent
 *
 * 管理场景中队伍组件
 */
UCLASS()
class PROJECTVGAME_API UChessTeamComponent : public UGameStateComponent
{
	GENERATED_UCLASS_BODY()


protected:
	//~ Begin UActorComponent Interface.
	virtual void OnRegister() override;
	//~ Begin UActorComponent Interface.

public:

	// 添加队伍成员
	UFUNCTION(BlueprintCallable, Category=Team)
	void AddTeamMember(const ETeamType Team, AGridChessBase* Member);
	void AddTeamMember(const ETeamType Team, const TArray<AGridChessBase*>& Members);

	// 检测棋子是否在队伍中
	UFUNCTION(BlueprintCallable, Category=Team)
	bool CheckChessInTeam(const ETeamType Team, const FName ChessID)const;
	
	// 获取队伍长度
	UFUNCTION(BlueprintCallable, Category=Team)
	FORCEINLINE int32 GetTeamLength(const ETeamType Team) const { return Teams[Team].Num(); }

	// 获取队伍成员
	UFUNCTION(BlueprintCallable, Category=Team)
	FORCEINLINE AGridChessBase* GetMember(const ETeamType Team, int32 Index) const { return Teams[Team][Index].Get(); }

private:
	// 队伍
	TMap<const ETeamType, TArray<TWeakObjectPtr<AGridChessBase>>> Teams;
};

#define BEGIN_TEAM_SCOPE(TEAM_TYPE)\
{ \
	const ETeamType Team = TEAM_TYPE;

#define END_TEAM_SCOPE()\
}

#define GET_TEAM_NUM(TEAM_COMP) TEAM_COMP->GetTeamLength(Team)
#define GET_TEAM_MEM(TEAM_COMP, INDEX) TEAM_COMP->GetMember(Team, INDEX)