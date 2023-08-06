// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "GridChessMovementComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnChessMovementFinished)

/**
 * UGridChessMovementComponent
 *
 * 棋子移动组件，挂载到需要执行移动的Pawn上
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessMovementComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UGridChessMovementComponent* FindGridChessMovementComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UGridChessMovementComponent>() : nullptr);
	}

	UFUNCTION(BlueprintCallable)
	void SetMoveTarget(const FVector& Target);

	UFUNCTION(BlueprintCallable)
	void MoveToPathfinding(int32 Index);
	
protected:
	virtual void BeginPlay() override;

public:
	void Register_OnChessMovementFinished(FOnChessMovementFinished::FDelegate&& Delegate);

private:
	FOnChessMovementFinished OnChessMovementFinished;
};
