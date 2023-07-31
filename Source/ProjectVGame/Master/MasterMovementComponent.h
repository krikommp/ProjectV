// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "MasterMovementComponent.generated.h"

struct FInterpJump
{
	float StartZ = 0.0f;
	float EndZ = 0.0f;
	float Time = 0.0f;

	void Init(float Z, float ZOffset);
	float Interp(float DeltaTime, float Speed);
	void Reset();
	bool Check(float Z);
};

class ATilemap3DActor;
/**
 * UMasterMovementComponent
 *
 * 主相机移动组件，用于战旗场景漫游等操作
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UMasterMovementComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
									FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
									   FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 地面检测的射线高度，也就是与地面的距离
	UPROPERTY(EditAnywhere, Category="Ground")
	float GroundCheckLength = 100.f;
	// 下坠速度
	UPROPERTY(EditAnywhere, Category="Ground")
	float FallingSpeed = 30.0f;
	// 移动检测距离
	UPROPERTY(EditAnywhere, Category="Move")
	float MoveCheckLength = 50.0f;
	// 移动速度
	UPROPERTY(EditAnywhere, Category="Move")
	float MoveSpeed = 30.0f;
	// 上跳速度
	UPROPERTY(EditAnywhere, Category="Move")
	float JumpSpeed = 1.5f;
	// 定义该模块的名称
	static const FName NAME_ActorFeatureName;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void Falling(float DeltaTime) const;
	bool CheckUnderGround() const;
	void Moving(float DeltaTime) const;
	void MovingUp(float DeltaTime);
	bool CheckNeedJump() const;
	void FinishMovement();

	uint8 bNeedJump : 1;
	uint8 bPanning : 1;
	
	TWeakObjectPtr<const ATilemap3DActor> Tilemap3DActorRef;

	FInterpJump InterpJump;
};
