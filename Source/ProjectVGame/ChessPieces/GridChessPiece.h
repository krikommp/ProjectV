// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "AbilitySystemInterface.h"
#include "GridChessPiece.generated.h"

class UGridLocalPlayer;
class UGridChessPieceExtensionComponent;
class UGridChessPieceHealthComponent;
class UGridAbilitySystemComponent;
class UGridChessPieceComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;
class UArrowComponent;
class UGridActionSet;
class UGridHealthSet;
class UGridMoveSet;
class UGridCombatSet;

/**
 * AGridChessPiece
 *
 * 基本的棋子单位
 */
UCLASS(Abstract, Config=Game)
class PROJECTVGAME_API AGridChessPiece : public AModularPawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGridChessPiece(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ~APawn interface
	virtual void SpawnDefaultController() override;
	// ~APawn interface
	
	UFUNCTION(BlueprintCallable, Category="Grid|PlayerState")
	UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void SetupSkeletalMeshAsset(USkeletalMesh* SkeletalMesh) const;

	DECLARE_EVENT(AGridChessPiece, FOnTurnSwicthDelegate);
	FOnTurnSwicthDelegate& OnTurnStartDelegate() const { return OnTurnStartEvent;  }
	FOnTurnSwicthDelegate& OnTurnFinishDelegate() const { return OnTurnFinishEvent; }

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|Turn")
	void OnTurnStart();
	void OnTurnStartInternal();

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|Turn")
	void OnTurnOver();
	void OnTurnOverInternal();

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|Turn", meta=(DisplayName="OnChessPieceSelect"))
	void K2_OnChessPieceSelect();
	void OnChessPieceSelect();

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|Turn", meta=(DisplayName="OnChessPieceUnSelect"))
	void K2_OnChessPieceUnselect();
	void OnChessPieceUnSelect();

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|UI", meta=(DisplayName="OnGameplayEffectActivate"))
	void K2_OnGameplayEffectActivate(const UGridAbilityBuffUIData* BuffUIData, float Duration);

	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnCursorHoverBegin"))
	void K2_OnCursorHoverBegin();
	void OnCursorHoverBegin();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnCursorHoverEnd"))
	void K2_OnCursorHoverEnd();
	void OnCursorHoverEnd();
	
protected:
	
	virtual void BeginPlay() override;
	virtual bool IsLocallyControlled() const override;
	
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();
	virtual void OnChessPieceInfoInitialized();
	
	void InitializeGameplayTags();

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

 	mutable FOnTurnSwicthDelegate OnTurnStartEvent;

	mutable FOnTurnSwicthDelegate OnTurnFinishEvent;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid|Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGridChessPieceComponent> ChessPieceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGridChessPieceExtensionComponent> ChessPieceExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGridChessPieceHealthComponent> ChessPieceHealthComponent;

	UPROPERTY(VisibleAnywhere, Category="Grid|Piece")
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category="Gird|Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere,Category="Grid|Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere,Category="Grid|Components")
	TObjectPtr<UArrowComponent> ArrowComponent;

private:

	UPROPERTY(Transient)
	TObjectPtr<UGridActionSet> ActionSet;

	UPROPERTY(Transient)
	TObjectPtr<UGridHealthSet> HealthSet;

	UPROPERTY(Transient)
	TObjectPtr<UGridCombatSet> CombatSet;

	UPROPERTY(Transient)
	TObjectPtr<UGridMoveSet> MoveSet;
};
