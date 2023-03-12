// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "GameFeatureStateChangeObserver.h"
#include "GameFeaturesProjectPolicies.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "GridGameFeaturePolicy.generated.h"

class FName;
class UGameFeatureData;
struct FPrimaryAssetId;

/**
 * 
 */
UCLASS(MinimalAPI, Config = Game)
class UGridGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_BODY()

public:

	PROJECTVGAME_API static UGridGameFeaturePolicy& Get();

	UGridGameFeaturePolicy(const FObjectInitializer& ObjectInitializer);

	//~UGameFeaturesProjectPolicies interface
	virtual void InitGameFeatureManager() override;
	virtual void ShutdownGameFeatureManager() override;
	virtual TArray<FPrimaryAssetId> GetPreloadAssetListForGameFeature(const UGameFeatureData* GameFeatureToLoad, bool bIncludeLoadedAssets = false) const override;
	virtual bool IsPluginAllowed(const FString& PluginURL) const override;
	virtual const TArray<FName> GetPreloadBundleStateForGameFeature() const override;
	virtual void GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const override;
	//~End of UGameFeaturesProjectPolicies interface

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observers;
};
