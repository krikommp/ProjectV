// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "GridMapWarFogComponent.generated.h"

class AGridMapManager;
class APostProcessVolume;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridMapWarFogComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

public:
	UGridMapWarFogComponent(FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="Grid|WarFog")
	static UGridMapWarFogComponent* FindWarFogComponent(AActor* Actor) { return Actor ? Actor->FindComponentByClass<UGridMapWarFogComponent>() : nullptr; } 

	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	
	/**
	 * @brief 启用战斗可见性迷雾
	 */
	UFUNCTION(BlueprintCallable, Category="Grid|WarFog")
	void EnableVisibleFog();

	/**
	 * @brief 隐藏可见性迷雾
	 */
	UFUNCTION(BlueprintCallable, Category="Grid|WarFog")
	void DisableVisibleFog();

	/**
	 * @brief 向材质传递可见范围数据
	 */
	UFUNCTION(BlueprintCallable, Category="Grid|WarFog")
	void PassVisibleRangeToMaterial(const TArray<int32>& InRangeArray);
protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Grid|WarFog", meta=(DisplayName="InitializeMaterial"))
	void K2_InitializeMaterial(APostProcessVolume* InPostProcessVolume, UMaterialInstanceDynamic* InMaterial);
	void InitializeMaterial(APostProcessVolume* InPostProcessVolume);
	void SetupMaterialParameters(UMaterialInstanceDynamic* InMaterial) const;
	void OnRemoveVisibleRangeDisplay();
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Grid|WarFog")
	TObjectPtr<UMaterialInstanceDynamic> WarFogVisibleRangeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|WarFog")
	TObjectPtr<UMaterialInterface> WarFogMaterial;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|WarFog")
	TObjectPtr<APostProcessVolume> PostProcessVolume;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> CurrentFogMaterial;

	UPROPERTY(Transient)
	TWeakObjectPtr<AGridMapManager> GridMapManagerPtr;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> WarFogVisibleRangeTexture;

	TArray<FLinearColor> TextureData;
};
