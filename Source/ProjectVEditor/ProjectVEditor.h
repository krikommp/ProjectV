#pragma once

#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProjectVEditor, Log, All);

#define PathTrace ECC_GameTraceChannel1
#define WallTrace ECC_GameTraceChannel2
#define RangeTrace ECC_GameTraceChannel3

DECLARE_DELEGATE_OneParam(FOnTileSetClicked, const FName&);