// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tilemap3DPreviewChess.generated.h"

UCLASS()
class PROJECTVEDITOR_API ATilemap3DPreviewChess : public AActor
{
	GENERATED_BODY()

public:
	ATilemap3DPreviewChess();
	void SetupSkeletalMeshAsset(USkeletalMesh* SkeletalMesh) const;
	void SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse) const;
private:
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(Transient)
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;
};
