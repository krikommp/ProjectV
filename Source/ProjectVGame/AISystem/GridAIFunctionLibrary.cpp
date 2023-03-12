// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAIFunctionLibrary.h"

#include "GridAITargetType.h"

UGridAITargetType* UGridAIFunctionLibrary::GetAITargetType(TSubclassOf<UGridAITargetType> AITargetType)
{
	return AITargetType->GetDefaultObject<UGridAITargetType>();
}
