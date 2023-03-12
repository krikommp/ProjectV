// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"

#include "GridChessPieceSkillComponent.generated.h"

class UGridAbilitySystemComponent;
class UGridHeroInfo;
class UGridSkillInfo;
class UGridGameplayAbility_Card;

/**
 * UGridChessPieceSkillComponent
 *
 * 技能组件
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessPieceSkillComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UGridChessPieceSkillComponent(const FObjectInitializer &ObjectInitialize);

	UFUNCTION(BlueprintPure, Category = "Grid|ChessPieceSkill")
	static UGridChessPieceSkillComponent *FindChessPieceSkillComponent(const AActor *Actor) { return Actor ? Actor->FindComponentByClass<UGridChessPieceSkillComponent>() : nullptr; }

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceSkill")
	UGridSkillInfo* FindSkillInfoByAbility(TSubclassOf<UGridGameplayAbility_Card> AbilityClass) const;

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceSkill")
	FORCEINLINE UGridAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	void OnChessPieceInitialized();
private:

	UPROPERTY(Transient)
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	TWeakObjectPtr<UGridHeroInfo> HeroInfo;
};
