#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include "Logging/MessageLog.h"

PROJECTVGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogGrid, Log, All);
PROJECTVGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogGridExperience, Log, All);
PROJECTVGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogGridAbilitySystem, Log, All);
PROJECTVGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogGridTeam, Log, All);
PROJECTVGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogGridHero, Log, All);
PROJECTVGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogGridAIEvaluation, Log, All);
PROJECTVGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogGridCard, Log, All);

PROJECTVGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);