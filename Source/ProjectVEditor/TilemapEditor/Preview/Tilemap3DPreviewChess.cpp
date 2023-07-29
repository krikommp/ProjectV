// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DPreviewChess.h"

#include "Components/CapsuleComponent.h"

ATilemap3DPreviewChess::ATilemap3DPreviewChess()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(50.0f, 80.0f);
	CapsuleComponent->SetRelativeLocation(FVector::UpVector * 50.0f);
	CapsuleComponent->SetupAttachment(RootComponent);
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ATilemap3DPreviewChess::SetupSkeletalMeshAsset(USkeletalMesh* SkeletalMesh) const
{
	SkeletalMeshComponent->SetSkeletalMeshAsset(SkeletalMesh);
}

void ATilemap3DPreviewChess::SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse) const
{
	CapsuleComponent->SetCollisionResponseToChannel(Channel, NewResponse);
}
