// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapStateComponent.h"

#include "GridLogChannel.h"
#include "GridMapFunctionLibrary.h"
#include "GridMapManager.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GameModes/GridGameState.h"
#include "Kismet/GameplayStatics.h"

UGridMapStateComponent::UGridMapStateComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	GridMapManager = nullptr;
	bGridMapInitialized = false;
}

void UGridMapStateComponent::OnRegister()
{
	Super::OnRegister();

	AGridGameState* GameState = GetGameStateChecked<AGridGameState>();
	if (UGridExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UGridExperienceManagerComponent>())
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("[UGridPlayerHandStateComponent::OnRegister]：Can not found ExperienceManagerComponent on GameState!"));
	}
}

void UGridMapStateComponent::CallOrRegister_OnGridMapInitialized(FOnGridMapMangerInitielized::FDelegate&& Delegate)
{
	if (bGridMapInitialized)
	{
		Delegate.Execute(GridMapManager);
	}
	if (!OnGridMapInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnGridMapInitialized.Add(Delegate);
	}
}

void UGridMapStateComponent::OnExperienceLoaded(const UGridExperienceDefinition* Experience)
{
	TArray<AActor*> OuterActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridMapManager::StaticClass(), OuterActors);
	if (OuterActors.Num() == 0)
	{
		UE_LOG(LogGrid, Error, TEXT("No GridMap in this Level!"));
		return;
	}
	if (OuterActors.Num() > 1)
	{
		UE_LOG(LogGrid, Warning, TEXT("Move than one GridMap in this Level, will select the first [%s] GridMap."), *GetNameSafe(OuterActors[0]));
	}
	GridMapManager = Cast<AGridMapManager>(OuterActors[0]);
	ActivateGridMap();
}

void UGridMapStateComponent::ActivateGridMap()
{
	check(GridMapManager);
	UGridMapFunctionLibrary::CreateNewGridMapDataAsset(GridMapManager);
	GridMapManager->InitializeVisibleComponent();
	GridMapManager->InitializeDecalAndPathComponent();
	GridMapManager->PawnArray.SetNumZeroed(GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ);
	if (OnGridMapInitialized.IsBound())
	{
		OnGridMapInitialized.Broadcast(GridMapManager);
	}
	bGridMapInitialized = true;
}
