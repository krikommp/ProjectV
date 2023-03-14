

#pragma once

#include "CoreMinimal.h"
#include "GridGameplayAbility.h"
#include "AbilitySystem/AbilityEffects/GridGameplayEffectContainer.h"
#include "UI/PlayerHand/GridPlayerHand.h"
#include "GridGameplayAbility_ChessPiece.generated.h"

class AGridChessPiece;
class UGridChessPieceExtensionComponent;
class AGridMapManager;

/**
 * UGridGameplayAbility_ChessPiece
 *
 * 适用于棋子的 gameplay ability 
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by chess piece."))
class PROJECTVGAME_API UGridGameplayAbility_ChessPiece : public UGridGameplayAbility
{
	GENERATED_BODY()

	friend class UGridAbilitySystemComponent;
public:

	UGridGameplayAbility_ChessPiece(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Grid|Ability")
	UGridAbilitySystemComponent* GetGridAbilitySystemComponentFormActorInfo() const;

	UFUNCTION(BlueprintCallable, Category="Grid|Ability")
	AGridChessPiece* GetGridChessPieceFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category="Grid|Ability")
	UGridChessPieceExtensionComponent* GetChessPieceExtensionComponent() const;

	UFUNCTION(BlueprintCallable, Category="Grid|Ability")
	AGridMapManager* GetGridMapManager() const;

	// ~GameplayEffect

	UFUNCTION(BlueprintCallable, Category="Grid|Ability", meta=(AutoCreateRefTerm = "EventData"))
	virtual FGridGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FGridGameplayEffectContainer& Container, int32 TargetTileIndex, int32 Range, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category="Grid|Ability", meta=(AutoCreateRefTerm = "EventData"))
	virtual FGridGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, int32 TargetTileIndex, int32 Range, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category="Grid|Ability")
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FGridGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintCallable, Category="Grid|Ability", meta=(AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag GameplayTag, int32 TargetTileIndex, int32 Range, const FGameplayEventData& EventData);
	
	// ~GameplayEffect

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameplayEffect")
	TMap<FGameplayTag, FGridGameplayEffectContainer> EffectContainers;
};
