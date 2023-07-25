#include "GridAbilityTargetType.h"
#include "GridMapManager/GridMapNode.h"
#include "ChessPieces/GridChessPiece.h"
#include "GridMapManager/GridMapFunctionLibrary.h"
#include "GridMapManager/GridMapManager.h"
#include "GridMapManager/GridMapStateComponent.h"

static void AppendValidTargets(const AGridMapManager* GridMapManager, int32 Index, bool bExcludeGridMapNode, TArray<AActor*>& OutActors)
{
	if (!bExcludeGridMapNode)
	{
		if (GridMapManager->GridMapNodeArray.IsValidIndex(Index) && GridMapManager->GridMapNodeArray[Index] != nullptr) 
		{
			OutActors.Add(GridMapManager->GridMapNodeArray[Index]);
		}
	}
	if (GridMapManager->PawnArray.IsValidIndex(Index) && GridMapManager->PawnArray[Index] != nullptr)
	{
		OutActors.Add(GridMapManager->PawnArray[Index]);
	}
}

void UGridAbilityTargetType::GetTargets_Implementation(AActor* TargetingActor, AGameStateBase* GameState,
                                                       int32 TargetTileIndex, int32 Range, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults,
                                                       TArray<AActor*>& OutActors) const
{
	return;
}

void UGridAbilityTargetType_User::GetTargets_Implementation(AActor* TargetingActor, AGameStateBase* GameState,
                                                            int32 TargetTileIndex, int32 Range, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults,
                                                            TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingActor);
}

void UGridAbilityTargetType_SingleOnGridMap::GetTargets_Implementation(AActor* TargetingActor,
	AGameStateBase* GameState, int32 TargetTileIndex, int32 Range, FGameplayEventData EventData,
	TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	if (const UGridMapStateComponent* GridMapStateComponent = GameState->FindComponentByClass<UGridMapStateComponent>())
	{
		if (const AGridMapManager* GridMapManager = GridMapStateComponent->GetGridMapManager())
		{
			AppendValidTargets(GridMapManager, TargetTileIndex, false, OutActors);
		}
	}
}

void UGridAbilityTargetType_MultiOnGridMap::GetTargets_Implementation(AActor* TargetingActor, AGameStateBase* GameState,
	int32 TargetTileIndex, int32 Range, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults,
	TArray<AActor*>& OutActors) const
{
	if (const UGridMapStateComponent* GridMapStateComponent = GameState->FindComponentByClass<UGridMapStateComponent>())
	{
		if (const AGridMapManager* GridMapManager = GridMapStateComponent->GetGridMapManager())
		{
			// AppendValidTargets(GridMapManager, TargetTileIndex, false, OutActors);
			const TArray<int32> InRangeIndexes = UGridMapFunctionLibrary::GetTileIndexesInRange(GridMapManager, TargetTileIndex, Range);
			for (const int32 Index : InRangeIndexes)
			{
				AppendValidTargets(GridMapManager, Index, false, OutActors);
			}
		}
	}
}
