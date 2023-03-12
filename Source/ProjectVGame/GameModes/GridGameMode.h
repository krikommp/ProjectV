// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonInputModeTypes.h"
#include "GridExperienceDefinition.h"
#include "ModularGameMode.h"
#include "GridGameMode.generated.h"

class UGridPawnData;
class UGridExperienceDefinition;

/**
 * 当玩家加入游戏时调用
 *
 * 会被玩家完成初始化之后调用
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameModeCombinedPostLogin, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);

/**
 * AGridGameMode
 *
 * 项目自定义 GameMode 类
 */
UCLASS(Config = Game, meta=(ShortTooltip="The base game mode class used by this project."))
class PROJECTVGAME_API AGridGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:

	AGridGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Grid|Pawn")
	const UGridPawnData* GetPawnDataForController(const AController* InController) const;

	// ~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void InitGameState() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	// ~AGameModeBase interface

	FOnGameModeCombinedPostLogin& OnGameModeCombinedPostLogin() { return OnGameModeCombinedPostLoginDelegate; }

	// 重新放置一个玩家或机器人
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

	// Agnostic version of PlayerCanRestart that can be used for both player bots and players
	virtual bool ControllerCanRestart(AController* Controller);
	
private:
	FOnGameModeCombinedPostLogin OnGameModeCombinedPostLoginDelegate;

protected:
	// ~AGameModeBase interface
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
	// ~AGameModeBase interface

	void OnExperienceLoaded(const UGridExperienceDefinition* CurrentExperience);
	bool IsExperienceLoaded() const;
	void HandleInputModeChanged(ECommonInputMode InputMode);

	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);

	void HandleMatchAssignmentIfNotExpectingOne();
};
