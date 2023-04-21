// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/PawnComponent.h"
#include "GridChessPieceMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChessPieceBeginMovementDynamic);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChessPieceEndMovementDynamic);

class UGridAbilitySystemComponent;
class UGridActionSet;
class UGridMoveSet;
class AGridMapManager;
class AGridChessPiece;
class UGridGameAbility_Move;

/**
 * UGridChessPieceMovementComponent
 *
 * 棋子移动组件
 * 适用于棋子在棋盘上移动
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessPieceMovementComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UGridChessPieceMovementComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	static UGridChessPieceMovementComponent* FindChessPieceMovementComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGridChessPieceMovementComponent>() : nullptr); }

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	FORCEINLINE UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	int32 GetMoveRange() const;

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	int32 GetMaxMoveRange() const;

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	int32 GetMovePreStepCost() const;

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	int32 GetVisibleRange() const;

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	float GetMaxSpeed() const;

	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	float GetAcceleration() const;

	// 开始寻路
	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	void StartToPathFinding(bool bDisplayTiles);

	// 结束寻路
	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	void EndPathFinding();

	// Hover Tile 时执行
	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	bool DecideWhatToDoWhenHover(int32 HoverTileIndex);

	// 点击地面执行
	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	void DecideActionForClickedTile(APlayerController* InController, int32 ClickedTileIndex);

	// 执行移动
	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	void BeginMovement(int32 InTileIndex, int32 InStopXFromTarget = 0);

	// 激活移动技能
	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	void ActivateMovement(int32 InTileIndex, int32 InStopXFromTarget = 0);

	// 判断是否需要执行移动
	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	bool CheckNeedMove(int32 InTileIndex, int32 InStopXFromTarget, bool bDisplayPath, int32& OutEndTileIndex);

	// 判断是否允许移动
	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
	bool CheckMove(TSubclassOf<UGridGameAbility_Move> InMoveAbility) const;

	// 获取当前速度
	UFUNCTION(BlueprintPure, Category="Grid|ChessPieceMovement")
 	FORCEINLINE float GetCurrentSpeed() const { return CurrentSpeed; }

	UFUNCTION(BlueprintCallable, Category="Grid|ChessPieceMovement")
	FORCEINLINE void SetCurrentSpeed(float InSpeed) { CurrentSpeed = InSpeed; }
protected:

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	void OnAbilitySystemInitialized();
	void OnAbilitySystemUninitialized();
	void OnGridMapManagerInitialize(AGridMapManager* InGridMapManager);

	void HandleChessPieceSelectChanged(AGridChessPiece* OldChessPiece, AGridChessPiece* NewChessPiece);
	void HandleChessPieceTurnOver();

public:
	// 棋子开始移动事件
	UPROPERTY(BlueprintAssignable, Category="Grid|ChessPieceMovement", meta=(DisplayName="OnChessPieceBeginMovement"))
	FOnChessPieceBeginMovementDynamic OnChessPieceBeginMovementDynamic;

	// 棋子结束移动事件
	UPROPERTY(BlueprintAssignable, Category="Grid|ChessPieceMovement", meta=(DisplayName="OnChessPieceEndMovement"))
	FOnChessPieceEndMovementDynamic OnChessPieceEndMovementDynamic;

	void Internal_BeginMovement();
	void Internal_EndMovement();
private:

	UPROPERTY(Transient)
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient)
	TObjectPtr<const UGridActionSet> ActionSet;

	UPROPERTY(Transient)
	TObjectPtr<const UGridMoveSet> MoveSet;
	
	TWeakObjectPtr<AGridMapManager> GridMapManager;

	float CurrentSpeed;
};
