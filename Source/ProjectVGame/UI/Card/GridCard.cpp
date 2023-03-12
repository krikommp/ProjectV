// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCard.h"

#include "GridLogChannel.h"
#include "SMInstance.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGridCard::UGridCard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CardStateMachineInstance = nullptr;
}

void UGridCard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	DoUpdate(InDeltaTime);
	
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGridCard::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (OnCardPointerEnter.IsBound())
	{
		OnCardPointerEnter.Broadcast();
	}
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UGridCard::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (OnCardPointerLeave.IsBound())
	{
		OnCardPointerLeave.Broadcast();
	}
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UGridCard::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (OnCardPointerDown.IsBound())
	{
		OnCardPointerDown.Broadcast(InMouseEvent);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UGridCard::RecycleCard()
{
	StopStateMachine();
	PlayerHand = nullptr;
	CardInfo = nullptr;
}

void UGridCard::InitializeStateMachine()
{
	CreateCardStateMachineInstance();
}

void UGridCard::SetupCardInfo(UGridCardInfo* InCardInfo)
{
	CardInfo = InCardInfo;

	if (CardInfo == nullptr)
	{
		UE_LOG(LogGrid, Error, TEXT("[UGridCard::SetupCardInfo]: Cannot initialize Card Info for [%s] with NULL CardInfo"), *GetNameSafe(this));
	}
	ReceiveSetupCardInfo(CardInfo);
}

void UGridCard::RequestDesiredTransformUpdate(const FWidgetTransform& DesiredTransform, bool bImmediately)
{
	SuggestedTransform = DesiredTransform;
	if (bImmediately)
	{
		SetRenderTransform(SuggestedTransform);
	}
}

void UGridCard::SuggestZOrder(int32 InZOrder)
{
	if (const auto CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this))
	{
		CanvasSlot->SetZOrder(InZOrder);
	}
}

bool UGridCard::MoveToSuggestedTransform(const FWidgetTransform& InSuggestedTransform, float InSpeed, float InDeltaTime)
{
	// 终止条件
	if (UKismetMathLibrary::EqualEqual_Vector2DVector2D(GetRenderTransform().Translation, InSuggestedTransform.Translation, 1.0)
		&& UKismetMathLibrary::NearlyEqual_FloatFloat(GetRenderTransform().Angle, InSuggestedTransform.Angle, 0.1)
		&& UKismetMathLibrary::EqualEqual_Vector2DVector2D(GetRenderTransform().Scale, InSuggestedTransform.Scale, 0.001))
	{
		SetRenderTransform(FWidgetTransform(InSuggestedTransform.Translation, InSuggestedTransform.Scale,FVector2D(0.0, 0.0), GetRenderTransform().Angle));
		OnCardMotionEnded.Broadcast();
		OnCardMotionEnded.Clear();
		bCardMotionEnded = true;
		return true;
	}
	PreviousSpeed = InSpeed;
	
	const float MotionSpeed = UKismetMathLibrary::FMin(InSpeed * InDeltaTime, 1.0); 
	const FVector LerpTranslation = UKismetMathLibrary::VLerp(
		FVector(GetRenderTransform().Translation.X, GetRenderTransform().Translation.Y, 0.0),
		FVector(InSuggestedTransform.Translation.X, InSuggestedTransform.Translation.Y, 0.0),
		MotionSpeed);
	const FVector LerpScale = UKismetMathLibrary::VLerp(
		FVector(GetRenderTransform().Scale.X, GetRenderTransform().Scale.Y, 0.0),
		FVector(InSuggestedTransform.Scale.X, InSuggestedTransform.Scale.Y, 0.0),
		MotionSpeed);
	const float LerpAngle = UKismetMathLibrary::Lerp(
		GetRenderTransform().Angle,
		InSuggestedTransform.Angle,
		MotionSpeed);
	SetRenderTransform(FWidgetTransform(FVector2D(LerpTranslation.X, LerpTranslation.Y), FVector2D(LerpScale.X, LerpScale.Y), FVector2D(0.0, 0.0), LerpAngle));
	bCardMotionEnded = false;
	return false;
}

FVector2D UGridCard::GetMousePosition()
{
	return UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
}

void UGridCard::SetCardSize(const FVector2D& NewCardSize) const
{
	CardSizeBox->SetWidthOverride(NewCardSize.X);
	CardSizeBox->SetHeightOverride(NewCardSize.Y);
}

void UGridCard::StopStateMachine()
{
	DoShutDown();
}

void UGridCard::GateCardInput()
{
	if (CardState == ECardState::Disable) CardState = ECardState::Idle;
	else if (CardState == ECardState::Idle) CardState = ECardState::Disable;
}

void UGridCard::CreateCardStateMachineInstance()
{
	if (!CardStateMachineClass)
	{
		UE_LOG(LogGrid, Error, TEXT("CardStateMachineClass Is Invalid, could not create Instance from this."));
		return;
	}

	if (CardStateMachineInstance == nullptr)
	{
		CardStateMachineInstance = NewObject<USMInstance>(this, CardStateMachineClass, NAME_None, RF_NoFlags);
	}

	check(CardStateMachineInstance);

	if (!CardStateMachineInstance->IsInitialized())
	{
		CardStateMachineInstance->Initialize(this);
	}
}

void UGridCard::DoStart() const
{
	if (!CardStateMachineInstance)
	{
		return;
	}

	CardStateMachineInstance->Start();
}

void UGridCard::DoUpdate(float DeltaTime) const
{
	if (!CardStateMachineInstance)
	{
		return;
	}

	CardStateMachineInstance->Update(DeltaTime);
}

void UGridCard::DoStop() const
{
	if (!CardStateMachineInstance)
	{
		return;
	}

	CardStateMachineInstance->Stop();
}

void UGridCard::DoShutDown()
{
	if (!CardStateMachineInstance)
	{
		return;
	}

	CardStateMachineInstance->Shutdown();

	CardStateMachineInstance = nullptr;
}
