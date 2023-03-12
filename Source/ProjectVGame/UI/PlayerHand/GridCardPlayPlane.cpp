// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCardPlayPlane.h"

#include "GridLogChannel.h"
#include "GridPlayerHand.h"

UGridCardPlayPlane::UGridCardPlayPlane(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGridCardPlayPlane::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 当且仅当手牌不为空，才发送离开选牌区域事件
	if (PlayerHand != nullptr && PlayerHand->SelectedCard == nullptr)
	{
		PlayerHand->NotifyLeaveSelectCardPlane();
	}
	// todo... 当手牌不为空时，应当进入打牌区域
	if (PlayerHand != nullptr && PlayerHand->SelectedCard != nullptr)
	{
		PlayerHand->NotifyEnterPlayCardPlane();
	}
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

FReply UGridCardPlayPlane::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (PlayerHand)
	{
		PlayerHand->NotifyPlayCardPlaneMouseButtonDown(InMouseEvent);
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("InValid PlayerHand in [%s], please call initialize function first."), *GetNameSafe(this));
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UGridCardPlayPlane::SetupCardPlayPlane(UGridPlayerHand* InPlayerHand)
{
	if (InPlayerHand == PlayerHand) {
		return;
	}

	PlayerHand = InPlayerHand;
}
