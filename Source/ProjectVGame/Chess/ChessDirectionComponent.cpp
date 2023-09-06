// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessDirectionComponent.h"

#include "ChessDirectionArrow.h"
#include "GridGameplayTags.h"
#include "Character/GridPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Tilemap/TilemapExtensionComponent.h"

const FName UChessDirectionComponent::NAME_ActorFeatureName(TEXT("ChessDirectionComponent"));

UChessDirectionComponent::UChessDirectionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), LookAtIndex(INDEX_NONE)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}


TArray<int32> UChessDirectionComponent::GetNeighborIndexes() const
{
	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not found TilemapExtensionComponent!"))
		return TArray<int32>();
	}

	const FTilemapPathFindingBlock& PathfindingBlock = TilemapExtensionComponent->GetPathfindingBlockSelf();
	TArray<int32> NeighborIndexes;
	NeighborIndexes.Append(PathfindingBlock.EdgeArrayIndex);

	return NeighborIndexes;
}

const FTilemapPathFindingBlock& UChessDirectionComponent::GetNeighborPathfindingBlock(int32 EdgeIndex) const
{
	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	check(TilemapExtensionComponent);

	const FTilemapPathFindingBlock& PathfindingBlock = TilemapExtensionComponent->GetPathfindingBlockSelf();
	check(PathfindingBlock.EdgeArrayIndex.Contains(EdgeIndex));

	return TilemapExtensionComponent->GetPathfindingBlock(EdgeIndex);
}

void UChessDirectionComponent::SetLookAtIndex(int32 Index)
{
	APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	if (LookAtIndex == Index)
		return;

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not found TilemapExtensionComponent!"))
		return;
	}
	
	if (!TilemapExtensionComponent->GetTilemap()->PathFindingBlocks.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Error, TEXT("InValid pathfinding Block Index: [%d]!"), Index);
		return;
	}

	const FTilemapPathFindingBlock& LookAtPathfindingBlock = TilemapExtensionComponent->GetPathfindingBlock(Index);
	const FVector LookAtLocation = LookAtPathfindingBlock.Location;

	const FVector SelfLocation = Pawn->GetActorLocation();
	FVector Direction = LookAtLocation - SelfLocation;
	Direction.Z = 0.f;
	FRotator NewRotation = Direction.Rotation();
	NewRotation.Yaw -= 90.0f;
	
	Pawn->SetActorRotation(NewRotation);

	LookAtIndex = Index;
}

void UChessDirectionComponent::ShowAllDirectionArrows()
{
	const APawn* Pawn = GetPawn<APawn>();

	int32 Index = 0;
	TArray<int32> EdgeIndexes = GetNeighborIndexes();
	for (const int32 EdgeIndex : EdgeIndexes)
	{
		if (!DirectionArrows.IsValidIndex(Index))
			break;

		const auto& Edge = GetNeighborPathfindingBlock(EdgeIndex);

		const FVector LookAtLocation = Edge.Location;

		const FVector SelfLocation = Pawn->GetActorLocation();
		FVector Direction = LookAtLocation - SelfLocation;
		Direction.Z = 0.f;
		const FRotator NewRotation = Direction.Rotation();
			
		DirectionArrows[Index]->SetActorLocation(Edge.Location);
		DirectionArrows[Index]->SetActorRotation(NewRotation);
		DirectionArrows[Index]->SetTargetIndex(EdgeIndex);
		DirectionArrows[Index]->SetActorHiddenInGame(false);
		DirectionArrows[Index]->EnableCollision(true);

		++Index;
	}
}

void UChessDirectionComponent::HideAllDirectionArrows()
{
	for (const auto& ArrowComp : DirectionArrows)
	{
		ArrowComp->EnableCollision(false);
		ArrowComp->SetActorHiddenInGame(true);
	}
}

void UChessDirectionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("UTilemapExtensionComponent on [%s] can only be added to Pawn actors."),
					 *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> Components;
	Pawn->GetComponents(UChessDirectionComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one UChessDirectionComponent should exist on [%s]."),
					 *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UChessDirectionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 监听 PawnExtComp 的状态改变事件
	BindOnActorInitStateChanged(UTilemapExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// 当前我们已经完成了 Spawn，需要重置当前的初始化状态
	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UChessDirectionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

bool UChessDirectionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                  FGameplayTag DesiredState) const
{
	check(Manager);

	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// pawn 生成完毕，准备进入下一步初始化流程
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
		if (TilemapExtensionComponent != nullptr && TilemapExtensionComponent->IsValid())
		{
			return true;
		}
	}
	else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		return true;
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	return false;
}

void UChessDirectionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	if (CurrentState == FGridGameplayTags::Get().InitState_DataAvailable && DesiredState == FGridGameplayTags::Get().
			InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		check(Pawn);

		const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
		check(TilemapExtensionComponent);

		for (int32 Index = 0; Index < 4; ++Index)
		{
			AChessDirectionArrow* Arrow = Cast<AChessDirectionArrow>(Pawn->GetWorld()->SpawnActor(AChessDirectionArrow::StaticClass()));

			FChessArrowInitParams ArrowInitParams;
			ArrowInitParams.ArrowMesh = ArrowMesh;
			ArrowInitParams.ArrowMaterial = ArrowMat;
			ArrowInitParams.OwnerPawn = Pawn;

			Arrow->InitializeArrow(ArrowInitParams);
			Arrow->SetActorHiddenInGame(true);
			DirectionArrows.Add(Arrow);
		}
		
		if (TilemapExtensionComponent->GetTilemap()->bDiagonalMovement)
		{
			for (int32 Index = 0; Index < 4; ++Index)
			{
				AChessDirectionArrow* Arrow = Cast<AChessDirectionArrow>(Pawn->GetWorld()->SpawnActor(AChessDirectionArrow::StaticClass()));

				FChessArrowInitParams ArrowInitParams;
				ArrowInitParams.ArrowMesh = ArrowMesh;
				ArrowInitParams.ArrowMaterial = ArrowMat;
				ArrowInitParams.OwnerPawn = Pawn;

				Arrow->InitializeArrow(ArrowInitParams);
				Arrow->SetActorHiddenInGame(true);
				DirectionArrows.Add(Arrow);
			}
		}
	}
}

void UChessDirectionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UTilemapExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGridGameplayTags::Get().InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UChessDirectionComponent::CheckDefaultInitialization()
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = {
		InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}
