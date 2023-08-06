// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonInputModeTypes.h"
#include "GridMasterComponent.h"
#include "InputActionValue.h"
#include "UObject/Object.h"
#include "GridTurnBaseMasterComponent.generated.h"

class AGridMapManager;
class USpringArmComponent;
class AGridChessPiece;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridOnMouseButtonDown, const FGameplayTag&, TargetTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridOnMouseConfirmButtonDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridOnMouseCancelButtonDown);

/**
 * UGridTurnBaseMasterComponent
 *
 * 基于战旗网格的Master组件，需要场景中拥有 GridMapManager
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridTurnBaseMasterComponent : public UGridMasterComponent
{
	GENERATED_BODY()

public:

	UGridTurnBaseMasterComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category="Grid|TurnBaseMaster")
	static UGridTurnBaseMasterComponent* FindTurnBaseMasterComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGridTurnBaseMasterComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category="Grid|TurnBaseMaster")
	FORCEINLINE void SetEdgeScrolling(bool InEdgeScrolling) { bEdgeScrolling = InEdgeScrolling; }

	UFUNCTION(BlueprintCallable, Category="Grid|TurnBaseMaster")
	FORCEINLINE void SetIsPanning(bool InIsPanning) { bIsPanning = InIsPanning; }
	
protected:
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent) override;

	void Input_CameraMove(const FInputActionValue& InputActionValue);
	void Input_CameraRotate(const FInputActionValue& InputActionValue);
	void Input_MouseWheel(const FInputActionValue& InputActionValue);
	void Input_MouseConfirm(const FInputActionValue& InputActionValue);
	void Input_MouseCancel(const FInputActionValue& InputActionValue);

	void HandleActiveInputModeChanged(ECommonInputMode InputMode);
	void HandleChessPieceSelectChanged(AGridChessPiece* OldChessPiece, AGridChessPiece* NewChessPiece);

private:
	// 鼠标到达边界时移动屏幕
	void EdgeScrollingTick(float& UIInputRightLeft, float& UIInputUpDown) const;

	// 跟随相机目标
	void FollowCameraTarget() const;

	// 相机手动移动
	void CameraControllerMove();
	
	// void OnGridMapInitialized(AGridMapManager* InGridMapManager);
	// // 判断是否超出地图边界
	// FVector CheckOuterBounds(const FVector& InLocation) const;

public:

	UPROPERTY(BlueprintAssignable, Category="Grid|TurnBaseMaster")
	FGridOnMouseButtonDown OnMouseButtonDown;

	UPROPERTY(BlueprintAssignable, Category="Grid|TurnBaseMaster")
	FGridOnMouseConfirmButtonDown OnMouseConfirmButtonDown;

	UPROPERTY(BlueprintAssignable, Category="Grid|TurnBaseMaster")
	FGridOnMouseCancelButtonDown OnMouseCancelButtonDown;
	
private:
	bool bEdgeScrolling = false;

	bool bCanPan = false;

	bool bCanZoom = true;

	bool bCanRotate = true;

	bool bFollowCustomTarget = false;

	bool bIsPanning = false;

	float EdgeScrollingEdgeSize = 0.025f;

	float CameraMoveSpeed = 20.0f;

	float CameraZoomSpeed = 400.0f;

	float CameraRotateSpeed = 2.0f;

	float ZoomRoof = 4800.0f;

	float ZoomFloor = 200.0f;

	FVector2D CameraMove;

	FVector CustomTarget;
};
