﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/EngineTypes.h"
#include "GameFramework/PlayerStart.h"
#include "GameplayTagContainer.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "GridPlayerStart.generated.h"

class AController;
class UObject;

enum class EGridPlayerStartLocationOccupancy
{
	Empty,
	Partial,
	Full
};


/**
 * AGridPlayerStart
 *
 * 项目自定义 PlayerStart
 */
UCLASS()
class PROJECTVGAME_API AGridPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	AGridPlayerStart(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const FGameplayTagContainer& GetGameplayTags() { return StartPointTags; }

	EGridPlayerStartLocationOccupancy GetLocationOccupancy(AController* const ControllerPawnToFit) const;

	/** Did this player start get claimed by a controller already? */
	bool IsClaimed() const;

	/** If this PlayerStart was not claimed, claim it for ClaimingController */
	bool TryClaim(AController* OccupyingController);

protected:
	/** Check if this PlayerStart is still claimed */
	void CheckUnclaimed();

	/** The controller that claimed this PlayerStart */
	UPROPERTY(Transient)
	TObjectPtr<AController> ClaimingController = nullptr;

	/** Interval in which we'll check if this player start is not colliding with anyone anymore */
	UPROPERTY(EditDefaultsOnly, Category = "Player Start Claiming")
	float ExpirationCheckInterval = 1.f;

	/** Tags to identify this player start */
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer StartPointTags;

	/** Handle to track expiration recurring timer */
	FTimerHandle ExpirationTimerHandle;
};
