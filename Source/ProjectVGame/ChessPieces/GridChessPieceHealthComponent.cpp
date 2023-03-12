#include "GridChessPieceHealthComponent.h"

#include "GameplayEffectExtension.h"
#include "GridGameplayTags.h"
#include "NativeGameplayTags.h"
#include "GridLogChannel.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GridHealthSet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/GridVerbMessage.h"
#include "System/GridAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridChessPieceHealthComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Grid_Elimination_Message, "Grid.Elimination.Message");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Grid_BreakDefense_Message, "Grid.BreakDefense.Message");

UGridChessPieceHealthComponent::UGridChessPieceHealthComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EGridDeathState::NotDead;
}

void UGridChessPieceHealthComponent::OnUnregister()
{
	UnInitializeFromAbilitySystem();
	
	Super::OnUnregister();
}

void UGridChessPieceHealthComponent::InitializeWithAbilitySystem(UGridAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogGrid, Error,
			   TEXT(
				   "GridHealthComponent: Health component for owner [%s] has already been initialized with an ability system."
			   ), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogGrid, Error,
			   TEXT("GridHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."),
			   *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UGridHealthSet>();
	if (!HealthSet)
	{
		UE_LOG(LogGrid, Error,
			   TEXT(
				   "GridHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."
			   ), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGridHealthSet::GetHealthAttribute()).AddUObject(
	this, &UGridChessPieceHealthComponent::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGridHealthSet::GetMaxHealthAttribute()).AddUObject(
		this, &UGridChessPieceHealthComponent::HandleMaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGridHealthSet::GetDefenseAttribute()).AddUObject(
		this, &UGridChessPieceHealthComponent::HandleDefenseChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
	HealthSet->OnOutOfDefense.AddUObject(this, &ThisClass::HandleOutOfDefense);

	AbilitySystemComponent->SetNumericAttributeBase(UGridHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());

	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth(), nullptr);
	OnDefenseChanged.Broadcast(this, HealthSet->GetDefense(), HealthSet->GetDefense(), nullptr);
}

void UGridChessPieceHealthComponent::UnInitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet)
	{
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void UGridChessPieceHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		const FGridGameplayTags& GameplayTags = FGridGameplayTags::Get();

		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dead, 0);
	}
}

float UGridChessPieceHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UGridChessPieceHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UGridChessPieceHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

int32 UGridChessPieceHealthComponent::GetDefense() const
{
	return FMath::Floor( (HealthSet ? HealthSet->GetDefense() : 0.0f) );
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

void UGridChessPieceHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UGridChessPieceHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UGridChessPieceHealthComponent::HandleDefenseChanged(const FOnAttributeChangeData& ChangeData)
{
	OnDefenseChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UGridChessPieceHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	if (AbilitySystemComponent)
	{
		// 向自己的 Ability System 发送 “GameplayEvent.Death" 事件，用于触发死亡 Ability
		{
			FGameplayEventData Payload;
			Payload.EventTag = FGridGameplayTags::Get().GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// 向其他子系统发送消息
		{
			FGridVerbMessage Message;
			Message.Verb = TAG_Grid_Elimination_Message;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = AbilitySystemComponent->GetAvatarActor();
			Message.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}
	}
}

void UGridChessPieceHealthComponent::HandleOutOfDefense(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	if (AbilitySystemComponent)
	{
		// 向自己的 Ability System 发送 “GameplayEvent.BreakDefense" 事件
		{
			FGameplayEventData Payload;
			Payload.EventTag = FGridGameplayTags::Get().GameplayEvent_BreakDefense;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// 向其他子系统发送消息
		{
			FGridVerbMessage Message;
			Message.Verb = TAG_Grid_BreakDefense_Message;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = AbilitySystemComponent->GetAvatarActor();
			Message.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}
	}
}

void UGridChessPieceHealthComponent::StartDeath()
{
	if (DeathState != EGridDeathState::NotDead)
	{
		return;
	}

	DeathState = EGridDeathState::DeathStarted;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FGridGameplayTags::Get().Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);
}

void UGridChessPieceHealthComponent::FinishDeath()
{
	if (DeathState != EGridDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EGridDeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FGridGameplayTags::Get().Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);
}

void UGridChessPieceHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
}

