// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "MasterInputComponent.generated.h"

#define FIND_COMP(Name)\
	UFUNCTION(BlueprintPure) \
	static U##Name* Find##Name(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<U##Name>() : nullptr); }


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UMasterInputComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()
	FIND_COMP(MasterInputComponent)

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                   FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

private:
	// 用于告知其他模块输入绑定完成
	static const FName Name_BindInputsNow;
	// 定义该模块的名称
	static const FName NAME_ActorFeatureName;
	// 是否完成绑定标志位
	uint8 bReadyToBindInputs : 1;

	// 执行按键绑定的操作
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
};
