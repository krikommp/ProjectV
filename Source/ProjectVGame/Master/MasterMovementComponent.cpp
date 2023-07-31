// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterMovementComponent.h"

#include "GridTraceChannel.h"
#include "Kismet/GameplayStatics.h"


UMasterMovementComponent::UMasterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UMasterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMasterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Falling(DeltaTime);
}

void UMasterMovementComponent::Falling(float DeltaTime) const
{
	if (CheckUnderGround())
		return;

	const FVector ActorLocation = GetPawn<APawn>()->GetActorLocation();
	const FVector NewLocation = ActorLocation - FVector(0, 0, FallingSpeed * DeltaTime);
	GetPawn<APawn>()->SetActorLocation(NewLocation);
}

bool UMasterMovementComponent::CheckUnderGround() const
{
	const FVector ActorLocation = GetPawn<APawn>()->GetActorLocation();

	const FVector RayStart = ActorLocation;
	const FVector RayEnd = ActorLocation - FVector::UpVector * GroundCheckLength;

	FHitResult HitResult;
	TArray<AActor*> IgnoreActors;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), RayStart, RayEnd, UEngineTypes::ConvertToTraceType(PathTrace), false, IgnoreActors, EDrawDebugTrace::Persistent, HitResult, false);

	return HitResult.bBlockingHit;
}
