// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Components/ActorComponent.h"
#include "Components/PlayerStateComponent.h"
#include "ChessPackageComponent.generated.h"

class UGridHeroInfo;
class UChessPackageInfo;
class UGridExperienceDefinition;
class USaveGame;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChessPackageLoaded, const UChessPackageInfo* /*ChessPackage*/);
/**
 * UChessPackageComponent
 *
 * 用于获取 ChessPackageInfo 的组件
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UChessPackageComponent : public UPlayerStateComponent, public ILoadingProcessInterface
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category=ChessPackage)
	static UChessPackageComponent* FindChessPackageComponent(const ULocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable, Category=ChessPackage)
 	UChessPackageInfo* GetChessPackageInfo() const;
	UFUNCTION(BlueprintCallable, Category=ChessPackage)
	UGridHeroInfo* GetChessInfo(const FName& ChessID) const;
	
protected:
	// ~Begin UActorComponent Interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~Begins UActorComponent Interface

	// ~Begin ILoadingProcessInterface Interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~Begin ILoadingProcessInterface Interface

	void OnExperienceLoaded(const UGridExperienceDefinition* Experience);
	void Step1_HandleAsyncLoad(const FString& InSlotName, const int32 InUserIndex, USaveGame* LoadedSave);
	void Step2_LoadChessInfo();
	
public:
	void CallOrRegister_OnChessPackageLoaded(FOnChessPackageLoaded::FDelegate&& Delegate);
	
private:
	// 棋子存档
	UPROPERTY(Transient)
	TObjectPtr<UChessPackageInfo> ChessPackageInfo;
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UGridHeroInfo>> ChessInfoMap;

	// 加载完毕标志位
	uint8 bChessInfoLoaded : 1;
	
	// 棋子包加载完成事件
	FOnChessPackageLoaded OnChessPackageLoaded;
};
