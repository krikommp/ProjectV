#include "GridAssetManagerStartupJob.h"


#include "HAL/Platform.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "GridLogChannel.h"
#include "Trace/Detail/Channel.h"

TSharedPtr<FStreamableHandle> FGridAssetManagerStartupJob::DoJob() const
{
	const double JobStartTime = FPlatformTime::Seconds();

	TSharedPtr<FStreamableHandle> Handle;
	UE_LOG(LogGrid, Display, TEXT("Startup job \"%s\" starting"), *JobName);
	JobFunc(*this, Handle);

	if (Handle.IsValid())
	{
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(this, &FGridAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
		Handle->WaitUntilComplete(0.0f, false);
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	UE_LOG(LogGrid, Display, TEXT("Startup job \"%s\" took %.2f seconds to complete"), *JobName, FPlatformTime::Seconds() - JobStartTime);

	return Handle;
}
