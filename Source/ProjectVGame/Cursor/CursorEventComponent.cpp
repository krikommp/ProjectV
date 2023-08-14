// Fill out your copyright notice in the Description page of Project Settings.


#include "CursorEventComponent.h"

#include "CursorClickInterface.h"
#include "CursorHoverInterface.h"
#include "GridGameplayTags.h"
#include "Messages/GridVerbMessage.h"

UCursorEventComponent::UCursorEventComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer), MouseCursorState(EMouseCursorState::ECS_UnHover)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCursorEventComponent::OnRegister()
{
	Super::OnRegister();

	const FGridGameplayTags& GameplayTags = FGridGameplayTags::Get();
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MouseClickListenerHandle = MessageSubsystem.RegisterListener(GameplayTags.InputTag_MouseConfirm, this, &ThisClass::ListenForMouseClickEvent);
	MouseHoverListenerHandle = MessageSubsystem.RegisterListener(GameplayTags.InputTag_MouseHover, this, &ThisClass::ListenForMouseHoverEvent);
}

void UCursorEventComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCursorEventComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.UnregisterListener(MouseHoverListenerHandle);
	MessageSubsystem.UnregisterListener(MouseClickListenerHandle);
	
	Super::EndPlay(EndPlayReason);
}

void UCursorEventComponent::ListenForMouseHoverEvent(FGameplayTag EventTag, const FGridMouseMessage& Message)
{
	const APawn* Pawn = GetPawn<APawn>();

	switch (MouseCursorState)
	{
	case EMouseCursorState::ECS_UnHover:
		if (Pawn == Message.HitTarget)
		{
			OnMouseBeginHover();
			MouseCursorState = EMouseCursorState::ECS_Hover;
		}
		break;
	case EMouseCursorState::ECS_Hover:
		if (Pawn == Message.HitTarget)
		{
			OnMouseHovering();
			MouseCursorState = EMouseCursorState::ECS_Hovering;
		}else
		{
			OnMouseEndHover();
			MouseCursorState = EMouseCursorState::ECS_UnHover;
		}
		break;
	case EMouseCursorState::ECS_Hovering:
		if (Pawn == Message.HitTarget)
		{
			OnMouseHovering();
		}else
		{
			OnMouseEndHover();
			MouseCursorState = EMouseCursorState::ECS_UnHover;
		}
		break;
	default: ;
	}
}

void UCursorEventComponent::ListenForMouseClickEvent(FGameplayTag EventTag, const FGridMouseMessage& Message)
{
	APawn* Pawn = GetPawn<APawn>();

	if (Pawn != Message.HitTarget)
		return;

	if (Pawn == nullptr)
		return;

	if (!Pawn->GetClass()->ImplementsInterface(UCursorClickInterface::StaticClass()))
		return;

	ICursorClickInterface* CursorEventInterface = Cast<ICursorClickInterface>(Pawn);
	if (CursorEventInterface == nullptr)
		return;

	CursorEventInterface->OnCursorClick();
}

void UCursorEventComponent::OnMouseBeginHover()
{
	APawn* Pawn = GetPawn<APawn>();

	if (Pawn == nullptr)
		return;

	if (!Pawn->GetClass()->ImplementsInterface(UCursorHoverInterface::StaticClass()))
		return;

	ICursorHoverInterface* CursorEventInterface = Cast<ICursorHoverInterface>(Pawn);
	if (CursorEventInterface == nullptr)
		return;

	CursorEventInterface->OnHoverBegin();
}

void UCursorEventComponent::OnMouseHovering()
{
	APawn* Pawn = GetPawn<APawn>();

	if (Pawn == nullptr)
		return;

	if (!Pawn->GetClass()->ImplementsInterface(UCursorHoverInterface::StaticClass()))
		return;

	ICursorHoverInterface* CursorEventInterface = Cast<ICursorHoverInterface>(Pawn);
	if (CursorEventInterface == nullptr)
		return;

	CursorEventInterface->OnHovering();
}

void UCursorEventComponent::OnMouseEndHover()
{
	APawn* Pawn = GetPawn<APawn>();

	if (Pawn == nullptr)
		return;

	if (!Pawn->GetClass()->ImplementsInterface(UCursorHoverInterface::StaticClass()))
		return;

	ICursorHoverInterface* CursorEventInterface = Cast<ICursorHoverInterface>(Pawn);
	if (CursorEventInterface == nullptr)
		return;

	CursorEventInterface->OnHoverEnd();
}
