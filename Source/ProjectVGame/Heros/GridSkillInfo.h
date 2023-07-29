// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GridSkillData.h"
#include "AbilitySystem/AbilityEvents/GridAbilityInputEvent.h"
#include "GameFramework/SaveGame.h"

#include "GridSkillInfo.generated.h"

class UGridAbilitySystemComponent;
class AGridChessPiece;
class UGridLocalPlayer;

/**
 * UGridSkillInfo
 *
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridSkillInfo : public USaveGame
{
	GENERATED_BODY()
	friend class UGridChessPieceSkillComponent;

public:
	UGridSkillInfo();

	static UGridSkillInfo* CreateOrLoadSkillInfo(const UGridLocalPlayer* InLocalPlayer, const FName& InHeroId, const FGridSkillData& InSkillData);

	FString GetSaveSlotName() const;

	UFUNCTION(BlueprintPure, Category="Grid|Skill")
	bool CheckAbilityCanActivate() const;

	UFUNCTION(BlueprintPure, Category="Grid|Skill")
	UGridGameplayAbility_Card* GetAbility() const;

	UFUNCTION(BlueprintCallable, Category="Grid|Skill")
	bool TryActivateSkill();

	UFUNCTION(BlueprintCallable, Category="Grid|Skill")
	void SendInputEvent(const FGridAbilityInputEvent& InputEvent);
public:

	UPROPERTY(BlueprintReadOnly)
	FName ID;

	UPROPERTY(BlueprintReadOnly)
	FName HeroId;

	UPROPERTY(BlueprintReadOnly)
	int32 Level;

	UPROPERTY(BlueprintReadOnly, Transient)
	FGridSkillData SkillData;

private:
	UPROPERTY(Transient)
	AGridChessPiece* Owner;
	
	UPROPERTY(Transient)
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	FGameplayAbilitySpecHandle AbilitySpecHandle;
};
