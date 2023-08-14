// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponents.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "MasterInputComponent.generated.h"

struct FMappableConfigPair;
struct FInputActionValue;

/**
 * UMasterInputComponent
 *
 * 主相机输入组件，用于定义玩家的各种输入配置
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UMasterInputComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UMasterInputComponent* FindMasterInputComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UMasterInputComponent>() : nullptr);
	}

	// 默认的输入配置
	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigPair> DefaultInputConfigs;

	// 相机旋转速度
	UPROPERTY(EditAnywhere)
	float CameraRotateSpeed = 3.0f;
	
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
	static const FName NAME_BindInputsNow;
	// 定义该模块的名称
	static const FName NAME_ActorFeatureName;
	// 是否完成绑定标志位
	uint8 bReadyToBindInputs : 1;

	// 执行按键绑定的操作
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	//~ Begin 按键操作函数
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_CameraRotate(const FInputActionValue& InputActionValue);
	void Input_MouseConfirm(const FInputActionValue& InputActionValue);
	void Input_MouseCancel(const FInputActionValue& InputActionValue);
	//~ Begin 按键操作函数

	// 检测鼠标悬停的Actor是否有效
	void CheckMouseHoverActor() const;
};
