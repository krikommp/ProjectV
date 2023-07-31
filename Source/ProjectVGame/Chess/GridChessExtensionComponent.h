// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponents.h"
#include "Components/ActorComponent.h"
#include "GridChessExtensionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessExtensionComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
	FIND_COMP(GridChessExtensionComponent)

protected:
	virtual void BeginPlay() override;

public:
};
