#pragma once
#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"

template <typename T>
class PROJECTVGAME_API FWidgetObjectPool
{
public:
	FWidgetObjectPool(UWorld* WorldContextObject, int32 Num, TSubclassOf<T> InWidgetClass = T::StaticClass());
	
	T* GetWidget();
	void ReturnWidget(T* Widget); 
private:
	// 用于保存 Widget 数组
	TArray<TStrongObjectPtr<T>> WidgetPool;

	// 需要被实例化的 Widget 类型
	TSubclassOf<T> WidgetClass;

	TWeakObjectPtr<UWorld> WorldPtr;
};

template <typename T>
FWidgetObjectPool<T>::FWidgetObjectPool(UWorld* InWorldContextObject, int32 Num, TSubclassOf<T> InWidgetClass)
{
	WidgetClass = InWidgetClass;
	WorldPtr = InWorldContextObject;
	for (int32 i = 0; i < Num; ++i)
	{
		T* Widget = CreateWidget<T>(WorldPtr.Get(), WidgetClass);
		Widget->AddToViewport();
		Widget->SetVisibility(ESlateVisibility::Hidden);
		WidgetPool.Push(TStrongObjectPtr<T>(Widget));
	}
}

template <typename T>
T* FWidgetObjectPool<T>::GetWidget()
{
	static_assert(std::is_base_of<UUserWidget, T>::value, "T must a subclass of UUserWidget!");

	if (WidgetPool.Num() > 0)
	{
		T* Widget = WidgetPool.Pop().Get();
		Widget->AddToViewport();
		Widget->SetVisibility(ESlateVisibility::Visible);
		return Widget;
	}

	T* Widget = CreateWidget<T>(WorldPtr.Get(), WidgetClass);
	Widget->AddToViewport();
	Widget->SetVisibility(ESlateVisibility::Visible);
	return Widget;
}

template <typename T>
void FWidgetObjectPool<T>::ReturnWidget(T* Widget)
{
	static_assert(std::is_base_of<UUserWidget, T>::value, "T must a subclass of UUserWidget!");

	Widget->SetVisibility(ESlateVisibility::Hidden);
	Widget->RemoveFromParent();
	WidgetPool.Push(TStrongObjectPtr<T>(Widget));
}