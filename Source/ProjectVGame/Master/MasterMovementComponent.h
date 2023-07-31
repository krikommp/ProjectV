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
	float GroundCheckLength = 0.1f;

	// 下坠速度
	UPROPERTY(EditAnywhere, Category="Ground")
	float FallingSpeed = 10.0f;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void Falling(float DeltaTime) const;
	bool CheckUnderGround() const;
};
