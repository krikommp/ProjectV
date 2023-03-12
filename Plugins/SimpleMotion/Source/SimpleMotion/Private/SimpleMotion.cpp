// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleMotionPrivatePCH.h"

#define LOCTEXT_NAMESPACE FRAMEWORK_LOCTEXT_NAMESPACE
DEFINE_LOG_CATEGORY(LogSimpleMotion)

void FSimpleMotionModule::StartupModule()
{
	FRAMEWORK_LOG_MESSAGE(TEXT("Starting the SimpleMotion module."));

	FRAMEWORK_LOG_MESSAGE(TEXT("SimpleMotion module startup completed."));
}

void FSimpleMotionModule::ShutdownModule()
{
	FRAMEWORK_LOG_MESSAGE(TEXT("Shutting down the SimpleMotion module."));

	FRAMEWORK_LOG_MESSAGE(TEXT("SimpleMotion Module Shutting dowm completed."));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleMotionModule, SimpleMotion)