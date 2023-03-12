// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameData.h"

#include "GridAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridGameData)

UGridGameData::UGridGameData()
{
}

const UGridGameData& UGridGameData::Get()
{
	return UGridAssetManager::Get().GetGameData();
}
