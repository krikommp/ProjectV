// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tilemap3DEditorManager.generated.h"

class FAssetTypeActions_Tilemap3DAsset;
enum ETilemap3DEditMode : uint8
{
	EEM_View = 0,

	EEM_Cube,
	EEM_Cube_Append,
	EEM_Cube_Remove,
	EEM_Cube_End,

	EEM_Mesh,
	EEM_Mesh_Select,
	EEM_Mesh_Append,
	EEM_Mesh_Remove,
	EEM_Mesh_End,

	EEM_Chess,
	EEM_Chess_Spawn,
	EEM_Chess_Remove,
	EEM_Chess_Select,
	EEM_Chess_End,
	
	UNKNOWN
};

using FTileMeshMap = TMap<FName, TObjectPtr<UInstancedStaticMeshComponent>>;

struct PROJECTVEDITOR_API FTilemap3DEditDelegates
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTilemapEditStatueChanged, bool);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTilemapEditModeChanged, const ETilemap3DEditMode /*= OldEditMode*/, const ETilemap3DEditMode /*= NewEditMode*/);
	static FOnTilemapEditStatueChanged OnTilemapEditStatueChanged;
	static FSimpleMulticastDelegate OnTilemapModelChanged;
	static FSimpleMulticastDelegate OnTilemapClearVoxel;
	static FSimpleMulticastDelegate OnTilemapGeneratePathFinding;
	static FOnTilemapEditModeChanged OnTilemapEditModeChanged;
	static FSimpleMulticastDelegate OnTilemapFillFloor;
};

DECLARE_DELEGATE_OneParam(FOnTileSetClicked, const FName&);

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
