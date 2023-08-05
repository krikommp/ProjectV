// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "TilemapAsset.h"
#include "GameFramework/Actor.h"
#include "Tilemap3DActor.generated.h"

class AGridChessBase;
class UProceduralMeshComponent;
class UTilemapAsset;

UCLASS()
class PROJECTVGAME_API ATilemap3DActor : public AModularPawn
{
	GENERATED_UCLASS_BODY()
	friend class UTilemapStateComponent;
	friend class UTilemapExtensionComponent;

	// 设置该对象的TilemapAsset
	void SetupTilemapAsset(const UTilemapAsset* InTilemapAsset);
	const UTilemapAsset* GetTilemap() const { return TilemapAsset.Get(); }

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<const UTilemapAsset> TilemapAsset;

	TArray<TWeakObjectPtr<const AGridChessBase>> ChessArray;

	UPROPERTY(Transient)
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

	UPROPERTY(Transient)
	TMap<FName, UInstancedStaticMeshComponent*> InstanceMeshCompMap;
};
