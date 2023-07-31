// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessExtensionComponent.h"


UGridChessExtensionComponent::UGridChessExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
	
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UGridChessExtensionComponent::BeginPlay()
{
	Super::BeginPlay();
}
