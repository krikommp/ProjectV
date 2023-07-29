// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DSelected.h"

ATilemap3DSelected::ATilemap3DSelected(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	check(SceneComponent != nullptr);
	this->RootComponent = SceneComponent;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
	StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ATilemap3DSelected::SetStaticMesh(UStaticMesh* PreviewMesh) const
{
	StaticMeshComponent->SetStaticMesh(PreviewMesh);
}

void ATilemap3DSelected::SetVisible(bool bVisible) const
{
	StaticMeshComponent->SetVisibility(bVisible);
}
