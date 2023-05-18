
#pragma once

#include "CoreMinimal.h"
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
	
	int32 GetTileIndex() const { return TileIndex; }

	UGridAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	void InitializeAbilitySystem();
	void SetNodePosition(const FVector& NewPosition);

	void OnChessPieceEnter(AGridChessPiece* InChessPiece) const;

	void OnChessPieceLeave(AGridChessPiece* InChessPiece) const;

private:
	void InitializeGameplayTags() const;
	
protected:

	int32 TileIndex;

	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;
};
