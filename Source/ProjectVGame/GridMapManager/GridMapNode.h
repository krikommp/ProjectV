
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GridMapNode.generated.h"

class AGridChessPiece;
class UGridAbilitySystemComponent;
/**
 * UGridMapNode
 *
 * 地图节点
 */
UCLASS(NotBlueprintable)
class PROJECTVGAME_API AGridMapNode : public AInfo
{
	GENERATED_BODY()
	
	friend class AGridMapManager;
	friend class UGridMapFunctionLibrary;

public:
	AGridMapNode(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	int32 GetTileIndex() const { return TileIndex; }

	UGridAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	void InitializeAbilitySystem();
	void SetNodePosition(const FVector& NewPosition);

	void OnChessPieceEnter(AGridChessPiece* InChessPiece) const;

	void OnChessPieceLeave(AGridChessPiece* InChessPiece) const;

	/**
	 * @brief 为棋子对象添加附加效果
	 * @param InChessPiece 棋子对象
	 */
	void AttachActiveGameplayEffect(const AGridChessPiece* InChessPiece) const;

	/**
	 * @brief 查找相邻符合要求的节点
	 * @param OutNearbyTiles 
	 * @param RequireTags 
	 */
	void FindAllNearbyTiles(TArray<const AGridMapNode*>& OutNearbyTiles, const FGameplayTagContainer& RequireTags) const;

private:
	void InitializeGameplayTags() const;
	
protected:

	int32 TileIndex;

	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;
};
