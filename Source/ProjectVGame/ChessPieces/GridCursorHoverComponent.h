// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "GridCursorHoverComponent.generated.h"

class AGridChessPiece;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChessPieceHoverBegin, AGridChessPiece*, InChessPiece);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChessPieceHoverEnd, AGridChessPiece*, InChessPiece);

/**
 * UGridCursorHoverComponent
 *
 * 光标选中组件
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridCursorHoverComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UGridCursorHoverComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure)
	static UGridCursorHoverComponent* FindCursorHoverComponent(const AActor* Actor) { return Actor ? Actor->FindComponentByClass<UGridCursorHoverComponent>() : nullptr; } 

public:
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnChessPieceHoverBegin OnChessPieceHoverBegin;

	UPROPERTY(BlueprintAssignable)
	FOnChessPieceHoverEnd OnChessPieceHoverEnd;
private:
	TObjectPtr<AGridChessPiece> lastChessPieceHovered;

	bool bCheckCursor;
};
