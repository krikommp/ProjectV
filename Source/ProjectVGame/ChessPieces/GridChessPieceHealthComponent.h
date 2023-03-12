

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "GridChessPieceHealthComponent.generated.h"

class UGridAbilitySystemComponent;
class UGridHealthSet;
class UGridChessPieceHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridHealth_DealthEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGridHealth_AttributeChanged, UGridChessPieceHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);


/**
 *	EGridDeathState
 *
 *	死亡状态
 */
UENUM(BlueprintType)
enum class EGridDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

/**
 * UGridChessPieceHealthComponent
 *
 * 管理 chess piece 生命相关的组件
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessPieceHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	UGridChessPieceHealthComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceHealth")
	static UGridChessPieceHealthComponent* FindChessPieceHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGridChessPieceHealthComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceHealth")
	void InitializeWithAbilitySystem(UGridAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceHealth")
	void UnInitializeFromAbilitySystem();

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceHealth")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceHealth")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceHealth")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceHealth")
	int32 GetDefense() const;

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceHealth")
	EGridDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Grid|ChessPieceHealth", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EGridDeathState::NotDead); }

	virtual void StartDeath();

	virtual void FinishDeath();

	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

protected:
	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(const struct FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const struct FOnAttributeChangeData& ChangeData);
	virtual void HandleDefenseChanged(const struct FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const struct FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);
	virtual void HandleOutOfDefense(AActor* DamageInstigator, AActor* DamageCauser, const struct FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

public:

	UPROPERTY(BlueprintAssignable)
	FGridHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FGridHealth_AttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FGridHealth_AttributeChanged OnDefenseChanged;

	UPROPERTY(BlueprintAssignable)
	FGridHealth_DealthEvent OnDeathStarted;

	UPROPERTY(BlueprintAssignable)
	FGridHealth_DealthEvent OnDeathFinished;
	
protected:

	UPROPERTY()
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UGridHealthSet> HealthSet;

	UPROPERTY()
	EGridDeathState DeathState;
};
