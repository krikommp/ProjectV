// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridChessPieceComponent.h"
#include "GridChessPieceData.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GridChessPieceExtensionComponent.generated.h"

class UGridChessPieceData;
class UGridAbilitySystemComponent;
class UGridLocalPlayer;
class UGridHeroInfo;

/**
 * UGridChessPieceExtensionComponent
 *
 * 为棋子类数据初始化提供便利的函数
 */
UCLASS()
class PROJECTVGAME_API UGridChessPieceExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UGridChessPieceExtensionComponent(const FObjectInitializer& ObjectInitializer);

	static const FName NAME_ActorFeatureName;

	// ~ IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	// ~ IGameFrameworkInitStateInterface interface

	UFUNCTION(BlueprintPure, Category="Grid|Piece")
	static UGridChessPieceExtensionComponent* FindGridChessPieceExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGridChessPieceExtensionComponent>() : nullptr); } 

	template <class T>
	const T* GetChessPieceData() const { return Cast<T>(PieceData); }
	
	void SetChessPieceData(const UGridChessPieceData* InPieceData);

	UFUNCTION(BlueprintPure, Category="Grid|Piece")
	UGridHeroInfo* GetChessPieceInfo() const { return HeroInfo; } 

	UFUNCTION(BlueprintPure, Category="Grid|Piece")
	UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const { return AbilitySystemComponent; }

	UFUNCTION(BlueprintPure, Category="Grid|Piece")
	int32 GetPlayerIndex() const { return PieceData->PlayerIndex; }

	// 判断棋子是否为同盟
	UFUNCTION(BlueprintPure, Category="Grid|Piece")
	bool IsFaction(const AGridChessPiece* Other) const;

	// 获取棋子单位位置索引
	UFUNCTION(BlueprintPure, Category="Grid|Piece")
	FORCEINLINE int32 GetTileIndex() const { return TileIndex; }

	// 设置棋子位置索引
	UFUNCTION(BlueprintCallable, Category="Grid|Piece")
	FORCEINLINE void SetTileIndex(int32 InTileIndex) { TileIndex = InTileIndex; }
	
	// 需要在 pawn 初始化 ASC 时调用，用于通知其他组件 ASC 初始化完毕事件
	void InitializeAbilitySystem(UGridAbilitySystemComponent* InASC, AActor* InOwnerActor);

	// 需要在 pawn 注销 ASC 时调用，用于通知其他组件 ASC 注销事件
	void UninitializeAbilitySystem();

	// 初始化棋子数据
	void InitializeChessPieceInfo(UGridLocalPlayer* InLocalPlayer, UGridHeroInfo* InHeroInfo);

	// 初始化 AbilitySet
	void InitializeAbilitySet();

	// 注册事件到 OnChessPieceInfoInitialized, 并在 Piece Info 初始化完成时调用
	void OnChessPieceInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	// 注册事件到 OnAbilitySystemInitialized, 并在 ASC 被注册时调用
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	// 注册事件到 OnAbilitySystemUninitialized, 并在 ASC 被注销时调用
	void OnAbilitySystemUninitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	FSimpleMulticastDelegate OnChessPieceInfoInitialized;

	UPROPERTY(EditInstanceOnly, Category="Grid|Piece")
	TObjectPtr<const UGridChessPieceData> PieceData;

	UPROPERTY()
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UGridHeroInfo> HeroInfo;

	UPROPERTY()
	TObjectPtr<const UGridLocalPlayer> Player;

	// 棋子单位位置索引
	int32 TileIndex;
};
