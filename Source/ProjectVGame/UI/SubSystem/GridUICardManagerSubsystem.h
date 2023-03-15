// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/WidgetObjectPool.h"
#include "UObject/UnrealTypePrivate.h"

#include "GridUICardManagerSubsystem.generated.h"

class UGridCard;

UCLASS(Const, BlueprintType)
class UGridPlayerHandConfigDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 卡牌失效时的透明度
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CardDisableOpacity = 0.1f;

	// 抽牌时卡牌移动速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CardDrawSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoverScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoverOffsetY = -110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoverOffsetJumpPropY = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoverOffset = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoverSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DragSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DiscardSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FirstRenderZOrder = 9999;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CardSpacingInHand = 240.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CardLinedSpacingInHand = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CardAngleFromCenter = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SelectTargetOffsetY = -110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SelectTargetOffsetJumpPropY = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UE5SelectTargetOffset = -90.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SelectTargetSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D CardSizeInHand = { 384 * 0.6f, 512 * 0.6f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoverOtherCardMoveX = 5000.0f;
};

/**
 * UGridUICardManagerSubsystem
 *
 * UI显示卡牌的管理器
 */
UCLASS(Config=Game)
class PROJECTVGAME_API UGridUICardManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGridUICardManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category="Grid|UICardSubsystem")
	UGridCard* GetCardWidget() const;

	UFUNCTION(BlueprintCallable, Category="Grid|UICardSubSystem")
	void ReturnCardWidget(UGridCard* ReturnCard) const;

	UFUNCTION(BlueprintPure)
	const UGridPlayerHandConfigDataAsset* GetPlayerHandConfig() const;

	template <typename T>
	bool GetValue(FName FieldName, T& RetValue) const;

	template <typename T>
	T GetValue(FName FieldName) const;
private:
	// 卡牌对象池
	UPROPERTY(Config)
	TSubclassOf<UGridCard> CardDefaultClass;

	// 默认在池中的卡牌数量
	UPROPERTY(Config)
	int32 DefaultCardNumInPool = 10;

	TSharedPtr<FWidgetObjectPool<UGridCard>> CardWidgetPool;

	UPROPERTY(Config)
	TSoftObjectPtr<UGridPlayerHandConfigDataAsset> ConfigDataAsset;
};

template <typename T>
bool UGridUICardManagerSubsystem::GetValue(FName FieldName, T& RetValue) const
{
	check(ConfigDataAsset.IsValid());

	if (const FProperty* Property= FindUFieldOrFProperty<FProperty>(UGridPlayerHandConfigDataAsset::StaticClass(), FieldName))
	{
		RetValue = *Property->ContainerPtrToValuePtr<T>(ConfigDataAsset.Get());
		return true;
	}

	return false;
}

template <typename T>
T UGridUICardManagerSubsystem::GetValue(FName FieldName) const
{
	T RetValue;
	check(GetValue(FieldName, RetValue));
	return RetValue;
}
