// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UObject/Object.h"
#include "GridCardSelectPlane.generated.h"

class UGridPlayerHand;

/**
 * UGridCardSelectPlane
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridCardSelectPlane : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UGridCardSelectPlane(const FObjectInitializer& ObjectInitializer);

	// ~UserWidget interface
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	// ~UserWidget interface

	void SetupCardSelectPlane(UGridPlayerHand* InPlayerHand);
protected:

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UGridPlayerHand> PlayerHand;
};
