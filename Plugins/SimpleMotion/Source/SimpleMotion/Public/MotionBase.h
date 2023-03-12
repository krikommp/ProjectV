// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Slate/WidgetTransform.h"
#include "MotionBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishMotion);

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIMPLEMOTION_API UMotionBase : public UObject
{
	GENERATED_BODY()

public:
	UMotionBase();

public:
	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	virtual void Tick(float fDeltaTime);

public:
	/* getter or setter begin. */

	UFUNCTION(BlueprintPure, Category="UMotionBase")
	bool GetIsOperating() const { return bIsOperating; }
	
	/* getter or setter end.  */
public:
	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	virtual bool CheckFinalState();

	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	virtual void OnMotionEnds();

	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	virtual void KeepMotion(float fDeltaTime);

	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	virtual void StopMotion();
	
public:
	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	void ExecuteVector(const FVector2D& InTarget, float InSpeed);

	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	void ExecuteRotator(const FRotator& InRotator, float InSpeed);

	UFUNCTION(BlueprintCallable, Category="UMotionBase")
	void ExecuteWidgetTransform(const FWidgetTransform& Transform, float InSpeed);
	
	UFUNCTION(BlueprintImplementableEvent, Category="UMotionBase")
	bool ReceiveCheckFinalState();

	UFUNCTION(BlueprintImplementableEvent, Category="UMotionBase")
	void ReceiveMotionEnds();

	UFUNCTION(BlueprintImplementableEvent, Category="UMotionBase")
	void ReceiveKeepMotion(float fDeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category="UMotionBase")
	void ReceiveStopMotion();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="UMotionBase|Target", meta=(AllowPrivateAccess = true))
	FVector2D Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="UMotionBase|Target", meta=(AllowPrivateAccess = true))
	FRotator Rotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="UMotionBase|Target", meta=(AllowPrivateAccess = true))
	FWidgetTransform WidgetTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="UMotionBase|Speed", meta=(AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="UMotionBase|Threshold", meta=(AllowPrivateAccess = true))
	float Threshold;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="UMotionBase|IsOperating", meta=(AllowPrivateAccess = true))
	bool bIsOperating;

public:
	UPROPERTY(BlueprintAssignable, Category="UMotionBase")
	FOnFinishMotion OnFinishMotion;
};
