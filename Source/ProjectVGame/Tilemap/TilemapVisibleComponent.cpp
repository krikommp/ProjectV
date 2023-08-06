// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapVisibleComponent.h"

#include "TilemapExtensionComponent.h"

UTilemapVisibleComponent::UTilemapVisibleComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;	
}

void UTilemapVisibleComponent::BeginPlay()
{
	Super::BeginPlay();
}