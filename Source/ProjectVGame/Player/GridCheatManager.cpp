// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCheatManager.h"

#include "GridLocalPlayer.h"
#include "GridLogChannel.h"
#include "GridPlayerController.h"
#include "Character/GridPlayerHandComponent.h"
#include "ChessPieces/GridChessPiece.h"
#include "ChessPieces/GridChessPieceExtensionComponent.h"
#include "Engine/Console.h"
#include "GameFramework/GameStateBase.h"
#include "GridTurnManager/GridTurnManagerComponent.h"
#include "Heros/GridCardInfo.h"
#include "System/GridAssetManager.h"

DEFINE_LOG_CATEGORY(LogGridCheat);

void UGridCheatManager::InitCheatManager()
{
	Super::InitCheatManager();

	LocalPlayerController = Cast<AGridPlayerController>(GetOuterAPlayerController());
}

void UGridCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(LogGridCheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void UGridCheatManager::AddNewCard(const FString& InCardID)
{
	if (const UGridTurnManagerComponent* TurnManagerComponent = GetWorld()->GetGameState()->FindComponentByClass<UGridTurnManagerComponent>())
	{
		if (const AGridChessPiece* ChessPiece = TurnManagerComponent->GetCurrentUnit())
		{
			if (const UGridChessPieceExtensionComponent* ChessPieceExtComp = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(ChessPiece))
			{
				const auto LocalPlayer = Cast<UGridLocalPlayer>(GetWorld()->GetGameInstance()->GetLocalPlayerByIndex(ChessPieceExtComp->GetChessPieceData<UGridChessPieceData>()->PlayerIndex));
				if (LocalPlayer != nullptr)
				{
					const UGridAssetManager& AssetManager = UGridAssetManager::Get();
					if (AssetManager.CheckCardData(InCardID))
					{
						const auto CardInfo = UGridCardInfo::CreateOrLoadCardInfo(LocalPlayer, InCardID, AssetManager.GetCardData(InCardID));
						CardInfo->OwningHero = ChessPiece;
						CardInfo->AbilitySystemComponent = ChessPieceExtComp->GetGridAbilitySystemComponent();
						if (UGridPlayerHandComponent* PlayerHandComponent = UGridPlayerHandComponent::FindPlayerHandComponent(LocalPlayerController->GetPawn()))
						{
							PlayerHandComponent->AddNewCardToPlayerHand(CardInfo);
							CheatOutputText(FString::Format(TEXT("Add New Card:[{0}]"), TArray<FStringFormatArg>{ *CardInfo->CardData.CardName.ToString() }));
						}
					}
				}
			}
		}
	}
}
