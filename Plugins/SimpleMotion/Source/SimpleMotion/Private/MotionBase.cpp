#include "SimpleMotionPrivatePCH.h"

UMotionBase::UMotionBase()
{
	
}

void UMotionBase::Tick(float fDeltaTime)
{
	if (!bIsOperating)
	{
		return;
	}
	if(CheckFinalState())
	{
		OnMotionEnds();
	}else
	{
		KeepMotion(fDeltaTime);
	}
}

bool UMotionBase::CheckFinalState()
{
	return ReceiveCheckFinalState();
}

void UMotionBase::OnMotionEnds()
{
	bIsOperating = false;
	ReceiveMotionEnds();
	if (OnFinishMotion.IsBound())
	{
		OnFinishMotion.Broadcast();
	}
}

void UMotionBase::KeepMotion(float fDeltaTime)
{
	ReceiveKeepMotion(fDeltaTime);
}

void UMotionBase::StopMotion()
{
	bIsOperating = false;
	ReceiveStopMotion();
}

void UMotionBase::ExecuteVector(const FVector2D& InTarget, float InSpeed)
{
	Speed = InSpeed;
	Target = InTarget;
	bIsOperating = true;
}

void UMotionBase::ExecuteRotator(const FRotator& InRotator, float InSpeed)
{
	
	Rotator = InRotator;
	bIsOperating = true;
}

void UMotionBase::ExecuteWidgetTransform(const FWidgetTransform& Transform, float InSpeed)
{
	Speed = InSpeed;
	WidgetTarget = Transform;
	bIsOperating = true;
}

