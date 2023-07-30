// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "GameFramework/Pawn.h"
#include "MasterCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UGridPawnExtensionComponent;
class UArrowComponent;
class UCapsuleComponent;

UCLASS()
class PROJECTVGAME_API AMasterCharacter : public AModularPawn
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ~Begin Pawn interface.
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	// ~Begin Pawn interface.
	
private:
	UPROPERTY(VisibleAnywhere, Category="Gird|Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(VisibleAnywhere, Category="Gird|Components")
	TObjectPtr<UArrowComponent> ArrowComponent;
	UPROPERTY(VisibleAnywhere, Category="Gird|Components")
	TObjectPtr<UGridPawnExtensionComponent> ExtensionComponent;
	UPROPERTY(VisibleAnywhere, Category="Gird|Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere, Category="Gird|Components")
	TObjectPtr<UCameraComponent> CameraComponent;
};
