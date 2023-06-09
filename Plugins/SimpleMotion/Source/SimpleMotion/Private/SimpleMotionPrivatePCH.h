﻿#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include "Modules/ModuleManager.h"

#include "SimpleMotion.h"
#include "MotionBase.h"

#define FRAMEWORK_MODULE "SimpleMotion"

#define FRAMEWORK_MODULE_VERSION_MAJOR (0)
#define FRAMEWORK_MODULE_VERSION_MINDR (1)

#define FRAMEWORK_LOCTEXT_NAMESPACE FRAMEWORK_MODULE

DECLARE_LOG_CATEGORY_EXTERN(LogSimpleMotion, Log, All);

#define FRAMEWORK_LOGGING 1

#ifdef FRAMEWORK_LOGGING
#define FRAMEWORK_LOG_HELPER(VERBOSITY, SIMPLEMOTION_LOG_TEXT, ...)\
do \
{\
UE_LOG(LogSimpleMotion, VERBOSITY, SIMPLEMOTION_LOG_TEXT, ##__VA_ARGS__ );\
}\
while ( 0 )

#define FRAMEWORK_LOG_MESSAGE(SIMPLEMOTION_LOG_TEXT, ...) \
FRAMEWORK_LOG_HELPER(Log, SIMPLEMOTION_LOG_TEXT, ##__VA_ARGS__)

#define FRAMEWORK_LOG_FATAL(SIMPLEMOTION_LOG_TEXT, ...) \
FRAMEWORK_LOG_HELPER(Fatal, SIMPLEMOTION_LOG_TEXT, ##__VA_ARGS__)

#define FRAMEWORK_LOG_ERROR(SIMPLEMOTION_LOG_TEXT, ...) \
FRAMEWORK_LOG_HELPER(Error, SIMPLEMOTION_LOG_TEXT, ##__VA_ARGS__)

#define FRAMEWORK_LOG_Warning(SIMPLEMOTION_LOG_TEXT, ...) \
FRAMEWORK_LOG_HELPER(Warning, SIMPLEMOTION_LOG_TEXT, ##__VA_ARGS__)

#define FRAMEWORK_LOG_DISPLAY(SIMPLEMOTION_LOG_TEXT, ...) \
FRAMEWORK_LOG_HELPER(Display, SIMPLEMOTION_LOG_TEXT, ##__VA_ARGS__)

#endif