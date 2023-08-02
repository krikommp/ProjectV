// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapAsset.h"
#include "GameFramework/Actor.h"
#include "Tilemap3DActor.generated.h"

class AGridChessBase;
class UProceduralMeshComponent;
class UTilemapAsset;

UCLASS()
class PROJECTVGAME_API ATilemap3DActor : public AActor
{
	GENERATED_BODY()
	friend class UTilemapStateComponent;
	friend class UTilemapExtensionComponent;

public:
	ATilemap3DActor();

	virtual void BeginPlay() override;

	// 设置该对象的TilemapAsset
	void SetupTilemapAsset(const UTilemapAsset* InTilemapAsset);
	const UTilemapAsset* GetTilemap() const { return TilemapAsset.Get(); }

	// 获取寻路数组总大小
	int32 GetPathfindingArrayNum() const;

private:
	TWeakObjectPtr<const UTilemapAsset> TilemapAsset;

	TArray<TWeakObjectPtr<const AGridChessBase>> ChessArray;

	UPROPERTY(Transient)
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

	UPROPERTY(Transient)
	TMap<FName, UInstancedStaticMeshComponent*> InstanceMeshCompMap;
};
