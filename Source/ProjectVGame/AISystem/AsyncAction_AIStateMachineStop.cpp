// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncAction_AIStateMachineStop.h"

#include "ChessPieces/GridChessPiece.h"
#include "ChessPieces/GridChessPieceAIStateMachineComponent.h"

UAsyncAction_AIStateMachineStop* UAsyncAction_AIStateMachineStop::WaitForAIStateMachineStopped(
	UObject* InWorldContextObject, AGridChessPiece* InTarget)
{
	UAsyncAction_AIStateMachineStop* Action = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncAction_AIStateMachineStop>();
		Action->WorldPtr = World;
		Action->Target = InTarget;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

void UAsyncAction_AIStateMachineStop::Activate()
{
	if (const AGridChessPiece* ChessPiece = Target.Get())
	{
		if (UGridChessPieceAIStateMachineComponent* AIStateMachineComponent = UGridChessPieceAIStateMachineComponent::FindAIStateMachineComponent(ChessPiece))
		{
			AIStateMachineComponent->OnAIStateMachineStop.Add(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleAIStateMachineStopped));
		}
	}else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_AIStateMachineStop::HandleAIStateMachineStopped()
{
	OnAsyncChessPieceAIStateMachineStopped.Broadcast();
	if (const AGridChessPiece* ChessPiece = Target.Get())
	{
		if (UGridChessPieceAIStateMachineComponent* AIStateMachineComponent = UGridChessPieceAIStateMachineComponent::FindAIStateMachineComponent(ChessPiece))
		{
			AIStateMachineComponent->OnAIStateMachineStop.RemoveAll(this);
		}
	}

	SetReadyToDestroy();
}
