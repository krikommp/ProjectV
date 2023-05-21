#include "GridLogChannel.h"

DEFINE_LOG_CATEGORY(LogGrid)
DEFINE_LOG_CATEGORY(LogGridExperience)
DEFINE_LOG_CATEGORY(LogGridAbilitySystem)
DEFINE_LOG_CATEGORY(LogGridTeam)
DEFINE_LOG_CATEGORY(LogGridHero)
DEFINE_LOG_CATEGORY(LogGridAIEvaluation)
DEFINE_LOG_CATEGORY(LogGridCard)

FString GetClientServerContextString(UObject* ContextObject)
{
	ENetRole Role = ROLE_None;

	if (AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		return (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
	}
	else
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			extern ENGINE_API FString GPlayInEditorContextString;
			return GPlayInEditorContextString;
		}
#endif
	}

	return TEXT("[]");
}
