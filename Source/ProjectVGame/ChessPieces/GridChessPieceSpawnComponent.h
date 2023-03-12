// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GridChessPieceData.h"
#include "GridChessPieceStart.h"
#include "Components/GameStateComponent.h"
#include "UObject/Object.h"
#include "GridChessPieceSpawnComponent.generated.h"

class AGridChessPiece;
class UGridChessPieceData;
class UGridExperienceDefinition;
class AGridChessPieceStart;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGridChessPieceSpawnFinished, const TArray<AGridChessPiece*>& ChessPieceArray);

/**
 * UGridChessPieceSpawnComponent
 *
 * 地图棋子放置组件
 */
UCLASS()
class PROJECTVGAME_API UGridChessPieceSpawnComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UGridChessPieceSpawnComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnRegister() override;

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPiecePawn")
	void CollectAllChessPieceOnMap();

	bool IsSpawnFinished() const { return bSpawnInitialized; }
	
	void CallOrRegister_OnChessPieceSpawnFinished(FOnGridChessPieceSpawnFinished::FDelegate&& Delegate);

private:
	AGridChessPiece* CreateNewChessPiece(const AGridChessPieceStart* ChessPieceStart) const;
	void OnExperienceLoaded(const UGridExperienceDefinition* ExperienceDefinition);
	void OnChessPieceInitialized();
private:
	UPROPERTY(Transient)
	TArray<AGridChessPiece*> ChessPiecesList;

	FOnGridChessPieceSpawnFinished SpawnFinished;

	bool bSpawnInitialized = false;
};
