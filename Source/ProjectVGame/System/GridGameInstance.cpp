// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameInstance.h"

#include "GridGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/GridPlayerController.h"

UGridGameInstance::UGridGameInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGridGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		const FGridGameplayTags& GameplayTags = FGridGameplayTags::Get();

		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void UGridGameInstance::Shutdown()
{
	Super::Shutdown();
}

AGridPlayerController* UGridGameInstance::GetPrimaryPlayerController() const
{
	return Cast<AGridPlayerController>(Super::GetPrimaryPlayerController(false));
}

bool UGridGameInstance::CanJoinRequestedSession() const
{
	// Temporary first pass:  Always return true
	// This will be fleshed out to check the player's state
	if (!Super::CanJoinRequestedSession())
	{
		return false;
	}
	return true;
}


