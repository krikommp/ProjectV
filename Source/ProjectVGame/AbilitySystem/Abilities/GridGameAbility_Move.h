// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridGameplayAbility.h"
#include "UObject/Object.h"
#include "GridGameAbility_Move.generated.h"

class UGridChessPieceMovementComponent;
class AGridChessPiece;
class AGridMapManager;
class UGameplayEffect;
class UGridAbilitySystemComponent;

/**
 * UGridGameAbility_Move
 *
 * 移动技能
 */
UCLASS()
class PROJECTVGAME_API UGridGameAbility_Move : public UGridGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UFUNCTION(BlueprintCallable, Category="Grid|Move Ability")
	void BeginMovement();

	UFUNCTION(BlueprintCallable, Category="Grid|Move Ability")
	void TickMovement(float DeltaTime);

	void AccelerateAndMovePawnAlongSpline(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="Grid|Move Ability")
	void EndMovement();

	UPROPERTY(BlueprintReadOnly, Category="Grid|Move Ability")
	TObjectPtr<UGridChessPieceMovementComponent> MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category="Grid|Move Ability")
	TObjectPtr<AGridChessPiece> ChessPiece;

	TWeakObjectPtr<AGridMapManager> GridMapManager;
	
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category="Grid|Move Ability")
	TSubclassOf<UGameplayEffect> MoveCostEffect;

private:
	float CurrentSplineDistance;

	bool bEndMovement;
};
