// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceMovementComponent.h"
#include "GridChessPiece.h"
#include "GridLogChannel.h"
#include "GridChessPieceExtensionComponent.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GridMoveSet.h"
#include "AbilitySystem/Attributes/GridActionSet.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

#include "AbilitySystemBlueprintLibrary.h"
#include "GridGameplayTags.h"
#include "GridGlobalDelegates.h"
#include "AbilitySystem/Abilities/GridGameAbility_Move.h"
#include "Character/GridTurnBaseMasterComponent.h"
#include "GridMapManager/GridMapFunctionLibrary.h"
#include "GridMapManager/GridMapManager.h"
#include "GridMapManager/GridMapStateComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridChessPieceMovementComponent)

UGridChessPieceMovementComponent::UGridChessPieceMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}


int32 UGridChessPieceMovementComponent::GetMoveRange() const
{
	return FMath::Floor(ActionSet ? ActionSet->GetActionPoint() / GetMovePreStepCost() : 0.0f);
}

int32 UGridChessPieceMovementComponent::GetMaxMoveRange() const
{
	return FMath::Floor(ActionSet ? ActionSet->GetMaxActionPoint() / GetMovePreStepCost() : 0.0f);
}

int32 UGridChessPieceMovementComponent::GetMovePreStepCost() const
{
	return FMath::Floor(ActionSet ? (ActionSet->GetMovePreStepCost() * ActionSet->GetMoveCostRatio()) : 0.0f);
}

int32 UGridChessPieceMovementComponent::GetVisibleRange() const
{
	return FMath::Floor(ActionSet ? ActionSet->GetVisibleRange() : 0.0f);
}

float UGridChessPieceMovementComponent::GetMaxSpeed() const
{
	return (MoveSet ? MoveSet->GetMaxSpeed() : 0.0f);
}

float UGridChessPieceMovementComponent::GetAcceleration() const
{
	return (MoveSet ? MoveSet->GetAcceleration() : 0.0f);
}

void UGridChessPieceMovementComponent::StartToPathFinding(bool bDisplayTiles)
{
	check(GridMapManager.IsValid());

	const APawn* Pawn = GetPawnChecked<APawn>();

	if (const UGridChessPieceExtensionComponent* ChessPieceExtComp =
		UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Pawn))
	{
		const auto VisibleTileIndexArray = GridMapManager->FindTilesInRange(ChessPieceExtComp->GetTileIndex(),
		                                                                    GetVisibleRange(), false, true,
		                                                                    (GridMapManager->TileBoundsX +
			                                                                    GridMapManager->
			                                                                    TileBoundsY) * 0.5f * 0.5f,
		                                                                    0.0f, true);
		const auto MoveTileIndexArray = GridMapManager->PathFinding(
			ChessPieceExtComp->GetTileIndex(),
			GetMoveRange(),
			GetMaxMoveRange(),
			bDisplayTiles,
			false);
		if (bDisplayTiles)
		{
			//GridMapManager->DisplayRangeMarkers(VisibleTileIndexArray, GridMapManager->TileInSightRangeDecal);
			// GridMapManager->DisplayRangeMarkers(MoveTileIndexArray, GridMapManager->TileInMoveRangeDecal, true);
			//UGridMapFunctionLibrary::SpawnEdgeMeshes(GridMapManager, GridMapManager->CanMoveToArray, Grid)
			UGridMapFunctionLibrary::DisplayMoveRangeEdgeMarkers(GridMapManager.Get(),
			                                                     ChessPieceExtComp->GetTileIndex(),
			                                                     GridMapManager->CanMoveToArray,
			                                                     GridMapManager->IndexCanMoveToArray);
			//UGridMapFunctionLibrary::DisplayInsightRangeEdgeMarkers(GridMapManager.Get(), GridMapManager->TilesInSightArray, GridMapManager->RangeArray);
		}
	}
}

void UGridChessPieceMovementComponent::EndPathFinding()
{
	check(GridMapManager.IsValid());

	GridMapManager->ClearAllInRangeMeshMarkers();
}

bool UGridChessPieceMovementComponent::DecideWhatToDoWhenHover(int32 HoverTileIndex)
{
	check(GridMapManager.IsValid());

	if (GridMapManager->IsTileImpassable(HoverTileIndex))
	{
		GridMapManager->RemoveDisplayedPath();
		return false;
	}

	GridMapManager->SetMarkerLocationUseIndex(HoverTileIndex, GridMapManager->HoverMarkerDecal);
	GridMapManager->SetMarkerVisibility(true, GridMapManager->HoverMarkerDecal);
	GridMapManager->SetMarkerVisibility(false, GridMapManager->TargetMarkerDecal);

	if (GridMapManager->PawnArray.IsValidIndex(HoverTileIndex) && GridMapManager->PawnArray[HoverTileIndex])
	{
		GridMapManager->RemoveDisplayedPath();
		return false;
	}

	if (GridMapManager->CanMoveToArray[HoverTileIndex].Cost <= 0)
	{
		GridMapManager->RemoveDisplayedPath();
		return false;
	}

	GridMapManager->FindPathWithinPathfindingArray(HoverTileIndex, true, true, true);
	return true;
}

void UGridChessPieceMovementComponent::DecideActionForClickedTile(APlayerController* InController,
                                                                  int32 ClickedTileIndex)
{
	check(GridMapManager.IsValid());

	const APawn* Player = InController->GetPawn();
	if (UGridTurnBaseMasterComponent* TurnBaseMasterComponent =
		UGridTurnBaseMasterComponent::FindTurnBaseMasterComponent(Player))
	{
		TurnBaseMasterComponent->SetIsPanning(false);
	}

	if (GridMapManager->CanMoveToArray[ClickedTileIndex].Cost <= 0)
	{
		return;
	}

	BeginMovement(ClickedTileIndex);
}

void UGridChessPieceMovementComponent::BeginMovement(int32 InTileIndex, int32 InStopXFromTarget)
{
	check(GridMapManager.IsValid());

	AGridChessPiece* Owner = GetPawnChecked<AGridChessPiece>();

	GridMapManager->FindPathWithinPathfindingArray(InTileIndex, true, false, false, InStopXFromTarget);

	ActivateMovement(InTileIndex, InStopXFromTarget);
}

void UGridChessPieceMovementComponent::ActivateMovement(int32 InTileIndex, int32 InStopXFromTarget)
{
	check(GridMapManager.IsValid());

	AGridChessPiece* Owner = GetPawnChecked<AGridChessPiece>();

	const FGridGameplayTags GameplayTags = FGridGameplayTags::Get();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, GameplayTags.Ability_Behavior_Move,
	                                                         FGameplayEventData());
}

bool UGridChessPieceMovementComponent::CheckNeedMove(int32 InTileIndex, int32 InStopXFromTarget, bool bDisplayPath,
                                                     int32& OutEndTileIndex,
                                                     TArray<FStructPathFinding>& OutCanMoveToArray)
{
	check(GridMapManager.IsValid());

	AGridChessPiece* Owner = GetPawnChecked<AGridChessPiece>();

	const UGridChessPieceExtensionComponent* ChessPieceExtComp =
		UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Owner);

	check(ChessPieceExtComp);

	const int32 Distance = GridMapManager->FindDistanceInTilesBetweenIndexes(
		ChessPieceExtComp->GetTileIndex(), InTileIndex);

	if (Distance > GetMoveRange())
	{
		//GridMapManager->PathFinding(ChessPieceExtComp->GetTileIndex(), Distance, Distance, true, false, false);
		TArray<FStructPathFinding> NewIndexCanMoveToArray;
		TArray<int32> NewReachablePawnsArray;
		int32 NewCurrentSearchStep = 0;
		TArray<int32> NewTileIndexes;
		OutCanMoveToArray.Empty();
		for (int32 Index = 0; Index < GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->
		     GridSizeZ; ++Index)
		{
			OutCanMoveToArray.Add({0, 0, 0});
		}
		GridMapManager->PathFinding_Internal(ChessPieceExtComp->GetTileIndex(), Distance, Distance, true, false,
		                                     false, NewCurrentSearchStep, OutCanMoveToArray, NewIndexCanMoveToArray,
		                                     NewTileIndexes, NewReachablePawnsArray);
	}
	else
	{
		OutCanMoveToArray = GridMapManager->CanMoveToArray;
	}

	const TArray<int32>& LocalPathIndexArray = GridMapManager->FindPathToIndex(
		OutCanMoveToArray, InTileIndex, InStopXFromTarget);
	if (LocalPathIndexArray.IsEmpty() || LocalPathIndexArray.Num() == 1)
		return false;

	GridMapManager->CreateSplinePath(LocalPathIndexArray);

	if (bDisplayPath)
	{
		GridMapManager->DisplayPathAsSpline();
	}

	OutEndTileIndex = LocalPathIndexArray[0];

	return true;
}


bool UGridChessPieceMovementComponent::CheckMove(TSubclassOf<UGridGameAbility_Move> InMoveAbility) const
{
	if (!AbilitySystemComponent) return false;

	if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InMoveAbility))
	{
		if (AbilitySpec->Ability)
		{
			return AbilitySpec->Ability->CanActivateAbility(AbilitySpec->Handle,
			                                                AbilitySystemComponent->AbilityActorInfo.Get());
		}
	}

	return false;
}

void UGridChessPieceMovementComponent::OnRegister()
{
	Super::OnRegister();

	FGridGlobalDelegates::OnChessPieceSelectChanged.AddUObject(this, &ThisClass::HandleChessPieceSelectChanged);
	FGridGlobalDelegates::OnChessPieceTurnOver.AddUObject(this, &ThisClass::HandleChessPieceTurnOver);
	CurrentSpeed = 0.0f;

	if (const AGridChessPiece* Pawn = GetPawn<AGridChessPiece>())
	{
		if (UGridChessPieceExtensionComponent* ChessPieceExtComp =
			UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Pawn))
		{
			ChessPieceExtComp->OnAbilitySystemInitialized_RegisterAndCall(
				FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
			ChessPieceExtComp->OnAbilitySystemUninitialized_RegisterAndCall(
				FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
		}
		if (UGridMapStateComponent* GridMapStateComponent = GetWorld()->GetGameState()->FindComponentByClass<
			UGridMapStateComponent>())
		{
			GridMapStateComponent->CallOrRegister_OnGridMapInitialized(
				FOnGridMapMangerInitielized::FDelegate::CreateUObject(this, &ThisClass::OnGridMapManagerInitialize));
		}
	}
	else
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[UGridChessPieceMovementComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));
#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("GridChessPieceMovementComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName ChessPieceCardLogName = TEXT("GridChessPieceMovementComponent");

			FMessageLog(ChessPieceCardLogName).Error()
			                                  ->AddToken(FUObjectToken::Create(
				                                  this, FText::FromString(GetNameSafe(this))))
			                                  ->AddToken(FTextToken::Create(Message));

			FMessageLog(ChessPieceCardLogName).Open();
		}
#endif
	}
}

void UGridChessPieceMovementComponent::OnUnregister()
{
	Super::OnUnregister();

	FGridGlobalDelegates::OnChessPieceTurnOver.RemoveAll(this);
	FGridGlobalDelegates::OnChessPieceSelectChanged.RemoveAll(this);
}

void UGridChessPieceMovementComponent::OnAbilitySystemInitialized()
{
	const APawn* Pawn = GetPawnChecked<APawn>();

	if (const UGridChessPieceExtensionComponent* ChessPieceExtComp =
		UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Pawn))
	{
		auto GridASC = ChessPieceExtComp->GetGridAbilitySystemComponent();

		if (AbilitySystemComponent)
		{
			UE_LOG(LogGrid, Error,
			       TEXT("GridChessPieceCardComponent: [%s] has already been initialized with an ability system."),
			       *GetNameSafe(Pawn));
			return;
		}

		AbilitySystemComponent = GridASC;
		if (!AbilitySystemComponent)
		{
			UE_LOG(LogGrid, Error,
			       TEXT(
				       "GridChessPieceCardComponent: Cannot initialize health component for owner [%s] with NULL ability system."
			       ), *GetNameSafe(Pawn));
			return;
		}

		ActionSet = AbilitySystemComponent->GetSet<UGridActionSet>();
		if (ActionSet == nullptr)
		{
			UE_LOG(LogGrid, Error,
			       TEXT(
				       "OnAbilitySystemInitialized: Cannot initialize action component for owner [%s] with NULL action set on the ability system."
			       ), *GetNameSafe(Pawn));
			return;
		}

		MoveSet = AbilitySystemComponent->GetSet<UGridMoveSet>();
		if (MoveSet == nullptr)
		{
			UE_LOG(LogGrid, Error,
			       TEXT(
				       "OnAbilitySystemInitialized: Cannot initialize action component for owner [%s] with NULL action set on the ability system."
			       ), *GetNameSafe(Pawn));
			return;
		}
	}
}

void UGridChessPieceMovementComponent::OnAbilitySystemUninitialized()
{
	ActionSet = nullptr;
	MoveSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void UGridChessPieceMovementComponent::OnGridMapManagerInitialize(AGridMapManager* InGridMapManager)
{
	check(InGridMapManager);

	GridMapManager = InGridMapManager;

	AGridChessPiece* Owner = GetPawnChecked<AGridChessPiece>();
	if (const UGridChessPieceExtensionComponent* ChessPieceExtComp =
		UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Owner))
	{
		GridMapManager->PawnArray[ChessPieceExtComp->GetTileIndex()] = Owner;
	}
}

void UGridChessPieceMovementComponent::HandleChessPieceSelectChanged(AGridChessPiece* OldChessPiece,
                                                                     AGridChessPiece* NewChessPiece)
{
	EndPathFinding();
}

void UGridChessPieceMovementComponent::HandleChessPieceTurnOver()
{
	EndPathFinding();
}

void UGridChessPieceMovementComponent::Internal_BeginMovement()
{
	OnChessPieceBeginMovementDynamic.Broadcast();
	OnChessPieceBeginMovementDynamic.Clear();
}

void UGridChessPieceMovementComponent::Internal_EndMovement()
{
	OnChessPieceEndMovementDynamic.Broadcast();
	OnChessPieceEndMovementDynamic.Clear();
}
