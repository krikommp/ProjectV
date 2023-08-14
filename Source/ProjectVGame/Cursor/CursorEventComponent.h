// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "CursorEventComponent.generated.h"

struct FGridMouseMessage;

UENUM(BlueprintType)
enum class EMouseCursorState : uint8
{
	ECS_UnHover,
	ECS_Hover,
	ECS_Hovering,
};

/**
 * UCursorHoverComponent
 *
 * 鼠标悬停组件
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UCursorEventComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UCursorEventComponent* FindCursorEventComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UCursorEventComponent>() : nullptr);
	}

protected:
	// Called when the game starts
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ListenForMouseHoverEvent(FGameplayTag EventTag, const FGridMouseMessage& Message);
	void ListenForMouseClickEvent(FGameplayTag EventTag, const FGridMouseMessage& Message);
public:
	void OnMouseBeginHover();
	void OnMouseHovering();
	void OnMouseEndHover();

private:
	// 鼠标悬停状态
	EMouseCursorState MouseCursorState;

	FGameplayMessageListenerHandle MouseHoverListenerHandle;
	FGameplayMessageListenerHandle MouseClickListenerHandle;
};
