// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerHandStateComponent.h"

#include "GridCardDrawPile.h"
#include "GridCardGraveyard.h"
#include "NativeGameplayTags.h"
#include "GridLogChannel.h"
#include "GridPlayerHand.h"
#include "PrimaryGameLayout.h"
#include "Character/GridTurnBaseMasterComponent.h"
#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GameModes/GridGameState.h"
#include "Input/CommonUIActionRouterBase.h"
#include "UI/Card/GridCard.h"

namespace FGridPlayerHandTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_PLATFORM_TRAIT_PLAYERHAND, "Platform.Trait.PlayerHand");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_GAME, "UI.Layer.Game");
}

UGridPlayerHandStateComponent::UGridPlayerHandStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UGridPlayerHandStateComponent::CheckAllCardInHandMotionEnded()
{
	if (!bStartToCheckMotion) return true;
	
	check(PlayerHand);

	for (const auto& Card : PlayerHand->Cards)
	{
		if (!Card->CheckCardMotionEnded())
		{
			return false;
		}
	}

	PlayerHand->OnPlayerHandMotionEnded.Broadcast();
	bStartToCheckMotion = false;

	return true;
}

void UGridPlayerHandStateComponent::ResetPlayerHand()
{
	check(PlayerHand);

	PlayerHand->ClearCard();
	PlayerHand->CardDrawPile->ClearCard();
	PlayerHand->CardGraveyard->ClearCard();

	PlayerHand->CurrentHandOwner = nullptr;
	
}

void UGridPlayerHandStateComponent::ShowPlayerHand()
{
	if (!PlayerHand) return;

	PlayerHand->CardDrawPile->SetVisibility(ESlateVisibility::Visible);
	PlayerHand->CardGraveyard->SetVisibility(ESlateVisibility::Visible);
}

void UGridPlayerHandStateComponent::HidePlayerHand()
{
	if (!PlayerHand) return;

	PlayerHand->CardDrawPile->SetVisibility(ESlateVisibility::Hidden);
	PlayerHand->CardGraveyard->SetVisibility(ESlateVisibility::Hidden);
}

void UGridPlayerHandStateComponent::OnRegister()
{
	Super::OnRegister();

	AGridGameState* GameState = GetGameStateChecked<AGridGameState>();
	if (UGridExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<
		UGridExperienceManagerComponent>())
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(
			FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
	else
	{
		UE_LOG(LogGrid, Error,
		       TEXT("[UGridPlayerHandStateComponent::OnRegister]：Can not found ExperienceManagerComponent on GameState!"
		       ));
	}
}

void UGridPlayerHandStateComponent::OnExperienceLoaded(const UGridExperienceDefinition* Experience)
{
	ActivatePlayerHand();
}

void UGridPlayerHandStateComponent::ActivatePlayerHand()
{
	if (PlayerHandClass == nullptr)
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[UGridPlayerHandStateComponent::ShowPlayerHand]: PlayerHandClass in InValid, please set this value in config file first."
		       ));
		return;
	}
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
	{
		for (UCommonActivatableWidget* GameWidget : RootLayout->GetLayerWidget(FGridPlayerHandTags::TAG_UI_LAYER_GAME)->GetWidgetList())
		{
			if (GameWidget->IsA(UGridPlayerHand::StaticClass()))
			{
				PlayerHand = Cast<UGridPlayerHand>(GameWidget);
			}
		}
	}
	if (PlayerHand == nullptr)
	{
		UE_LOG(LogGrid, Error, TEXT("Player Hand is invalid, you should add this widget in gamefeature.."));
		return;
	}

	HidePlayerHand();
}