#include "GridGameplayTags.h"

#include "GameplayTagsManager.h"
#include "GridLogChannel.h"


FGridGameplayTags FGridGameplayTags::GameplayTags;


void FGridGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(Manager);

	// Notify manager that we are done adding native tags.
	Manager.DoneAddingNativeTags();
}

FGameplayTag FGridGameplayTags::FindTagByName(FString TagString, bool bMatchPartialString)
{
	const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

	if (!Tag.IsValid() && bMatchPartialString)
	{
		FGameplayTagContainer AllTags;
		Manager.RequestAllGameplayTags(AllTags, true);

		for (const FGameplayTag TestTag : AllTags)
		{
			if (TestTag.ToString().Contains(TagString))
			{
				UE_LOG(LogGrid, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
				Tag = TestTag;
				break;
			}
		}
	}

	return Tag;
}

void FGridGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "由于目标死亡，技能使用失败");
	AddTag(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "由于消耗不足，技能使用失败");
	AddTag(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "正在冷却，技能使用失败");
	AddTag(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "缺少所需 Tag, 技能使用失败");
	AddTag(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "没有通过网络检测，技能使用失败");
	AddTag(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "由于 Tag 被阻塞，技能使用失败");
	AddTag(Ability_ActivateFail_NotOwner, "Ability.ActivateFail.NotOwner", "不是技能的持有者，技能使用失敗");

	AddTag(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath", "该技能不随单位死亡而中止");
	AddTag(Ability_Behavior_Move, "Ability.Behavior.Move", "移动");
	
	AddTag(Ability_Data_Cooldown, "Ability.Data.Cooldown", "冷却数据读取");
	AddTag(Ability_Cooldown, "Ability.Cooldown", "冷却");

	AddTag(InputTag_Move, "InputTag.Move", "Move Input");
	AddTag(InputTag_MouseConfirm, "InputTag.Mouse.Confirm", "鼠标点击确认");
	AddTag(InputTag_MouseCancel, "InputTag.Mouse.Cancel", "鼠标点击取消");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
	AddTag(InputTag_Look_Stick, "InputTag.Look.Stick", "Look (stick) input.");
	AddTag(InputTag_TileBaseCameraMove, "InputTag.TileBase.CameraMove.KeyBoard", "战旗相机移动");
	AddTag(InputTag_TileBaseCameraDistance, "InputTag.TileBase.CameraDistance", "战旗相机远近调节");
	AddTag(InputTag_TileBaseMouseCameraMove, "InputTag.TileBase.CameraMove.Mouse", "战旗相机移动");
	AddTag(InputTag_TileBaseCameraRotate, "InputTag.TileBase.CameraRotate", "战旗相机旋转");
	
	AddTag(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	AddTag(GameplayEvent_BreakDefense, "GameplayEvent.BreakDefense", "单位被破防时触发事件");
	AddTag(GameplayEvent_Death, "GameplayEvent.Death", "单位死亡时触发事件");
	AddTag(GameplayEvent_Reset, "GameplayEvent.Reset", "单位重置时触发事件");
	AddTag(GameplayEvent_AbilityCardInput, "GameplayEvent.AbilityCardInput", "卡牌输入事件");
	AddTag(GameplayEvent_PopAttackInput, "GameplayEvent.PopAttackInput", "抛出攻击输入事件");
	AddTag(GameplayTag_Save, "GameplayEvent.Save", "保存游戏");
	AddTag(GameplayEvent_OnChessLeaveTile, "GameplayEvent.OnChessLeaveTile", "棋子离开格子时触发事件");
	AddTag(GameplayEvent_OnChessEnterTile, "GameplayEvent.OnChessEnterTile", "棋子进入格子时触发事件");

	AddTag(Status_Death, "Status.Death", "目标处于死亡状态");
	AddTag(Status_Death_Dying, "Status.Death.Dying", "目标开始死亡流程");
	AddTag(Status_Death_Dead, "Status.Death.Dead", "目标结束死亡流程");

	AddTag(Element_Water, "Element.Water", "水元素");
	AddTag(Element_Fire, "Element.Fire", "火元素");
	AddTag(Element_Ice, "Element.Ice", "冰元素");
	AddTag(Element_Oil, "Element.Oil", "油元素");
	AddTag(Element_Thunder, "Element.Thunder", "雷元素");
	AddTag(Element_Toxic, "Element.Toxic", "毒元素");
}

void FGridGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

void FGridGameplayTags::AddMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode)
{
	AddTag(OutTag, TagName, "Character movement mode tag.");
	GameplayTags.MovementModeTagMap.Add(MovementMode, OutTag);
}

void FGridGameplayTags::AddCustomMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName,
	uint8 CustomMovementMode)
{
	AddTag(OutTag, TagName, "Character custom movement mode tag.");
	GameplayTags.CustomMovementModeTagMap.Add(CustomMovementMode, OutTag);
}