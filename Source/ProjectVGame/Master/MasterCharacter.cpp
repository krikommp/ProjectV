// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Character/GridPawnExtensionComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MasterMovementComponent.h"

AMasterCharacter::AMasterCharacter(const FObjectInitializer& ObjectInitializer)
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	CapsuleComponent->InitCapsuleSize(50.0f, 80.0f);
	RootComponent = CapsuleComponent;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	ExtensionComponent = ObjectInitializer.CreateDefaultSubobject<UGridPawnExtensionComponent>(this, TEXT("ExtensionComponent"));
	MovementComponent = ObjectInitializer.CreateDefaultSubobject<UMasterMovementComponent>(this, TEXT("MovementComponent"));
	
	PrimaryActorTick.bCanEverTick = true;
}

void AMasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	ExtensionComponent->SetupPlayerInputComponent();
}

void AMasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ExtensionComponent->HandleControllerChanged();
}

void AMasterCharacter::UnPossessed()
{
	Super::UnPossessed();
	ExtensionComponent->HandleControllerChanged();
}

