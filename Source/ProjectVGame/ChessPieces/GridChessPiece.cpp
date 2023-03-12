// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPiece.h"

#include "GridChessPieceExtensionComponent.h"
#include "GridChessPieceHealthComponent.h"
#include "GridLogChannel.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GridActionSet.h"
#include "AbilitySystem/Attributes/GridCombatSet.h"
#include "AbilitySystem/Attributes/GridHealthSet.h"
#include "AbilitySystem/Attributes/GridMoveSet.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Heros/GridHeroInfo.h"
#include "SMStateMachineComponent.h"

AGridChessPiece::AGridChessPiece(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(50.0f, 80.0f);
	RootComponent = CapsuleComponent;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));
	SkeletalMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGridAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	ChessPieceExtComponent = CreateDefaultSubobject<UGridChessPieceExtensionComponent>(TEXT("ChessPieceExtensionComponent"));
	ChessPieceExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	ChessPieceExtComponent->OnAbilitySystemUninitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
	
	ChessPieceComponent = CreateDefaultSubobject<UGridChessPieceComponent>(TEXT("ChessPieceComponent"));

	ChessPieceHealthComponent = CreateDefaultSubobject<UGridChessPieceHealthComponent>(TEXT("ChessPieceHealthComponent"));
	ChessPieceHealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	ChessPieceHealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	ChessPieceExtComponent->OnChessPieceInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnChessPieceInfoInitialized));

	ActionSet = CreateDefaultSubobject<UGridActionSet>(TEXT("ActionSet"));
	HealthSet = CreateDefaultSubobject<UGridHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UGridCombatSet>(TEXT("CombatSet"));
	MoveSet = CreateDefaultSubobject<UGridMoveSet>(TEXT("MoveSet"));
	
	PrimaryActorTick.bCanEverTick = true;
}

void AGridChessPiece::SpawnDefaultController()
{
	return;
}

UAbilitySystemComponent* AGridChessPiece::GetAbilitySystemComponent() const
{
	return GetGridAbilitySystemComponent();
}

void AGridChessPiece::SetupSkeletalMeshAsset(USkeletalMesh* SkeletalMesh) const
{
	SkeletalMeshComponent->SetSkeletalMeshAsset(SkeletalMesh);
}

void AGridChessPiece::OnTurnStartInternal()
{
	if (OnTurnStartDelegate().IsBound())
	{
		OnTurnStartDelegate().Broadcast();
	}
	OnTurnStart();
}

void AGridChessPiece::OnTurnOverInternal()
{
	if (OnTurnFinishDelegate().IsBound())
	{
		OnTurnFinishDelegate().Broadcast();
	}
	OnTurnOver();
}

void AGridChessPiece::OnChessPieceSelect()
{
	K2_OnChessPieceSelect();
}

void AGridChessPiece::OnChessPieceUnSelect()
{
	K2_OnChessPieceUnselect();
}

void AGridChessPiece::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void AGridChessPiece::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AGridChessPiece::OnCursorHoverBegin()
{
	K2_OnCursorHoverBegin();
}

void AGridChessPiece::OnCursorHoverEnd()
{
	K2_OnCursorHoverEnd();
}

void AGridChessPiece::BeginPlay()
{
	Super::BeginPlay();

	ChessPieceExtComponent->CheckDefaultInitialization();
}

bool AGridChessPiece::IsLocallyControlled() const
{
	return true;
}

void AGridChessPiece::OnAbilitySystemInitialized()
{
	UGridAbilitySystemComponent* GridASC = GetGridAbilitySystemComponent();
	check(GridASC);

	ChessPieceHealthComponent->InitializeWithAbilitySystem(GridASC);
	GridASC->OnAbilityBuffActive.AddDynamic(this, &ThisClass::K2_OnGameplayEffectActivate);

	InitializeGameplayTags();
}

void AGridChessPiece::OnAbilitySystemUninitialized()
{
}

void AGridChessPiece::OnChessPieceInfoInitialized()
{
	const auto& ChessPieceInfo = ChessPieceExtComponent->GetChessPieceInfo();
	SkeletalMeshComponent->SetSkeletalMeshAsset(ChessPieceInfo->HeroData.SkeletalMesh);
	SkeletalMeshComponent->SetAnimInstanceClass(ChessPieceInfo->HeroData.AnimBlueprint);
}

void AGridChessPiece::InitializeGameplayTags()
{
}

void AGridChessPiece::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	check(CapsuleComponent);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AGridChessPiece::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void AGridChessPiece::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	if (UGridAbilitySystemComponent* LyraASC = GetGridAbilitySystemComponent())
	{
		if (LyraASC->GetAvatarActor() == this)
		{
			ChessPieceExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}
