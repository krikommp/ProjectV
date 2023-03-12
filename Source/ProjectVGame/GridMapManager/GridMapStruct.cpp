// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapStruct.h"

FStructRange::FStructRange() : Index(0), Distance(0)
{
}

FStructRange::FStructRange(int32 InIndex, int32 InDistance) : Index(InIndex), Distance(InDistance)
{
}

FStructPathFinding::FStructPathFinding() : Index(0), Cost(0), Parent(0)
{
	
}

FStructPathFinding::FStructPathFinding(int32 InIndex, int32 InCost, int32 InParent) : Index(InIndex), Cost(InCost), Parent(InParent)
{
}