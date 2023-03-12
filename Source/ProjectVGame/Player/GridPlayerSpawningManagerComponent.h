// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameStateComponent.h"

#include "GridPlayerSpawningManagerComponent.generated.h"

class APlayerStart;
class AGridPlayerStart;

/**
 * UGridPlayerSpawningManagerComponent
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridPlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UGridPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer);

	/** UActorComponent */
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** ~UActorComponent */

protected:
	// Utility
	APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<AGridPlayerStart*>& FoundStartPoints) const;
	
	virtual AActor* OnChoosePlayerStart(AController* Player, TArray<AGridPlayerStart*>& PlayerStarts) { return nullptr; }
	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation) { }

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName=OnFinishRestartPlayer))
	void K2_OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation);

private:

	/** We proxy these calls from AGridGameMode, to this component so that each experience can more easily customize the respawn system they want. */
	AActor* ChoosePlayerStart(AController* Player);
	bool ControllerCanRestart(AController* Player);
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);
	friend class AGridGameMode;
	/** ~AGridGameMode */

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AGridPlayerStart>> CachedPlayerStarts;

private:
	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
};
