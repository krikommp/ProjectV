#include "GridAbilityTargetType.h"
#include "GridMapManager/GridMapNode.h"
#include "ChessPieces/GridChessPiece.h"
#include "GridMapManager/GridMapManager.h"
#include "GridMapManager/GridMapStateComponent.h"

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
			if (GridMapManager->PawnArray.IsValidIndex(TargetTileIndex) && GridMapManager->PawnArray[TargetTileIndex] != nullptr)
			{
				OutActors.Add(GridMapManager->PawnArray[TargetTileIndex]);
			}else if (GridMapManager->GridMapNodeArray.IsValidIndex(TargetTileIndex) && GridMapManager->GridMapNodeArray[TargetTileIndex] != nullptr)
			{
				OutActors.Add(GridMapManager->GridMapNodeArray[TargetTileIndex]);
			}
		}
	}
}
