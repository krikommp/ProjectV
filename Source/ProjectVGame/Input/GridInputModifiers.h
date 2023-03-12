// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "UObject/Object.h"
#include "GridInputModifiers.generated.h"

/**
 * UGridSettingBasedScalar
 * 
 * 根据 SharedUserSettings 中的 double 属性缩放输入
 */
UCLASS(NotBlueprintable, MinimalAPI, meta=(DisplayName="Setting Based Scalar"))
class UGridSettingBasedScalar : public UInputModifier
{
	GENERATED_BODY()

public:
	// 用于限制 X轴 输入的大小
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName XAxisScalarSettingName = NAME_None;

	// 用于限制 Y轴 输入的大小
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName YAxisScalarSettingName = NAME_None;

	// 用于限制 Z轴 输入的大小
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName ZAxisScalarSettingName = NAME_None;

	// 设置每个轴的输入最大值
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FVector MaxValueClamp = FVector(10.0f, 10.0f, 1.0f);

	// 设置每个轴的输入最小值
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FVector MinValueClamp = FVector::ZeroVector;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;

	// 缓存 Settings 已经被查找到的 Property, 这样我们就不需要每帧都通过反射来获取属性，提高性能
	TArray<const FProperty*> PropertyCache;
};

/**
 * UGridInputModifierAimInversion
 *
 * 基于 Shared Settings，应用轴翻转
 */
UCLASS(NotBlueprintable, MinimalAPI, meta=(DisplayName="Grid Aim Inversion Setting"))
class UGridInputModifierAimInversion : public UInputModifier
{
	GENERATED_BODY()

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};