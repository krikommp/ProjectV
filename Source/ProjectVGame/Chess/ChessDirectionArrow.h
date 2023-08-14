// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "Cursor/CursorClickInterface.h"
#include "Cursor/CursorHoverInterface.h"
#include "GameFramework/Pawn.h"
#include "ChessDirectionArrow.generated.h"

class UCursorEventComponent;
struct FGameplayTag;
struct FGridMouseMessage;

struct FChessArrowInitParams
{
	UStaticMesh* ArrowMesh;
	UMaterialInterface* ArrowMaterial;
	APawn* OwnerPawn;
};

UCLASS()
class PROJECTVGAME_API AChessDirectionArrow : public AModularPawn, public ICursorClickInterface, public ICursorHoverInterface
{
	GENERATED_UCLASS_BODY()

	void InitializeArrow(const FChessArrowInitParams& Params);
	void SetTargetIndex(int32 NewTargetIndex) { TargetIndex = NewTargetIndex; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//~ Begin ICursorClickInterface interface
	virtual void OnCursorClick() override;
	//~ Begin ICursorClickInterface interface

	//~ Begin ICursorHoverInterface interface
	virtual void OnHoverBegin() override;
	virtual void OnHovering() override;
	virtual void OnHoverEnd() override;
	//~ Begin ICursorHoverInterface interface
	
private:
	UPROPERTY(VisibleAnywhere, Category="Grid|Components")
	TObjectPtr<UStaticMeshComponent> ArrowMesh;
	UPROPERTY(VisibleAnywhere, Category="Grid|Components")
	TObjectPtr<UCursorEventComponent> CursorEventComponent;

	TWeakObjectPtr<APawn> OwnerPawn;
	int32 TargetIndex;
};
