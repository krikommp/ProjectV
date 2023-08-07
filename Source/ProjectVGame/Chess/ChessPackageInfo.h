// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UObject/Object.h"
#include "ChessPackageInfo.generated.h"

class UGridLocalPlayer;

/**
 * UChessPackageInfo
 *
 * 玩家持有的棋子存档
 */
UCLASS(BlueprintType, NotBlueprintable)
class PROJECTVGAME_API UChessPackageInfo : public USaveGame
{
	GENERATED_UCLASS_BODY()
	friend class UChessPackageComponent;

	// 创建或加载棋子存档
	static UChessPackageInfo* LoadOrCreateChessPackageInfo(const FString& PlayerName, const UGridLocalPlayer* LocalPlayer);

private:
	void Initialize(UGridLocalPlayer* InLocalPlayer);
	FORCEINLINE const FString& GetSlotName() const { return SlotName; }
public:
	// 保存棋子ID列表
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Chess|Package")
	TArray<FName> ChessIDList;
	
private:
	// 持有对象
	TWeakObjectPtr<const UGridLocalPlayer> OwningPlayer;
	
	// 保存槽名称
	UPROPERTY()
	FString SlotName;
};
