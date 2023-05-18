// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapNode.h"
#include "ChessPieces/GridChessPiece.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GridGameplayTags.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/AbilityEffects/GridGameplayEffect_Attach.h"

AGridMapNode::AGridMapNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGridAbilitySystemComponent>(
		this, TEXT("AbilitySystemComponent"));
}

void AGridMapNode::InitializeAbilitySystem()
{
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeGameplayTags();
}

void AGridMapNode::SetNodePosition(const FVector& NewPosition)
{
	SetActorLocation(NewPosition);
}

void AGridMapNode::OnChessPieceEnter(AGridChessPiece* InChessPiece) const
{
	const FGridGameplayTags GridGameplayTags = FGridGameplayTags::Get();
	FGameplayEventData EventData;
	EventData.Instigator = Owner;
	EventData.Target = InChessPiece;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, GridGameplayTags.GameplayEvent_OnChessEnterTile,
	                                                         FGameplayEventData());
	AttachActiveGameplayEffect(InChessPiece);
}

void AGridMapNode::OnChessPieceLeave(AGridChessPiece* InChessPiece) const
{
	const FGridGameplayTags GridGameplayTags = FGridGameplayTags::Get();
	FGameplayEventData EventData;
	EventData.Instigator = Owner;
	EventData.Target = InChessPiece;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, GridGameplayTags.GameplayEvent_OnChessLeaveTile,
															 FGameplayEventData());
}

void AGridMapNode::AttachActiveGameplayEffect(const AGridChessPiece* InChessPiece) const
{
	check(AbilitySystemComponent);

	// 定义我们要查询的标签
	FGameplayTagContainer Tags;
	Tags.AddTagFast(FGameplayTag::RequestGameplayTag(FName("Ability.Element"), true));

	// 创建查询
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(Tags);

	// 获取激活效果
	TArray<FActiveGameplayEffectHandle> ActiveEffects = AbilitySystemComponent->GetActiveEffects(Query);

	for (const auto& ActiveEffect : ActiveEffects)
	{
		// 获取激活效果
		if (const FActiveGameplayEffect* ActiveGE = AbilitySystemComponent->GetActiveGameplayEffect(ActiveEffect))
		{
			// 获取GameplayEffect类对象
		 	const UGameplayEffect* GameplayEffect = ActiveGE->Spec.Def;
			if (const UGridGameplayEffect_Attach* GridGameplayEffect_Attach = Cast<UGridGameplayEffect_Attach>(GameplayEffect))
			{
				for (const auto& AttachGameplayEffectClass : GridGameplayEffect_Attach->AttachGameplayEffects)
				{
					const UGameplayEffect* AttachGameplayEffect = AttachGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
					InChessPiece->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(AttachGameplayEffect,ActiveGE->Spec.GetLevel(), InChessPiece->GetAbilitySystemComponent()->MakeEffectContext());
				}
			}
		}
	}
}

void AGridMapNode::InitializeGameplayTags() const
{
	const FGridGameplayTags GridGameplayTags = FGridGameplayTags::Get();
	AbilitySystemComponent->AddLooseGameplayTag(GridGameplayTags.Ability_TargetType_MapNode);
}
