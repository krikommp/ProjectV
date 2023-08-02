// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapExtensionComponent.h"

#include "GridGameplayTags.h"
#include "Character/GridPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "TilemapAsset.h"
#include "TilemapStateComponent.h"
#include "Player/GridPlayerState.h"
#include "Chess/GridChessBase.h"

const FName UTilemapExtensionComponent::NAME_ActorFeatureName("TilemapExtension");

UTilemapExtensionComponent::UTilemapExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	Tilemap3DActor = nullptr;
}

void UTilemapExtensionComponent::CheckLocationOutBound(FVector& InLocation) const
{
	check(Tilemap3DActor->GetTilemap());
	
	const float XValue = Tilemap3DActor->GetTilemap()->GridSize * Tilemap3DActor->GetTilemap()->LevelSizeX - Tilemap3DActor->GetTilemap()->MapBoundOffset;
	const float YValue = Tilemap3DActor->GetTilemap()->GridSize * Tilemap3DActor->GetTilemap()->LevelSizeY - Tilemap3DActor->GetTilemap()->MapBoundOffset;

	InLocation.X = Tilemap3DActor->GetActorLocation().X + FMath::Max(InLocation.X < XValue ? InLocation.X : XValue, Tilemap3DActor->GetTilemap()->MapBoundOffset);
	InLocation.Y = Tilemap3DActor->GetActorLocation().Y + FMath::Max(InLocation.Y < YValue ? InLocation.Y : YValue, Tilemap3DActor->GetTilemap()->MapBoundOffset);
}

bool UTilemapExtensionComponent::CheckChessExist(int32 Index) const
{
	if (!Tilemap3DActor->ChessArray.IsValidIndex(Index))
		return false;
	return Tilemap3DActor->ChessArray[Index] == nullptr;
}

int32 UTilemapExtensionComponent::GetPathfindingArrayNum() const
{
	return Tilemap3DActor->GetPathfindingArrayNum();
}

const FTilemapPathFindingBlock& UTilemapExtensionComponent::GetPathfindingBlock(int32 Index) const
{
	ensureAlwaysMsgf(Tilemap3DActor->GetTilemap()->PathFindingBlocks.IsValidIndex(Index), TEXT("InValid pathfinding Block Index: [%d]!"), Index);

	return Tilemap3DActor->GetTilemap()->PathFindingBlocks[Index];
}

void UTilemapExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("UTilemapExtensionComponent on [%s] can only be added to Pawn actors."),
	                 *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> Components;
	Pawn->GetComponents(UTilemapExtensionComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one UTilemapExtensionComponent should exist on [%s]."),
	                 *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();

	if (UTilemapStateComponent* TilemapStateComponent = GetWorld()->GetGameState()->FindComponentByClass<UTilemapStateComponent>())
	{
		TilemapStateComponent->CallOrRegister_OnChessSpawn(FOnTilemapSpawnChess::FDelegate::CreateUObject(this, &ThisClass::OnChessSpawn));
	}
}

void UTilemapExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 监听 PawnExtComp 的状态改变事件
	BindOnActorInitStateChanged(UGridPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// 当前我们已经完成了 Spawn，需要重置当前的初始化状态
	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UTilemapExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UTilemapExtensionComponent::OnChessSpawn(const FTilemapSpawnParameters& Parameters)
{
	if (Parameters.Chess != GetPawn<AGridChessBase>())
		return;
	
	SetTilemap(Parameters.Tilemap, Parameters.PathfindingIndex);
}

bool UTilemapExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
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
		if (Tilemap3DActor == nullptr)
		{
			return false;
		}
		return true;
	}
	else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		AGridPlayerState* GridPS = GetPlayerState<AGridPlayerState>();

		// 这里是判断 PawnExtComp 的数据是否准备完毕
		// 注意 PawnExtComp 将会首先进入 InitState_DataInitialized 状态，然后该 Feature 再进
		// 默认可控制对象上面都会挂载一个 PawnExtComp
		return GridPS && Manager->HasFeatureReachedInitState(Pawn, UGridPawnExtensionComponent::NAME_ActorFeatureName,
		                                                     InitTags.InitState_DataInitialized);
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	return false;
}

void UTilemapExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
                                                       FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// 当执行到第三步初始化，即该组件依赖的其他组件都完成了相应的数据初始化操作
	if (CurrentState == FGridGameplayTags::Get().InitState_DataAvailable && DesiredState == FGridGameplayTags::Get().
		InitState_DataInitialized)
	{
		// todo... 接下来执行剩余初始化
	}
}

void UTilemapExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UGridPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGridGameplayTags::Get().InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UTilemapExtensionComponent::CheckDefaultInitialization()
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = {
		InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}

void UTilemapExtensionComponent::SetTilemap(const ATilemap3DActor* InTilemap, int32 InPathfindingIndex)
{
	check(InTilemap);

	Tilemap3DActor = InTilemap;
	PathfindingIndex = InPathfindingIndex;

	CheckDefaultInitialization();
}
