// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "UObject/Object.h"
#include "GridUIManagerSubsystem.generated.h"

class FSubsystemCollectionBase;
class UObject;


UCLASS()
class PROJECTVGAME_API UGridUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()
	
public:
	UGridUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();
	
	FTSTicker::FDelegateHandle TickHandle;
};
