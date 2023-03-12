// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceAIStateMachineComponent.h"

#include "GridChessPieceComponent.h"
#include "GridChessPieceExtensionComponent.h"
#include "GridLogChannel.h"
#include "SMInstance.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif


#include UE_INLINE_GENERATED_CPP_BY_NAME(GridChessPieceAIStateMachineComponent)

UGridChessPieceAIStateMachineComponent::UGridChessPieceAIStateMachineComponent(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIStateMachine = nullptr;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UGridChessPieceAIStateMachineComponent::StartAIStateMachine()
{
	DoStart();
}

void UGridChessPieceAIStateMachineComponent::StopAIStateMachine()
{
	DoStop();
}

void UGridChessPieceAIStateMachineComponent::DoStart() const
{
	if (!AIStateMachine) return;

	AIStateMachine->Start();
}

void UGridChessPieceAIStateMachineComponent::DoUpdate(float DeltaTime) const
{
	if (!AIStateMachine) return;

	AIStateMachine->Update(DeltaTime);
}

void UGridChessPieceAIStateMachineComponent::DoStop() const
{
	if (!AIStateMachine) return;

	AIStateMachine->Stop();

	OnAIStateMachineStop.Broadcast();
}

void UGridChessPieceAIStateMachineComponent::DoShutDown()
{
	if (!AIStateMachine) return;

	AIStateMachine->Shutdown();

	AIStateMachine = nullptr;
}

void UGridChessPieceAIStateMachineComponent::OnRegister()
{
	Super::OnRegister();

	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (UGridChessPieceExtensionComponent* ChessPieceExtComponent = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Pawn))
		{
			ChessPieceExtComponent->OnChessPieceInitialized_RegisterAndCall(
				FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnChessPieceInitialized));
		}
	}
	else
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[UGridChessPieceAIStateMachineComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));
#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("UGridChessPieceAIStateMachineComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName ChessPieceAILogName = TEXT("GridChessPieceAIStateMachineComponent");

			FMessageLog(ChessPieceAILogName).Error()
			                                ->AddToken(FUObjectToken::Create(
				                                this, FText::FromString(GetNameSafe(this))))
			                                ->AddToken(FTextToken::Create(Message));

			FMessageLog(ChessPieceAILogName).Open();
		}
#endif
	}
}

void UGridChessPieceAIStateMachineComponent::OnUnregister()
{
	Super::OnUnregister();

	DoShutDown();
}

void UGridChessPieceAIStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DoUpdate(DeltaTime);
}

void UGridChessPieceAIStateMachineComponent::OnChessPieceInitialized()
{
	APawn* Pawn = GetPawnChecked<APawn>();

	if (UGridChessPieceExtensionComponent* ChessPieceExtensionComponent =
		UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Pawn))
	{
		const UGridChessPieceData* ChessPieceData = ChessPieceExtensionComponent->GetChessPieceData<
			UGridChessPieceData>();

		if (ChessPieceData->AIStateMachine)
		{
			AIStateMachine = nullptr;
			AIStateMachine = NewObject<USMInstance>(this, ChessPieceData->AIStateMachine);
			check(AIStateMachine);

			AIStateMachine->Initialize(Pawn);
		}
	}
}
