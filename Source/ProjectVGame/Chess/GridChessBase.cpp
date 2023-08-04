// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessBase.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GridChessExtensionComponent.h"
#include "GridTraceChannel.h"
#include "Tilemap/TilemapExtensionComponent.h"


AGridChessBase::AGridChessBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(50.0f, 80.0f);
	CapsuleComponent->SetCollisionResponseToChannel(MouseClickTrace, ECR_Block);
	RootComponent = CapsuleComponent;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetReceivesDecals(false);
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);

	ChessExtensionComponent = ObjectInitializer.CreateDefaultSubobject<UGridChessExtensionComponent>(this, TEXT("ChessExtensionComponent"));
	TilemapExtensionComponent = ObjectInitializer.CreateDefaultSubobject<UTilemapExtensionComponent>(this, TEXT("TilemapExtensionComponent"));
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AGridChessBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGridChessBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGridChessBase::SetupSkeletalMeshAsset(USkeletalMesh* SkeletalMesh) const
{
	SkeletalMeshComponent->SetSkeletalMeshAsset(SkeletalMesh);
}

void AGridChessBase::SetupAnimInstanceClass(TSubclassOf<UAnimInstance> AnimBlueprint) const
{
	SkeletalMeshComponent->SetAnimInstanceClass(AnimBlueprint);
}

