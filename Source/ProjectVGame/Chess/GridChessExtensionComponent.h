// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponents.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "GridChessExtensionComponent.generated.h"

class UGridHeroInfo;
class UGridChessData;

/**
 * UGridChessExtensionComponent
 *
 * 棋子对象的扩展组件，用于获取数据等操作
 * 需要被挂载到棋子对象上
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()
	FIND_PAWN_COMP_FUNC(GridChessExtensionComponent)
	static const FName NAME_ActorFeatureName;

	void SetChessData(const UGridChessData* InData);
	const UGridChessData* GetChessData() const { return ChessData; }

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	// ~ IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	// ~ IGameFrameworkInitStateInterface interface

private:
	UPROPERTY(VisibleAnywhere, Category=Gameplay)
	TObjectPtr<const UGridChessData> ChessData;
	UPROPERTY(VisibleAnywhere, Category=Gameplay)
	TObjectPtr<const UGridHeroInfo> ChessInfo;
};
