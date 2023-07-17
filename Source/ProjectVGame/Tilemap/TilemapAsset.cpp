// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapAsset.h"

void FTilemap3DTerrainMeshData::Clear()
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	Colors.Empty();
	UV0.Empty();
	VertexCount = 0;
}
