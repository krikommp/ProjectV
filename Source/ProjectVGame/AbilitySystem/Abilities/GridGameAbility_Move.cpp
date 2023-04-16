// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameAbility_Move.h"

#include "AbilitySystemComponent.h"
#include "GridLogChannel.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/GridAbilityTask_OnTickEvent.h"
#include "ChessPieces/GridChessPiece.h"
#include "ChessPieces/GridChessPieceExtensionComponent.h"
#include "ChessPieces/GridChessPieceMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GridMapManager/GridMapManager.h"
#include "GridMapManager/GridMapStateComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridGameAbility_Move)

void UGridGameAbility_Move::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	ChessPiece = Cast<AGridChessPiece>(ActorInfo->AvatarActor);
	MovementComponent = UGridChessPieceMovementComponent::FindChessPieceMovementComponent(ChessPiece);
	AbilitySystemComponent = Cast<UGridAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGridGameAbility_Move::BeginMovement()
{
	if (ChessPiece == nullptr || MovementComponent == nullptr) return;

	GridMapManager = GetWorld()->GetGameState()->FindComponentByClass<UGridMapStateComponent>()->GetGridMapManager();
	if (GridMapManager == nullptr)
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[UGridGameAbility_Move::BeginMovement] Can not found GridMapManager, check GridMapComponent has been added"
		       ));
		return;
	}

	if (const UGridChessPieceExtensionComponent* ChessPieceExtComponent =
		UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(ChessPiece))
	{
		GridMapManager->PawnArray[ChessPieceExtComponent->GetTileIndex()] = nullptr;
		CurrentSplineDistance = GridMapManager->PathSpline->GetSplineLength();
		MovementComponent->SetCurrentSpeed(0.0f);
		bEndMovement = false;
	}

	MovementComponent->Internal_BeginMovement();
}

void UGridGameAbility_Move::TickMovement(float DeltaTime)
{
	if (bEndMovement) return;

	if (!GridMapManager.IsValid()) return;

	if (CurrentSplineDistance <= GridMapManager->PathSpline->GetDistanceAlongSplineAtSplinePoint(0))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		AccelerateAndMovePawnAlongSpline(DeltaTime);
	}
}

void UGridGameAbility_Move::AccelerateAndMovePawnAlongSpline(float DeltaTime)
{
	if (ChessPiece == nullptr || MovementComponent == nullptr) return;

	if (!GridMapManager.IsValid()) return;

	if (UGridChessPieceExtensionComponent* ChessPieceExtComponent = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(ChessPiece))
	{
		if (MovementComponent->GetMaxSpeed() > MovementComponent->GetCurrentSpeed())
		{
			MovementComponent->SetCurrentSpeed(MovementComponent->GetCurrentSpeed() + MovementComponent->GetAcceleration());
		}

		CurrentSplineDistance -= ((GridMapManager->TileBoundsX + GridMapManager->TileBoundsY) / 400.0f) * (
			DeltaTime * MovementComponent->GetCurrentSpeed());
		FVector Location = GridMapManager->PathSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance,
			ESplineCoordinateSpace::World);
		Location.Z = Location.Z - GridMapManager->SplineHeight;

		FRotator Rotator = GridMapManager->PathSpline->GetRotationAtDistanceAlongSpline(
			CurrentSplineDistance, ESplineCoordinateSpace::World);
		Rotator = UKismetMathLibrary::MakeRotator(0.0f, 0.0f, Rotator.Yaw + 180.0f);

		ChessPiece->SetActorLocationAndRotation(Location, Rotator);

		for (int32 PathIndex = 0; PathIndex < GridMapManager->GetPathVectorArray().Num(); ++PathIndex)
		{
			FVector PathLocation = GridMapManager->GetPathVectorArray()[PathIndex];
			PathLocation.Z -= GridMapManager->SplineHeight;
			if (UKismetMathLibrary::EqualEqual_VectorVector(ChessPiece->GetActorLocation(), PathLocation, 10.0f))
			{
				if (GridMapManager->GetPathIndexArray().IsValidIndex(PathIndex) && GridMapManager->GetPathIndexArray()[
					PathIndex] != ChessPieceExtComponent->GetTileIndex())
				{
					ChessPieceExtComponent->SetTileIndex(GridMapManager->GetPathIndexArray()[PathIndex]);
					const UGameplayEffect* MoveCost = MoveCostEffect->GetDefaultObject<UGameplayEffect>();
					AbilitySystemComponent->ApplyGameplayEffectToSelf(MoveCost, 1, AbilitySystemComponent->MakeEffectContext());
				}
			}
		}
	}
}

void UGridGameAbility_Move::EndMovement()
{
	if (ChessPiece == nullptr || MovementComponent == nullptr) return;

	if (!GridMapManager.IsValid()) return;

	if (!GridMapManager->GetPathIndexArray().IsEmpty())
	{
		if (UGridChessPieceExtensionComponent* ChessPieceExtComponent =
		UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(ChessPiece))
		{
			ChessPieceExtComponent->SetTileIndex(GridMapManager->GetPathIndexArray()[0]);
			const FVector TargetLocation = GridMapManager->
				IndexToVectorOnGrid(ChessPieceExtComponent->GetTileIndex(), 0.0f);

			ChessPiece->SetActorLocation(TargetLocation);

			GridMapManager->PawnArray[ChessPieceExtComponent->GetTileIndex()] = ChessPiece;
		}
	}

	bEndMovement = true;
	MovementComponent->SetCurrentSpeed(0.0f);
	MovementComponent->Internal_EndMovement();
}
