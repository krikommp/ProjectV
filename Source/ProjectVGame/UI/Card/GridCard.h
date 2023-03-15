// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GridCard.generated.h"

class UGridPlayerHand;
class UGridCardInfo;
class UGridBaseCardState;
class USizeBox;
class USMInstance;
class UGridCardStyle;
class AGridChessPiece;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCardPointerEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCardPointerLeave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCardMotionEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardPointerDown, const FPointerEvent&, InMouseEvent);

UENUM(BlueprintType)
enum class ECardState : uint8
{
	// 初始状态
	Init,
	
	// 抽牌状态
	Draw,

	// 闲置状态
	Idle,

	// 悬浮状态
	Hover,
	
	// 拖拽状态
	Drag,
	
	// 打出状态
	Play,
	
	// 失效状态
	Disable,
	
	// 丢弃状态
	Discard,

	// 使用状态
	Use,

	// 取消Hover
	UnHover,
	
	Unknown UMETA(Hidden),
};

/**
 * UGridCard
 *
 * 抽象的卡牌类
 * 无法直接实例化，需要通过蓝图继承
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta=(ToolTip="Base Card Class"))
class PROJECTVGAME_API UGridCard : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UGridCard(const FObjectInitializer& ObjectInitializer);

	// ~UUserWidget interface
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	// ~UUserWidget interface

	void RecycleCard();

	void InitializeStateMachine();

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|Card", meta=(DisplayName="SetupCardInfo"))
	void ReceiveSetupCardInfo(UGridCardInfo* NewCardInfo);
	void SetupCardInfo(UGridCardInfo* InCardInfo);
	
	// 将卡牌移动到一个合适的位置
	UFUNCTION(BlueprintCallable, Category="Grid|CardMotion")
	void RequestDesiredTransformUpdate(const FWidgetTransform& DesiredTransform, bool bImmediately = false);

	// 设置卡牌深度，用于排序
	UFUNCTION(BlueprintCallable, Category="Motion")
	void SuggestZOrder(int32 InZOrder);

	// Tick 中移动卡牌到指定位置
	bool MoveToSuggestedTransform(const FWidgetTransform& InSuggestedTransform, float InSpeed, float InDeltaTime);

	UFUNCTION(BlueprintCallable, Category="Card|Motion")
	bool MoveToSuggestedTransform(float InSpeed, float InDeltaTime) { return MoveToSuggestedTransform(SuggestedTransform, InSpeed, InDeltaTime); }

	UFUNCTION(BlueprintPure, Category="Grid|Motion")
	FVector2D GetMousePosition();

	// 设置卡牌大小
	void SetCardSize(const FVector2D& NewCardSize) const;

	// 检查当前卡牌运动是否结束
	FORCEINLINE bool CheckCardMotionEnded() const { return bCardMotionEnded; }

	// 卡牌输入开关
	UFUNCTION(BlueprintCallable, Category="Grid|Card")
	void GateCardInput();

	void DoStart() const;
	void DoUpdate(float DeltaTime) const;
	void DoStop() const;
	void DoShutDown();

	UFUNCTION(BlueprintCallable, Category="Grid|State")
	void StopStateMachine();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="CardStyle")
	void SetupCardStyle(UGridCardStyle* InCardStyle);

	UFUNCTION(BlueprintCallable, Category="CardStyle")
	void CheckCardStyle();
protected:
	// 卡牌状态机类
	UPROPERTY(EditAnywhere, Category="Grid|State")
	TSubclassOf<USMInstance> CardStateMachineClass;

	// 卡牌状态机实例
	UPROPERTY(Transient)
	USMInstance* CardStateMachineInstance;

	void CreateCardStateMachineInstance();

	void HandleChessPieceActionOver();
	void HandleChessPieceChanged(AGridChessPiece* InOld, AGridChessPiece* InNew);
public:
	// 当前持有该卡牌的 Hand
	// 可能为空
	UPROPERTY(BlueprintReadOnly, Transient, Category="Grid|PlayerHand")
	TObjectPtr<UGridPlayerHand> PlayerHand;

	UPROPERTY(BlueprintAssignable, Category="Card|Event")
	FOnCardPointerEnter OnCardPointerEnter;

	UPROPERTY(BlueprintAssignable, Category="Card|Event")
	FOnCardPointerLeave OnCardPointerLeave;

	UPROPERTY(BlueprintAssignable, Category="Card|Event")
	FOnCardPointerDown OnCardPointerDown;

	UPROPERTY(BlueprintAssignable, Category="Grid|Event")
	FOnCardMotionEnded OnCardMotionEnded;
	
private:
	// 卡牌先前的移动速度
	float PreviousSpeed;

	// 期望卡牌移动到的位置
	FWidgetTransform SuggestedTransform;

	// 卡牌大小组件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> CardSizeBox;

	// 当前卡牌运动是否完成
	bool bCardMotionEnded = true;
public:
	// 卡牌信息
	UPROPERTY(BlueprintReadOnly, Category="Grid|Card", meta=(AllowPrivateAccess=true))
	TObjectPtr<UGridCardInfo> CardInfo;

	// 卡牌状态
	UPROPERTY(BlueprintReadWrite, Category="Card|State")
	ECardState CardState = ECardState::Init;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "CardStyle")
	TObjectPtr<UGridCardStyle> NormalCardStyle;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "CardStyle")
	TObjectPtr<UGridCardStyle> CostCardStyle;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "CardStyle")
	TObjectPtr<UGridCardStyle> BlockCardStyle;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "CardStyle")
	TObjectPtr<UGridCardStyle> DisableCardStyle;
};
