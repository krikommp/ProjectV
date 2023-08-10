#include "Tilemap3DPlayerChessStartGenerator.h"

#include "Components/DecalComponent.h"
#include "Tilemap/TilemapAsset.h"
#include "TilemapEditor/Tilemap3DEditorSettings.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

void FTilemap3DPlayerChessStartGenerator::Setup(FTilemap3DEditorViewportClient* ViewportClient,
                                                UTilemapAsset* InTilemapAsset)
{
	const int32 Count = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY * InTilemapAsset->Floors;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		UBlock* Block = InTilemapAsset->Blocks[Index];

		if (Block->bPlayerChessStart == false)
			continue;

		FTransform Transform = FTransform::Identity;
		FVector Location = ViewportClient->GetTilemapAsset()->IndexToVector(Index);
		Location.Z = Index / (ViewportClient->GetTilemapAsset()->LevelSizeX * ViewportClient->GetTilemapAsset()->
			LevelSizeY) * ViewportClient->GetTilemapAsset()->GridSize + ViewportClient->GetTilemapAsset()->GridSize;
		Location.Z += 1.0f;
		Transform.SetLocation(Location);
		Transform.SetRotation(FQuat({90.0f, 0.0f, 0.0f}));

		UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();

		const FVector DecalSize = { 95.0f, 45.0f, 45.0f };
		UDecalComponent* DecalComponent = NewObject<UDecalComponent>();
		ViewportClient->GetPreviewScene()->AddComponent(DecalComponent, Transform);
		DecalComponent->bAllowAnyoneToDestroyMe = true;
		DecalComponent->SetDecalMaterial(Settings->DefaultPlayerChessRangeMat.LoadSynchronous());
		DecalComponent->DecalSize = DecalSize;
		DecalComponent->SetUsingAbsoluteScale(true);

		Block->PlayerStartDecal = DecalComponent;
	}
}
