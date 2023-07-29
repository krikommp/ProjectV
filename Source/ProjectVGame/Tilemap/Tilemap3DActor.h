// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTilemapAsset> TilemapAsset;

	UPROPERTY(Transient)
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

	UPROPERTY(Transient)
	TMap<FName, UInstancedStaticMeshComponent*> InstanceMeshCompMap;
};
