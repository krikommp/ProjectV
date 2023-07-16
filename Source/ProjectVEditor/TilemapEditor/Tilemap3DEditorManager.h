// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tilemap3DEditorManager.generated.h"

class FAssetTypeActions_Tilemap3DAsset;

struct PROJECTVEDITOR_API FTilemap3DEditDelegates
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTilemapEditStatueChanged, bool);
	static FOnTilemapEditStatueChanged OnTilemapEditStatueChanged;
	static FSimpleMulticastDelegate OnTilemapModelChanged;
	static FSimpleMulticastDelegate OnTilemapClearVoxel;
};

/**
 * UTilemapEditorManager
 *
 * Tilemap 编辑管理器
 */
UCLASS()
class PROJECTVEDITOR_API UTilemap3DEditorManager final : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin UEngineSubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ Begin UEngineSubsystem interface

private:
	TSharedPtr<FAssetTypeActions_Tilemap3DAsset> ActionType;
};
