// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceStart.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"


AGridChessPieceStart::AGridChessPieceStart(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(50.0f, 80.0f);
	CapsuleComponent->SetupAttachment(CustomGridAnchor);
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(CapsuleComponent);
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

const FTransform& AGridChessPieceStart::GetCustomGridAnchorTransform() const
{
	return CustomGridAnchor->GetComponentTransform();
}
