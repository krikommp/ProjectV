// Fill out your copyright notice in the Description page of Project Settings.


#include "GridHUD.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/EngineBaseTypes.h"
#include "UObject/UObjectIterator.h"

AGridHUD::AGridHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AGridHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGridHUD::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AGridHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AGridHUD::GetDebugActorList(TArray<AActor*>& InOutList)
{
	UWorld* World = GetWorld();

	Super::GetDebugActorList(InOutList);

	// Add all actors with an ability system component.
	for (TObjectIterator<UAbilitySystemComponent> It; It; ++It)
	{
		if (UAbilitySystemComponent* ASC = *It)
		{
			if (!ASC->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
			{
				AActor* AvatarActor = ASC->GetAvatarActor();
				AActor* OwnerActor = ASC->GetOwnerActor();

				if (AvatarActor && UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AvatarActor))
				{
					AddActorToDebugList(AvatarActor, InOutList, World);
				}
				else if (OwnerActor && UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor))
				{
					AddActorToDebugList(OwnerActor, InOutList, World);
				}
			}
		}
	}
}
