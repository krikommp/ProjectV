// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridGlobalAbilitySystem.generated.h"

class UGridAbilitySystemComponent;

/**
 * UGridGlobalAbilitySystem
 *
 * 管理全局 ASC
 */
UCLASS()
class PROJECTVGAME_API UGridGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UGridGlobalAbilitySystem();

	void RegisterASC(UGridAbilitySystemComponent* ASC);

	void UnregisterASC(UGridAbilitySystemComponent* ASC);
	
private:

	UPROPERTY()
	TArray<TObjectPtr<UGridAbilitySystemComponent>> RegisteredASCs;
};
