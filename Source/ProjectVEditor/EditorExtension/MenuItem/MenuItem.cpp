// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuItem.h"

typedef void(*FunctionPtr_t)(void*);

void FMenuItem::InitMenu(const FString& Path, const FString& Tooltip, UObject* InOwner, UFunction* Func)
{
	MenuPath = Path;
	MenuToolTip = Tooltip;
	MenuName = FPaths::GetPathLeaf(Path);
	Function = Func;
	Owner = InOwner;
	bLeaf = Function != nullptr ? true : false;
}

void FMenuItem::OnMenuClick()
{
	if (Owner.IsValid() && Function.IsValid())
	{
		FunctionPtr_t FuncPtr = reinterpret_cast<FunctionPtr_t>(Function->GetNativeFunc());
		FuncPtr(Owner.Get());
	}
	//UE_LOG(LogTemp, Error, TEXT("YES"));
}
