// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuItem.h"

void FMenuItem::InitMenu(const FString& Path, const FString& Tooltip, UObject* InOwner, const FName& InFuncName)
{
	MenuPath = Path;
	MenuToolTip = Tooltip;
	MenuName = FPaths::GetPathLeaf(Path);
	Owner = InOwner;
	FunctionName = InFuncName;
	bLeaf = Owner != nullptr ? true : false;
}

void FMenuItem::OnMenuClick() const
{
	if (!Owner.IsValid()) return;

	UFunction* Function = Owner->FindFunction(FunctionName);

	if (Function == nullptr) return;

	Owner->ProcessEvent(Function, nullptr);
}
