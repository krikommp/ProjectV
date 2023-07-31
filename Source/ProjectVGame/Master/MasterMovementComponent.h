// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "MasterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UMasterMovementComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;

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
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void Falling(float DeltaTime) const;
	bool CheckUnderGround() const;

	void Moving(float DeltaTime) const;
	bool CheckNeedJump() const;

	void FinishMovement();

	bool bNeedJump;
};
