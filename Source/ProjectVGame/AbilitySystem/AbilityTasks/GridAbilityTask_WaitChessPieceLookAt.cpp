// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAbilityTask_WaitChessPieceLookAt.h"

#include "ChessPieces/GridChessPieceMovementComponent.h"
#include "GridMapManager/GridMapManager.h"
#include "GridMapManager/GridMapStateComponent.h"
#include "Kismet/KismetMathLibrary.h"

UGridAbilityTask_WaitChessPieceLookAt::UGridAbilityTask_WaitChessPieceLookAt(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bStartMovement = false;
}

UGridAbilityTask_WaitChessPieceLookAt* UGridAbilityTask_WaitChessPieceLookAt::WaitChessPieceLookAt(
	UGameplayAbility* OwningAbility, int32 InTargetIndex)
{
	UGridAbilityTask_WaitChessPieceLookAt* MyObj = NewAbilityTask<UGridAbilityTask_WaitChessPieceLookAt>(OwningAbility);
	if (MyObj)
	{
		MyObj->TargetIndex = InTargetIndex;
		MyObj->bStartMovement = false;
	}
	return MyObj;
}

void UGridAbilityTask_WaitChessPieceLookAt::Activate()
{
	if (const UGridMapStateComponent* GridMapStateComponent = GetWorld()->GetGameState()->FindComponentByClass<
		UGridMapStateComponent>())
	{
		const FVector SourceLocation = Ability->GetActorInfo().AvatarActor->GetActorLocation();
		const FVector TargetLocation = GridMapStateComponent->GetGGridMapManagerRef().IndexToVectorOnGrid(
			TargetIndex, 0.0f);
		TargetRotator = UKismetMathLibrary::MakeRotator(
			0.0f, 0.0f, UKismetMathLibrary::FindLookAtRotation(SourceLocation, TargetLocation).Yaw);
		Speed = 0.0f;
		bStartMovement = true;
	}
	else
	{
		EndTask();
	}
}

void UGridAbilityTask_WaitChessPieceLookAt::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (bStartMovement)
	{
		if (GetChessPieceMaxSpeed() > Speed)
		{
			Speed += GetChessPieceAcceleration();
		}
		
		FRotator InterpRotator = FMath::RInterpTo(Ability->GetActorInfo().AvatarActor->GetActorRotation(),
		                                          TargetRotator, DeltaTime, Speed);
		Ability->GetActorInfo().AvatarActor->SetActorRotation(InterpRotator);

		if (UKismetMathLibrary::EqualEqual_RotatorRotator(Ability->GetActorInfo().AvatarActor->GetActorRotation(),
		                                                  TargetRotator, 0.1))
		{
			OnFinishLookAt();
		}
	}
}

void UGridAbilityTask_WaitChessPieceLookAt::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UGridAbilityTask_WaitChessPieceLookAt::OnFinishLookAt()
{
	bStartMovement = false;
	Speed = 0.0f;
	Ability->GetActorInfo().AvatarActor->SetActorRotation(TargetRotator);
	OnWaitChessPieceLookAt.Broadcast();
	EndTask();
}

float UGridAbilityTask_WaitChessPieceLookAt::GetChessPieceMaxSpeed() const
{
	if (const UGridChessPieceMovementComponent* MovementComponent = UGridChessPieceMovementComponent::FindChessPieceMovementComponent( Ability->GetActorInfo().AvatarActor.Get()))
	{
		return MovementComponent->GetMaxSpeed();
	}
	return 0.0f;
}

float UGridAbilityTask_WaitChessPieceLookAt::GetChessPieceAcceleration() const
{
	if (const UGridChessPieceMovementComponent* MovementComponent = UGridChessPieceMovementComponent::FindChessPieceMovementComponent( Ability->GetActorInfo().AvatarActor.Get()))
	{
		return MovementComponent->GetAcceleration();
	}
	return 0.0f;
}
