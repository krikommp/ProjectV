// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSet3DAsset.h"

FTileSet3DCube FTileSet3DCube::EmptyBlock;

FTileSet3DCube::FTileSet3DCube()
	: ID(FName()), BlockType(EBlock::Air), Cost(MAX_int32)
{
}
