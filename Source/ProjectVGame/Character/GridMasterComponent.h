

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "Input/GridInputConfig.h"
#include "GridMasterComponent.generated.h"

struct FMappableConfigPair;
class UGridInputConfig;
struct FInputActionValue;

/**
 * UGridMasterComponent
 *
 * 用于设置输入及摄像机行为
 * 需要依赖 PawnExtensionComponent 来进行初始化流程
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridMasterComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UGridMasterComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category="Grid|Master")
	static UGridMasterComponent* FindMasterComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGridMasterComponent>() : nullptr);  }

	// 添加一个输入映射
	void AddAdditionalInputConfig(const UGridInputConfig* InputConfig);

	// 如果该输入配置被加入，则移除
	void RemoveAdditionalInputConfig(const UGridInputConfig* InputConfig);

	// 返回是否控制了一个实际的玩家对象并且准备好数据进行按键绑定
	bool IsReadyToBindInputs() const;

	// 用于告知 UGameFrameworkComponentManager 已经完成按键绑定
	static const FName NAME_BindInputsNow;

	// 这个 Component 所属的 GameFeature 名称
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);

protected:
	/**
	* Input Configs that should be added to this player when initializing the input. These configs
	* will NOT be registered with the settings because they are added at runtime. If you want the config
	* pair to be in the settings, then add it via the GameFeatureAction_AddInputConfig
	* 
	* NOTE: You should only add to this if you do not have a game feature plugin accessible to you.
	* If you do, then use the GameFeatureAction_AddInputConfig instead. 
	*/
	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigPair> DefaultInputConfigs;

	/** Spec handle for the last ability to set a camera mode. */
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};
