// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tilemap3DSelected.generated.h"

class UStaticMeshComponent;
class UStaticMesh;

/**
 * ATilemap3DSelected
 *
 * 预览被选中的 Tilemap 模型
 */
UCLASS()
class PROJECTVEDITOR_API ATilemap3DSelected : public AActor
{
	GENERATED_BODY()

public:
	ATilemap3DSelected(const FObjectInitializer& ObjectInitializer);

	void SetStaticMesh(UStaticMesh* PreviewMesh) const;
	void SetVisible(bool bVisible) const;
private:

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
