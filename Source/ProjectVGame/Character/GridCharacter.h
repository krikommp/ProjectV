// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "ModularCharacter.h"
#include "GameplayTagAssetInterface.h"
#include "GridCharacter.generated.h"

class UGridPawnExtensionComponent;
class UGridAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;

/**
 * AGridCharacter
 *
 * 项目最基本的可控制对象
 * 在战旗中可以被视为 Master
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class PROJECTVGAME_API AGridCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AGridCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Grid|Character")
	UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|TurnBaseMaster")
	void MouseWheelDown(USpringArmComponent* InSpringArmComponent, float SpringArmStartLength, float DistanceToZoom, float ZoomRoof);

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|TurnBaseMaster")
	void MouseWheelUp(USpringArmComponent* InSpringArmComponent, float SpringArmStartLength, float DistanceToZoom, float ZoomFloor);
protected:

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGridPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;
};
