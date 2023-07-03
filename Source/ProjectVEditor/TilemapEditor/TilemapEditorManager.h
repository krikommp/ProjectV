// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TilemapEditorManager.generated.h"

class FAssetTypeActions_TilemapAsset;

struct PROJECTVEDITOR_API FTilemapEditDelegates
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTilemapEditStatueChanged, bool);
	static FOnTilemapEditStatueChanged OnTilemapEditStatueChanged;
};

/**
 * UTilemapEditorManager
 *
 * Tilemap 编辑管理器
 */
UCLASS()
class PROJECTVEDITOR_API UTilemapEditorManager final : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin UEngineSubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ Begin UEngineSubsystem interface

private:
	TSharedPtr<FAssetTypeActions_TilemapAsset> ActionType;
};
