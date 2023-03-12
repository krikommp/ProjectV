// Fill out your copyright notice in the Description page of Project Settings.


#include "GridLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// UGridLoadingScreenSubsystem

UGridLoadingScreenSubsystem::UGridLoadingScreenSubsystem()
{
}

void UGridLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UGridLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}
