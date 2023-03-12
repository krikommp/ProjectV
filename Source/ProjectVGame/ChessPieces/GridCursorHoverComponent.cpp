// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCursorHoverComponent.h"

#include "GridLogChannel.h"
#include "Player/GridPlayerController.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

#include "GridChessPiece.h"
#include "GridTraceChannel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridCursorHoverComponent)

UGridCursorHoverComponent::UGridCursorHoverComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	lastChessPieceHovered = nullptr;
	bCheckCursor = false;
}

void UGridCursorHoverComponent::OnRegister()
{
	Super::OnRegister();

	APlayerController* Owner = GetOwner<APlayerController>();

	if (Owner)
	{
		bCheckCursor = true;
	}
	else
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[UGridCursorHoverComponent::OnRegister] This component has been added to a blueprint whose base class is not a PlayerControlle. To use this component, it MUST be placed on a PlayerControlle Blueprint."
		       ));
#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("UGridCursorHoverComponent", "NotPlayerControlleError",
			                                       "has been added to a blueprint whose base class is not a PlayerControlle. To use this component, it MUST be placed on a PlayerControlle Blueprint. This will cause a crash if you PIE!");
			static const FName MessageLogName = TEXT("UGridCursorHoverComponent");

			FMessageLog(MessageLogName).Error()
			                           ->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
			                           ->AddToken(FTextToken::Create(Message));

			FMessageLog(MessageLogName).Open();
		}
#endif
	}
}

void UGridCursorHoverComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const APlayerController* Owner = GetOwner<APlayerController>();
	if (bCheckCursor && Owner)
	{
		FHitResult HitResult;
		Owner->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ChessPieceTrace), false, HitResult);
		if (HitResult.IsValidBlockingHit())
		{
			AGridChessPiece* newChessPieceHovered = Cast<AGridChessPiece>(HitResult.GetActor());
			if (lastChessPieceHovered != newChessPieceHovered)
			{
				if (lastChessPieceHovered != nullptr)
				{
					lastChessPieceHovered->OnCursorHoverEnd();
				}
				lastChessPieceHovered = newChessPieceHovered;
				lastChessPieceHovered->OnCursorHoverBegin();
				OnChessPieceHoverBegin.Broadcast(lastChessPieceHovered);
			}
		}
		else if (lastChessPieceHovered != nullptr)
		{
			lastChessPieceHovered->OnCursorHoverEnd();
			OnChessPieceHoverEnd.Broadcast(lastChessPieceHovered);
			lastChessPieceHovered = nullptr;
		}
	}
}
