// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSet3DAsset.h"

FTileSet3DSubObject FTileSet3DSubObject::EmptyBlock;

FTileSet3DSubObject::FTileSet3DSubObject()
	: ID(FName()), BlockType(EBlock::Air), Cost(MAX_int32)
{
}
