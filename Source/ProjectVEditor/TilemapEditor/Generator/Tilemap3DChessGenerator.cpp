#include "Tilemap3DChessGenerator.h"

#include "Tilemap/TilemapAsset.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Preview/Tilemap3DPreviewChess.h"

void FTilemap3DChessGenerator::Setup(FTilemap3DEditorViewportClient* ViewportClient, UTilemapAsset* InTilemapAsset, const UTileSet3DAsset* InTileSetAsset)
{
	const int32 Count = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY * InTilemapAsset->Floors;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		FBlock& Block = InTilemapAsset->Blocks[Index];

		if (!InTileSetAsset->ChessMap.Contains(Block.ChessID))
		{
			continue;
		}
		const auto ChessData = InTileSetAsset->ChessMap[Block.ChessID];
		auto ChessPiece = ViewportClient->GetWorld()->SpawnActor<ATilemap3DPreviewChess>();
		ChessPiece->SetupSkeletalMeshAsset(ChessData.SkeletalMesh);
		ChessPiece->SetActorTransform(Block.ChessTransform);
		Block.ChessInEditor = ChessPiece;
	}
}
