// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapAsset.h"
#include "GameFramework/Actor.h"
#include "Tilemap3DActor.generated.h"

class UProceduralMeshComponent;
class UTilemapAsset;

UCLASS()
class PROJECTVGAME_API ATilemap3DActor : public AActor
{
	GENERATED_BODY()

public:
	ATilemap3DActor();

	virtual void BeginPlay() override;

	// 设置该对象的TilemapAsset
	void SetupTilemapAsset(const UTilemapAsset* InTilemapAsset);

	// 检测传入位置是否越界, 如果越界将会返回修正之后的位置，否则返回原始位置
	void CheckLocationOutBound(FVector& InLocation) const;

private:
	TWeakObjectPtr<const UTilemapAsset> TilemapAsset;

	UPROPERTY(Transient)
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

	UPROPERTY(Transient)
	TMap<FName, UInstancedStaticMeshComponent*> InstanceMeshCompMap;
};
