// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Player/GridLocalPlayer.h"
#include "UObject/Object.h"
#include "GridSettingsShared.generated.h"

class UGridLocalPlayer;

/**
 * UGridSettingsShared
 *
 * Shared Settings 被 SaveGame 管理，这种设置不同于 local Settings, 可以安全的被云端保存，并且能够共享他们
 * 使用 SaveGame 类我们可以为每个玩家保存一份设置
 * 所以像控制器的按键绑定可以放在此处，因为如果放在 local Settings 中那么每个用户都将能够获取他
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridSettingsShared : public USaveGame
{
	GENERATED_BODY()

public:
	DECLARE_EVENT_OneParam(UGridSettingsShared, FOnSettingChangedEvent, UGridSettingsShared* Settings);
	FOnSettingChangedEvent OnSettingChangedEvent;

public:
	UGridSettingsShared();

	void Initialize(UGridLocalPlayer* LocalPlayer);

	bool IsDirty() const { return bIsDirty; }
	void ClearDirtyFlag() { bIsDirty = false; }

	void SaveSettings();
	static UGridSettingsShared* LoadOrCreateSettings(const UGridLocalPlayer* LocalPlayer);

	void ApplySettings();

public:
	UFUNCTION()
	double GetMouseSensitivityX() const { return MouseSensitivityX; }
	void SetMouseSensitivityX(double NewValue) { ChangeValueAndDirty(MouseSensitivityX, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	double GetMouseSensitivityY() const { return MouseSensitivityY; }
	void SetMouseSensitivityY(double NewValue) { ChangeValueAndDirty(MouseSensitivityY, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	double GetTargetingMultiplier() const { return TargetingMultiplier; }
	UFUNCTION()
	void SetTargetingMultiplier(double NewValue) { ChangeValueAndDirty(TargetingMultiplier, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	bool GetInvertVerticalAxis() const { return bInvertVerticalAxis; }
	UFUNCTION()
	void SetInvertVerticalAxis(bool NewValue) { ChangeValueAndDirty(bInvertVerticalAxis, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	bool GetInvertHorizontalAxis() const { return bInvertHorizontalAxis; }
	UFUNCTION()
	void SetInvertHorizontalAxis(bool NewValue) { ChangeValueAndDirty(bInvertHorizontalAxis, NewValue); ApplyInputSensitivity(); }
	
public:
	void ApplyInputSensitivity();
	
private:
	// 保存鼠标水平方向灵敏度
	UPROPERTY()
	double MouseSensitivityX = 1.0;

	// 保存鼠标竖直方向灵敏度
	UPROPERTY()
	double MouseSensitivityY = 1.0;
	
	UPROPERTY()
	double TargetingMultiplier = 0.5;

	// 反转Y轴
	UPROPERTY()
	bool bInvertVerticalAxis = false;

	// 反转X轴
	UPROPERTY()
	bool bInvertHorizontalAxis = false;
private:
	template<typename T>
	bool ChangeValueAndDirty(T& CurrentValue, const T& NewValue)
	{
		if (CurrentValue != NewValue)
		{
			CurrentValue = NewValue;
			bIsDirty = true;
			OnSettingChangedEvent.Broadcast(this);

			return true;
		}
		return false;
	}

	bool bIsDirty = false;

	UPROPERTY(Transient)
	TObjectPtr<UGridLocalPlayer> OwningPlayer = nullptr;
};
