// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridUserFacingExperienceDefinition.generated.h"

class UTexture2D;
class UUserWidget;
class UCommonSession_HostSessionRequest;

/**
 * UGridUserFacingExperienceDefinition
 *
 * 用于在 UI 中显示 Experience， 以及开始新的会话
 */
UCLASS(BlueprintType)
class PROJECTVGAME_API UGridUserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// 定义需要被加载的场景 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="Map"))
	FPrimaryAssetId MapID;

	// 定义需要被加载的 Experience
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="GridExperienceDefinition"))
	FPrimaryAssetId ExperienceID;

	// 需要作为 URL 传递的额外参数
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	TMap<FString, FString> ExtraArgs;

	// 标题
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	FText TileTitle;

	// 副标题
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	FText TileSubTitle;

	// 描述
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	FText TileDescription;

	// 需要显示在 UI 上的图标
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	TObjectPtr<UTexture2D> TileIcon;

	// 在加载这个 Experience 时需要显示的 LoadingScreen
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	TSoftClassPtr<UUserWidget> LoadingScreenWidget;

	// If true, 代表这个 Experience 将具有最高的优先级，将会被首先显示在 UI 上
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	bool bIsDefaultExperience = false;

	// If true, 代表会显示在前端
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	bool bShowInFrontEnd = true;

	// 当前会话最大可支持玩家数量
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	int32 MaxPlayerCount = 16;

public:

	// 创建一个 Request 对象
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;
};
