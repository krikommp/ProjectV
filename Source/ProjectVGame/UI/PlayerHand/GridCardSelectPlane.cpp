// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCardSelectPlane.h"
#include "GridPlayerHand.h"

UGridCardSelectPlane::UGridCardSelectPlane(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGridCardSelectPlane::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 当且仅当手牌为空时，才发送进入选牌区事件
	if (PlayerHand != nullptr && PlayerHand->SelectedCard == nullptr)
	{
		PlayerHand->NotifyEnterSelectCardPlane();
	}
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UGridCardSelectPlane::SetupCardSelectPlane(UGridPlayerHand* InPlayerHand)
{
	if (PlayerHand == InPlayerHand)
	{
		return;
	}
	PlayerHand = InPlayerHand;
}
