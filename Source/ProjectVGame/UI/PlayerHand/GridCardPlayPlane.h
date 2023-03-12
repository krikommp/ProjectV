// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GridCardPlayPlane.generated.h"

class UGridPlayerHand;

/**
 * UGridCardPlayPlane
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridCardPlayPlane : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UGridCardPlayPlane(const FObjectInitializer& ObjectInitializer);
	
	// ~UserWidget interface
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~UserWidget interface

	void SetupCardPlayPlane(UGridPlayerHand* InPlayerHand);
protected:

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UGridPlayerHand> PlayerHand;

	
};
