#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayTagsManager.h"

struct FGridGameplayTags
{
public:
	static const FGridGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	static FGameplayTag FindTagByName(FString TagString, bool bMatchPartialString = false);

public:
	FGameplayTag Ability_ActivateFail_IsDead;
	FGameplayTag Ability_ActivateFail_Cost;
	FGameplayTag Ability_ActivateFail_Cooldown;
	FGameplayTag Ability_ActivateFail_TagsBlocked;
	FGameplayTag Ability_ActivateFail_TagsMissing;
	FGameplayTag Ability_ActivateFail_Networking;
	FGameplayTag Ability_ActivateFail_NotOwner;

	FGameplayTag Ability_Behavior_SurvivesDeath;
	FGameplayTag Ability_Behavior_Move;

	FGameplayTag Ability_Data_Cooldown;
	FGameplayTag Ability_Cooldown;

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_MouseConfirm;
	FGameplayTag InputTag_MouseCancel;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Look_Stick;
	FGameplayTag InputTag_TileBaseCameraMove;
	FGameplayTag InputTag_TileBaseMouseCameraMove;
	FGameplayTag InputTag_TileBaseCameraDistance;
	FGameplayTag InputTag_TileBaseCameraRotate;
	
	FGameplayTag GameplayEvent_BreakDefense;
	FGameplayTag GameplayEvent_Death;
	FGameplayTag GameplayEvent_Reset;
	FGameplayTag GameplayTag_Save;
	FGameplayTag GameplayEvent_PopAttackInput;
	FGameplayTag GameplayEvent_AbilityCardInput;
	FGameplayTag GameplayEvent_OnChessEnterTile;
	FGameplayTag GameplayEvent_OnChessLeaveTile;

	FGameplayTag Status_Death;
	FGameplayTag Status_Death_Dying;
	FGameplayTag Status_Death_Dead;

	// GameFrameworkComponentManager 初始化状态
	
	// Actor/Component 已经 spawned 
	FGameplayTag InitState_Spawned;

	// 所有需要的数据加载完毕，准备进行初始化
	FGameplayTag InitState_DataAvailable;

	// 所有需要的资源都初始化完毕, 但是还没有准备好进行 Gameplay 
	FGameplayTag InitState_DataInitialized;

	// Actor/Component 准备完毕
	FGameplayTag InitState_GameplayReady;

	// 水元素
	FGameplayTag Element_Water;
	// 冰元素
	FGameplayTag Element_Ice;
	// 火元素
	FGameplayTag Element_Fire;
	// 雷元素
	FGameplayTag Element_Thunder;
	// 油元素
	FGameplayTag Element_Oil;
	// 毒元素
	FGameplayTag Element_Toxic;
	
	TMap<uint8, FGameplayTag> MovementModeTagMap;
	TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

protected:
	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode);
	void AddCustomMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 CustomMovementMode);
	
private:
	static FGridGameplayTags GameplayTags;
};