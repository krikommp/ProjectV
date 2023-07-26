// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GridChessPieceComponent.generated.h"

class UGridLocalPlayer;
class UGridHeroInfo;

/**
 * UGridChessPieceComponent
 *
 * 棋子组件，通过该组件来完成棋子的初始化
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessPieceComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	
public:

	UGridChessPieceComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintPure, Category="Grid|Piece")
	static UGridChessPieceComponent* FindGridChessPieceComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGridChessPieceComponent>() : nullptr); }

	static const FName NAME_ActorFeatureName;
	static const FName Name_LoadPieceInfoNow;

	// ~ IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	// ~ IGameFrameworkInitStateInterface interface

	FORCEINLINE bool CheckChessPieceInitialized() const { return bReadyToLoadPieceInfo; }
	
	void CallOrRegister_ChessPieceInitialized(FSimpleMulticastDelegate::FDelegate&& Delegate);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UGridHeroInfo* InitializePieceInfo(const UGridLocalPlayer* InLocalPlayer, const FName& HeroId);
protected:
	// 当棋子数据完成获取之后返回真
	bool bReadyToLoadPieceInfo;

	FSimpleMulticastDelegate OnChessPieceInitialized;
};
