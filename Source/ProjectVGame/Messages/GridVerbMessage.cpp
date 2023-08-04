// Fill out your copyright notice in the Description page of Project Settings.


#include "GridVerbMessage.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// FGridVerbMessage

FString FGridVerbMessage::ToString() const
{
	FString HumanReadableMessage;
	FGridVerbMessage::StaticStruct()->ExportText(/*out*/ HumanReadableMessage, this, /*Defaults=*/ nullptr, /*OwnerObject=*/ nullptr, PPF_None, /*ExportRootScope=*/ nullptr);
	return HumanReadableMessage;
}

FString FGridMouseMessage::ToString() const
{
	FString HumanReadableMessage;
	FGridMouseMessage::StaticStruct()->ExportText(/*out*/ HumanReadableMessage, this, /*Defaults=*/ nullptr, /*OwnerObject=*/ nullptr, PPF_None, /*ExportRootScope=*/ nullptr);
	return HumanReadableMessage;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

