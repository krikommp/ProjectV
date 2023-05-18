

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "GridGameplayEffect_Attach.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridGameplayEffect_Attach : public UGameplayEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GridMap, meta = (DisplayName = "AttachGameplayEffects"))
	TArray<TSubclassOf<UGameplayEffect>> AttachGameplayEffects;
};
