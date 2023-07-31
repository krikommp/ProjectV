// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterMovementComponent.h"

#include "GridTraceChannel.h"
#include "Kismet/GameplayStatics.h"


UMasterMovementComponent::UMasterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bNeedJump(false)
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
	bNeedJump = CheckNeedJump();
	Moving(DeltaTime);
	FinishMovement();
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
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), RayStart, RayEnd, UEngineTypes::ConvertToTraceType(PathTrace), false, IgnoreActors, EDrawDebugTrace::None, HitResult, false);

	return HitResult.bBlockingHit;
}

void UMasterMovementComponent::Moving(float DeltaTime) const
{
	APawn* Pawn = GetPawn<APawn>();

	const FVector InputVector = Pawn->GetPendingMovementInputVector();
	if (InputVector == FVector::Zero())
		return;

	const FVector CurrentLocation = GetOwner()->GetActorLocation();
	const FVector NewLocation = CurrentLocation + InputVector * MoveSpeed * DeltaTime + (bNeedJump ? FVector::UpVector * 100.0f : FVector::Zero()); 
	Pawn->SetActorLocation(NewLocation);
}

bool UMasterMovementComponent::CheckNeedJump() const
{
	APawn* Pawn = GetPawn<APawn>();

	FVector InputVector = Pawn->GetPendingMovementInputVector();
	if (InputVector == FVector::Zero())
		return false;
	InputVector.Normalize();

	const FVector ActorLocation = Pawn->GetActorLocation();

	const FVector RayStart = ActorLocation;
	const FVector RayEnd = ActorLocation - InputVector * MoveCheckLength;

	FHitResult HitResult;
	TArray<AActor*> IgnoreActors;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), RayStart, RayEnd, UEngineTypes::ConvertToTraceType(PathTrace), false, IgnoreActors, EDrawDebugTrace::None, HitResult, false);

	return HitResult.bBlockingHit;
}

void UMasterMovementComponent::FinishMovement()
{
	APawn* Pawn = GetPawn<APawn>();
	Pawn->ConsumeMovementInputVector();
	bNeedJump = false;
}
