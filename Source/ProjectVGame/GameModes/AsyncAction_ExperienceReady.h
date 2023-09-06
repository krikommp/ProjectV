// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "AsyncAction_ExperienceReady.generated.h"

class AGameStateBase;
class UGridExperienceDefinition;
class UWorld;
struct FFrame;

UENUM(BlueprintType)
enum class EExperienceLoadPriority : uint8
{
	EEP_Low,
	EEP_Normal,
	EEP_High,
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExperienceReadyAsyncDelegate);

/**
 * UAsyncAction_ExperienceReady
 *
 * 异步等待 Experience 加载完毕
 */
UCLASS()
class PROJECTVGAME_API UAsyncAction_ExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_ExperienceReady* WaitForExperienceReady(UObject* WorldContextObject, const EExperienceLoadPriority ExperienceLoadPriority);

	virtual void Activate() override;

public:

	UPROPERTY(BlueprintAssignable)
	FExperienceReadyAsyncDelegate OnReady;

private:

	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToExperienceLoading(AGameStateBase* GameState);
	void Step3_HandleExperienceLoaded(const UGridExperienceDefinition* CurrentExperience);
	void Step4_BroadcastReady();

	TWeakObjectPtr<UWorld> WorldPtr;
	EExperienceLoadPriority LoadPriority = EExperienceLoadPriority::EEP_Normal;
};
