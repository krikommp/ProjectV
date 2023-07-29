#include "Tilemap3DChessGenerator.h"

#include "ProjectVEditor.h"
#include "ChessPieces/GridChessPieceData.h"
#include "Tilemap/TilemapAsset.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Preview/Tilemap3DPreviewChess.h"

void FTilemap3DChessGenerator::Setup(FTilemap3DEditorViewportClient* ViewportClient, UTilemapAsset* InTilemapAsset, const UTileSet3DAsset* InTileSetAsset)
{
	const int32 Count = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY * InTilemapAsset->Floors;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		UBlock* Block = InTilemapAsset->Blocks[Index];

		if (Block->ChessData == nullptr)
		{
			continue;
		}
		if (!InTileSetAsset->ChessMap.Contains(Block->ChessData->PieceID))
		{
			continue;
		}
		const auto ChessData = InTileSetAsset->ChessMap[Block->ChessData->PieceID];
		auto ChessPiece = ViewportClient->GetWorld()->SpawnActor<ATilemap3DPreviewChess>();
		ChessPiece->SetupSkeletalMeshAsset(ChessData.SkeletalMesh);
		ChessPiece->SetCollisionResponseToChannel(TilemapChessTrace, ECR_Block);
		ChessPiece->SetActorTransform(Block->ChessData->ChessTransform);
		Block->ChessInEditor = ChessPiece;
	}
}
