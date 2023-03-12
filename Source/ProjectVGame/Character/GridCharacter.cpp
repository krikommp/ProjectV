// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCharacter.h"

#include "GridGameplayTags.h"
#include "GridPawnExtensionComponent.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

static FName NAME_GridCharacterCollisionProfile_Capsule(TEXT("GridPawnCapsule"));
static FName NAME_GridCharacterCollisionProfile_Mesh(TEXT("GridPawnMesh"));

AGridCharacter::AGridCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_GridCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_GridCharacterCollisionProfile_Mesh);

	PawnExtComponent = CreateDefaultSubobject<UGridPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(CapsuleComp);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void AGridCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGridCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGridCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGridCharacter::Reset()
{
	K2_OnReset();
}

void AGridCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGridCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

UGridAbilitySystemComponent* AGridCharacter::GetGridAbilitySystemComponent() const
{
	return Cast<UGridAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AGridCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetGridAbilitySystemComponent();
}

void AGridCharacter::OnAbilitySystemInitialized()
{
	UGridAbilitySystemComponent* GridASC = GetGridAbilitySystemComponent();
	check(GridASC);

	InitializeGameplayTags();
}

void AGridCharacter::OnAbilitySystemUninitialized()
{
}

void AGridCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PawnExtComponent->HandleControllerChanged();
}

void AGridCharacter::UnPossessed()
{
	AController* const OldController = Controller;
	Super::UnPossessed();
	PawnExtComponent->HandleControllerChanged();
}

void AGridCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void AGridCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UGridAbilitySystemComponent* GridASC = GetGridAbilitySystemComponent())
	{
		const FGridGameplayTags& GameplayTags = FGridGameplayTags::Get();

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				GridASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				GridASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}
	}
}
