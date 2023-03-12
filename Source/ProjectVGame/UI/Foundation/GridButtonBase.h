// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "UObject/Object.h"
#include "GridButtonBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);

protected:
	// ~UUserWidget interface
	virtual void NativePreConstruct() override;
	// ~UUserWidget interface

	// ~UCommonButtonBase interface
	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
	// ~UCommonButtonBase interface

	void RefreshButtonText();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();

	UFUNCTION(BlueprintCallable)
	void TestFunc();
	
private:
	UPROPERTY(EditAnywhere, Category="Button", meta=(InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1;

	UPROPERTY(EditAnywhere, Category="Button", meta=(EditCondition="bOverride_ButtonText"))
	FText ButtonText;
};
