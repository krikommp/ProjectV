// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "UObject/Object.h"
#include "GridPawnExtensionComponent.generated.h"

class UGridPawnData;
class UGridAbilitySystemComponent;

/**
 * UGridPawnExtensionComponent
 *
 * 提供便利的函数为 Characters / Pawns 类服务
 * 帮助其他组件初始化
 */
UCLASS()
class PROJECTVGAME_API UGridPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:

	UGridPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	static const FName NAME_ActorFeatureName;

	// ~ IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	// ~ IGameFrameworkInitStateInterface interface

	// 如果指定的 Actor 上存在 PawnExtension component，则返回
	UFUNCTION(BlueprintPure, Category="Grid|Pawn")
	static UGridPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGridPawnExtensionComponent>() : nullptr); }

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UGridPawnData* InPawnData);
	
	UFUNCTION(BlueprintPure, Category="Grid|Pawn")
	UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const { return AbilitySystemComponent; }

	// 需要在 pawn 初始化 ASC 时调用，用于通知其他组件 ASC 初始化完毕事件
	void InitializeAbilitySystem(UGridAbilitySystemComponent* InASC, AActor* InOwnerActor);

	// 需要在 pawn 注销 ASC 时调用，用于通知其他组件 ASC 注销事件
	void UninitializeAbilitySystem();

	// 当控制器改变时调用
	void HandleControllerChanged();

	// 当 PlayerState 复制时调用
	void HandlePlayerStateReplicated();

	// 当 InputComponent 被设置时调用
	void SetupPlayerInputComponent();

	// 注册事件到 OnAbilitySystemInitialized, 并在 ASC 被注册时调用
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	// 注册事件到 OnAbilitySystemUninitialized, 并在 ASC 被注销时调用
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnReq_PawnData();
protected:
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
	UPROPERTY(EditInstanceOnly, ReplicatedUsing=OnReq_PawnData, Category="Grid|Pawn")
	TObjectPtr<const UGridPawnData> PawnData;

	UPROPERTY()
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;
};
