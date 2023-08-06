// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "GameFramework/Pawn.h"
#include "GridChessBase.generated.h"

class UCapsuleComponent;
class UArrowComponent;
/**
 * AGridChessBase
 *
 * 最基础的棋子对象
 */
UCLASS(Abstract)
class PROJECTVGAME_API AGridChessBase : public AModularPawn
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetupSkeletalMeshAsset(USkeletalMesh* SkeletalMesh) const;
	void SetupAnimInstanceClass(TSubclassOf<UAnimInstance> AnimBlueprint) const;
	
protected:
	UPROPERTY(VisibleAnywhere, Category="Grid|Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(VisibleAnywhere,Category="Grid|Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere,Category="Grid|Components")
	TObjectPtr<UArrowComponent> ArrowComponent;
};
