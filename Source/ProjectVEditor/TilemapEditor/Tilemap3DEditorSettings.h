// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tilemap3DEditorSettings.generated.h"

class UMaterialInterface;
class UStaticMesh;

/**
 * UTilemap3DEditorSettings
 *
 * Tilemap 配置文件
 */
UCLASS(Config="TilemapEditor")
class PROJECTVEDITOR_API UTilemap3DEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Collision")
	TSoftObjectPtr<UMaterialInterface> CollisionPlaneMat;

	UPROPERTY(Config, EditAnywhere, Category="Collision")
	TSoftObjectPtr<UStaticMesh> CollisionMesh;
};
