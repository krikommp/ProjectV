// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerStart.h"

#include "Delegates/Delegate.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Pawn.h"
#include "Math/MathFwd.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Templates/SubclassOf.h"
#include "TimerManager.h"
#include "UObject/Class.h"

AGridPlayerStart::AGridPlayerStart(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

EGridPlayerStartLocationOccupancy AGridPlayerStart::GetLocationOccupancy(AController* const ControllerPawnToFit) const
{
	UWorld* const World = GetWorld();
	if (HasAuthority() && World)
	{
		if (AGameModeBase* AuthGameMode = World->GetAuthGameMode())
		{
			TSubclassOf<APawn> PawnClass = AuthGameMode->GetDefaultPawnClassForController(ControllerPawnToFit);
			const APawn* const PawnToFit = PawnClass ? GetDefault<APawn>(PawnClass) : nullptr;

			FVector ActorLocation = GetActorLocation();
			const FRotator ActorRotation = GetActorRotation();

			if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation, nullptr))
			{
				return EGridPlayerStartLocationOccupancy::Empty;
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				return EGridPlayerStartLocationOccupancy::Partial;
			}
		}
	}

	return EGridPlayerStartLocationOccupancy::Full;
}

bool AGridPlayerStart::IsClaimed() const
{
	return ClaimingController != nullptr;
}

bool AGridPlayerStart::TryClaim(AController* OccupyingController)
{
	if (OccupyingController != nullptr && !IsClaimed())
	{
		ClaimingController = OccupyingController;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ExpirationTimerHandle, FTimerDelegate::CreateUObject(this, &AGridPlayerStart::CheckUnclaimed), ExpirationCheckInterval, true);
		}
		return true;
	}
	return false;
}

void AGridPlayerStart::CheckUnclaimed()
{
	if (ClaimingController != nullptr && ClaimingController->GetPawn() != nullptr && GetLocationOccupancy(ClaimingController) == EGridPlayerStartLocationOccupancy::Empty)
	{
		ClaimingController = nullptr;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ExpirationTimerHandle);
		}
	}
}