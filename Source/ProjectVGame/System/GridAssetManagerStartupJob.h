#pragma once


#include "Containers/UnrealString.h"
#include "Delegates/Delegate.h"
#include "Engine/StreamableManager.h"
#include "HAL/PlatformTime.h"
#include "Templates/Function.h"
#include "Templates/SharedPointer.h"

DECLARE_DELEGATE_OneParam(FGridAssetManagerStartupJobSubstepProgress, float /*NewProgress*/);

/** Handles reporting progress from streamable handles */
struct FGridAssetManagerStartupJob
{
	FGridAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
	TFunction<void(const FGridAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> JobFunc;
	FString JobName;
	float JobWeight;
	mutable double LastUpdate = 0;

	/** Simple job that is all synchronous */
	FGridAssetManagerStartupJob(const FString& InJobName, const TFunction<void(const FGridAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, float InJobWeight)
		: JobFunc(InJobFunc)
		, JobName(InJobName)
		, JobWeight(InJobWeight)
	{}

	/** Perform actual loading, will return a handle if it created one */
	TSharedPtr<FStreamableHandle> DoJob() const;

	void UpdateSubstepProgress(float NewProgress) const
	{
		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
	}

	void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
	{
		if (SubstepProgressDelegate.IsBound())
		{
			// StreamableHandle::GetProgress traverses() a large graph and is quite expensive
			double Now = FPlatformTime::Seconds();
			if (LastUpdate - Now > 1.0 / 60)
			{
				SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
				LastUpdate = Now;
			}
		}
	}
};
