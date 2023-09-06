// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessDirectionArrow.h"

#include "ChessDirectionComponent.h"
#include "GridTraceChannel.h"
#include "Cursor/CursorEventComponent.h"

AChessDirectionArrow::AChessDirectionArrow(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer), TargetIndex(INDEX_NONE)
{
	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetReceivesDecals(false);
	RootComponent = ArrowMesh;

	CursorEventComponent = ObjectInitializer.CreateDefaultSubobject<UCursorEventComponent>(this, TEXT("CursorEventComponent"));
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AChessDirectionArrow::InitializeArrow(const FChessArrowInitParams& Params)
{
	OwnerPawn = Params.OwnerPawn;
	
	ArrowMesh->SetStaticMesh(Params.ArrowMesh);
	ArrowMesh->SetMaterial(0, Params.ArrowMaterial);
}

void AChessDirectionArrow::EnableCollision(bool bEnable) const
{
	if (bEnable)
	{
		ArrowMesh->SetCollisionResponseToChannel(MouseHoverTrace, ECR_Block);
		ArrowMesh->SetCollisionResponseToChannel(MouseClickTrace, ECR_Block);
	}else
	{
		ArrowMesh->SetCollisionResponseToChannel(MouseHoverTrace, ECR_Ignore);
		ArrowMesh->SetCollisionResponseToChannel(MouseClickTrace, ECR_Ignore);
	}
}

void AChessDirectionArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AChessDirectionArrow::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AChessDirectionArrow::OnCursorClick()
{
	if (!OwnerPawn.IsValid())
		return;

	UChessDirectionComponent* DirectionComponent = UChessDirectionComponent::FindChessDirectionComponent(OwnerPawn.Get());
	if (!DirectionComponent)
		return;

	DirectionComponent->SetLookAtIndex(TargetIndex);
}

void AChessDirectionArrow::OnHoverBegin()
{
}

void AChessDirectionArrow::OnHovering()
{
}

void AChessDirectionArrow::OnHoverEnd()
{
}
