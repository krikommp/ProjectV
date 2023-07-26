// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DPreviewChess.h"

ATilemap3DPreviewChess::ATilemap3DPreviewChess()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ATilemap3DPreviewChess::SetupSkeletalMeshAsset(USkeletalMesh* SkeletalMesh) const
{
	SkeletalMeshComponent->SetSkeletalMeshAsset(SkeletalMesh);
}

void ATilemap3DPreviewChess::SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse) const
{
	SkeletalMeshComponent->SetCollisionResponseToChannel(Channel, NewResponse);
}
