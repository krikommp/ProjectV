// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapStateComponent.h"

#include "GridLogChannel.h"
#include "Tilemap3DActor.h"
#include "TilemapAsset.h"
#include "TilemapExtensionComponent.h"
#include "GameFramework/Character.h"
#include "GameModes/GridExperienceDefinition.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GameModes/GridGameState.h"
#include "Kismet/GameplayStatics.h"

UTilemapStateComponent::UTilemapStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), TilemapActor(nullptr), bLoadTilemapFinished(false)
{
}

void UTilemapStateComponent::OnRegister()
{
	Super::OnRegister();

	if (UGridExperienceManagerComponent* ExperienceManagerComponent = GetGameState<AGridGameState>()->FindComponentByClass<UGridExperienceManagerComponent>())
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("[UTilemapStateComponent::OnRegister]: Can not found %s on Game State"), *GetNameSafe(this));
	}
}

bool UTilemapStateComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (bLoadTilemapFinished == false)
	{
		OutReason = TEXT("Tilemap Asset still loading.");
		return true;
	}
	return false;
}

void UTilemapStateComponent::OnExperienceLoaded(const UGridExperienceDefinition* Experience)
{
	// 首先尝试从场景中获取Tilemap Actor
	TArray<AActor*> OuterActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATilemap3DActor::StaticClass(), OuterActors);

	if (OuterActors.Num() >= 1)
	{
		UE_LOG(LogGrid, Verbose, TEXT("Multiple TilemapActors found in the scene, prioritizing the first TilemapActor."));
		TilemapActor = Cast<ATilemap3DActor>(OuterActors[0]);
		LoadTilemapFinished_Step1();
	}else
	{
		// 当前场景中没有手动放置任何 TilemapActor
		// 尝试自动创建一个 Actor 并放置到场景中
		TilemapActor = GetWorld()->SpawnActor<ATilemap3DActor>();
		TilemapActor->SetActorTransform(FTransform::Identity);
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UTilemapStateComponent::LoadTilemapFinished_Step1));
	}
}

void UTilemapStateComponent::LoadTilemapFinished_Step1()
{
	const UGridExperienceManagerComponent* ExperienceManagerComponent = GetGameState<AGridGameState>()->FindComponentByClass<UGridExperienceManagerComponent>();
	if (ExperienceManagerComponent == nullptr)
		return;
	if (ExperienceManagerComponent->GetCurrentExperienceChecked()->TilemapAsset == nullptr)
		return;
	// 尝试获取默认玩家控制对象
	const UTilemapAsset* TilemapAsset = ExperienceManagerComponent->GetCurrentExperienceChecked()->TilemapAsset;
	const APlayerController* DefaultPlayerController = GetWorld()->GetFirstPlayerController();
	APawn* Character = DefaultPlayerController->GetPawn();
	Character->SetActorTransform(TilemapAsset->PlayerStart);

	// 为TilemapActor设置TilemapAsset
	TilemapActor->SetupTilemapAsset(TilemapAsset);

	// 设置 Tilemap 数据到主相机
	if (UTilemapExtensionComponent* TilemapExtensionComponent = UTilemapExtensionComponent::FindTilemapExtensionComponent(Character))
	{
		TilemapExtensionComponent->SetTilemap(TilemapActor);
	}

	LoadTilemapFinished_Step2();
}

void UTilemapStateComponent::LoadTilemapFinished_Step2()
{
	bLoadTilemapFinished = true;
}
