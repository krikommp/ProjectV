// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridMapManager/GridActor/GridActor.h"
#include "GridChessPieceStart.generated.h"

class UCapsuleComponent;
class UArrowComponent;
class UGridChessPieceData;

UCLASS()
class PROJECTVGAME_API AGridChessPieceStart : public AGridActor
{
	GENERATED_BODY()
	
	friend class UGridChessPieceSpawnComponent;
	
public:
	AGridChessPieceStart(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceStart")
	const FTransform& GetCustomGridAnchorTransform() const;
	
protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Grid|ChessPieceStart")
	TObjectPtr<UGridChessPieceData> DefaultChessPieceData;
	
	UPROPERTY()
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY()
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
};
