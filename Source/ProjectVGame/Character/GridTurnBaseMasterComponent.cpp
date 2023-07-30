// Fill out your copyright notice in the Description page of Project Settings.

#include "GridTurnBaseMasterComponent.h"

#include "EnhancedInputSubsystems.h"
#include "GridCharacter.h"
#include "GridGameplayTags.h"
#include "GridGlobalDelegates.h"
#include "GridMapManager/GridMapStateComponent.h"
#include "GameModes/GridGameState.h"
#include "GridLogChannel.h"
#include "GridPawnData.h"
#include "GridPawnExtensionComponent.h"
#include "GridTraceChannel.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Input/GridInputComponent.h"
#include "Input/GridMappableConfigPair.h"
#include "Player/GridLocalPlayer.h"
#include "PlayerMappableInputConfig.h"
#include "Camera/CameraComponent.h"
#include "ChessPieces/GridChessPiece.h"
#include "ChessPieces/GridChessPieceExtensionComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GridMapManager/GridMapManager.h"
#include "GridTurnManager/GridTurnManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/GridPlayerController.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_E

UGridTurnBaseMasterComponent::UGridTurnBaseMasterComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bEdgeScrolling = false;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UGridTurnBaseMasterComponent::OnRegister()
{
	Super::OnRegister();

	UWorld* World = GetWorld();
	check(World);

	FGridGlobalDelegates::OnActiveInputModeChanged.AddUObject(this, &ThisClass::HandleActiveInputModeChanged);
	FGridGlobalDelegates::OnChessPieceSelectChanged.AddUObject(this, &ThisClass::HandleChessPieceSelectChanged);

	bCanPan = true;
	bEdgeScrolling = true;
}

void UGridTurnBaseMasterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FollowCameraTarget();
	CameraControllerMove();
}

void UGridTurnBaseMasterComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UGridLocalPlayer* LP = Cast<UGridLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UGridPawnExtensionComponent* PawnExtComp = UGridPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UGridPawnData* PawnData = PawnExtComp->GetPawnData<UGridPawnData>())
		{
			if (const UGridInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FGridGameplayTags& GameplayTags = FGridGameplayTags::Get();

				// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				for (const FMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically && Pair.CanBeActivated())
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;
						// Actually add the config to the local player							
						Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);	
					}
				}

				UGridInputComponent* GridIC = CastChecked<UGridInputComponent>(PlayerInputComponent);
				GridIC->AddInputMappings(InputConfig, Subsystem);

				TArray<uint32> BindHandles;
				GridIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_TileBaseCameraDistance, ETriggerEvent::Triggered, this, &ThisClass::Input_MouseWheel, /*bLogIfNotFound=*/ false);
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_TileBaseCameraMove, ETriggerEvent::Triggered, this, &ThisClass::Input_CameraMove, /*bLogIfNotFound=*/ false);
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_TileBaseCameraRotate, ETriggerEvent::Triggered, this, &ThisClass::Input_CameraRotate, /*bLogIfNotFound=*/ false);
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_MouseConfirm, ETriggerEvent::Started, this, &ThisClass::Input_MouseConfirm, /*bLogIfNotFound=*/ false);
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_MouseCancel, ETriggerEvent::Started, this, &ThisClass::Input_MouseCancel, /*bLogIfNotFound=*/ false);
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UGridTurnBaseMasterComponent::Input_CameraMove(const FInputActionValue& InputActionValue)
{
	AGridCharacter* Character = GetPawn<AGridCharacter>();
	check(Character);

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	CameraMove = Value;
}

void UGridTurnBaseMasterComponent::Input_CameraRotate(const FInputActionValue& InputActionValue)
{
	if (!bCanRotate) return;

	const float Value = InputActionValue.Get<float>();

	if (Value != 0.0f)
	{
		const AGridCharacter* Character = GetPawn<AGridCharacter>();
		check(Character);

		USpringArmComponent* SpringArmComponent = Character->GetSpringArmComponent();
		const FRotator Rotator = SpringArmComponent->GetComponentRotation();
		SpringArmComponent->SetWorldRotation(FRotator(Rotator.Pitch, Rotator.Yaw + (Value * CameraRotateSpeed), Rotator.Roll));
	}
}

void UGridTurnBaseMasterComponent::Input_MouseWheel(const FInputActionValue& InputActionValue)
{
	if (!bCanZoom) return;

	AGridCharacter* Character = GetPawn<AGridCharacter>();
	check(Character);

	USpringArmComponent* SpringArmComponent = Character->GetSpringArmComponent();

	if (InputActionValue.Get<float>() > 0.0f)
	{
		if (SpringArmComponent->TargetArmLength < ZoomRoof)
		{
			Character->MouseWheelDown(SpringArmComponent, SpringArmComponent->TargetArmLength, CameraZoomSpeed, ZoomRoof);
		}
	}

	if (InputActionValue.Get<float>() < 0.0f)
	{
		if (SpringArmComponent->TargetArmLength > ZoomFloor)
		{
			Character->MouseWheelUp(SpringArmComponent, SpringArmComponent->TargetArmLength, CameraZoomSpeed, ZoomFloor);
		}
	}
}

void UGridTurnBaseMasterComponent::Input_MouseConfirm(const FInputActionValue& InputActionValue)
{
	AGridCharacter* Owner = GetPawn<AGridCharacter>();
	check(Owner);
	
	// 发送一个鼠标点击事件
	FGridGameplayTags GameplayTags = FGridGameplayTags::Get();
	
	FGridVerbMessage Message;
	Message.Verb = GameplayTags.InputTag_MouseConfirm;
	Message.Instigator = Owner;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(Message.Verb, Message);

	OnMouseButtonDown.Broadcast(GameplayTags.InputTag_MouseConfirm);
	OnMouseConfirmButtonDown.Broadcast();
}

void UGridTurnBaseMasterComponent::Input_MouseCancel(const FInputActionValue& InputActionValue)
{
	AGridCharacter* Owner = GetPawn<AGridCharacter>();
	check(Owner);
	
	// 发送一个鼠标点击事件
	FGridGameplayTags GameplayTags = FGridGameplayTags::Get();
	
	FGridVerbMessage Message;
	Message.Verb = GameplayTags.InputTag_MouseCancel;
	Message.Instigator = Owner;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(Message.Verb, Message);

	OnMouseButtonDown.Broadcast(GameplayTags.InputTag_MouseCancel);
	OnMouseCancelButtonDown.Broadcast();
}

void UGridTurnBaseMasterComponent::HandleActiveInputModeChanged(ECommonInputMode InputMode)
{
	SetEdgeScrolling(InputMode != ECommonInputMode::Menu);
}

void UGridTurnBaseMasterComponent::HandleChessPieceSelectChanged(AGridChessPiece* OldChessPiece,
	AGridChessPiece* NewChessPiece)
{
	if (OldChessPiece != NewChessPiece) SetIsPanning(false);
}

void UGridTurnBaseMasterComponent::EdgeScrollingTick(float& UIInputRightLeft, float& UIInputUpDown) const
{
	if (!bEdgeScrolling) return;

	const AGridCharacter* Character = GetPawn<AGridCharacter>();
	check(Character);
	const AGridPlayerController* Controller = Cast<AGridPlayerController>(Character->GetController());
	check(Controller);

	int32 ViewPortSizeX, ViewPortSizeY;
	Controller->GetViewportSize(ViewPortSizeX, ViewPortSizeY);

	float MouseLocationX, MouseLocationY;
	Controller->GetMousePosition(MouseLocationX, MouseLocationY);

	if ( FMath::Abs(MouseLocationX - ViewPortSizeX * 0.5f) > ViewPortSizeX * (0.5f - EdgeScrollingEdgeSize) )
	{
		// 此时 Mouse 位于屏幕边缘，需要移动相机
		const float HalfViewPortSizeX = ViewPortSizeX * 0.5f;
		const float EdgeViewPortSizeX = (0.5f - EdgeScrollingEdgeSize) * ViewPortSizeX;
		const float MouseEdgeDistanceX = MouseLocationX - HalfViewPortSizeX;
		UIInputRightLeft = (CameraMoveSpeed / 20.0f) * ( ((FMath::Abs(MouseEdgeDistanceX) - EdgeViewPortSizeX) / (HalfViewPortSizeX - EdgeViewPortSizeX)) * UKismetMathLibrary::SignOfFloat(MouseEdgeDistanceX) );
	}else
	{
		// 此时 Mouse 处于屏幕中间位置，不需要移动相机
		UIInputRightLeft = 0.0f;
	}
	
	if ( FMath::Abs(ViewPortSizeY * 0.5f - MouseLocationY) > ViewPortSizeY * (0.5f - EdgeScrollingEdgeSize) ) 
	{
		const float HalfViewPortSizeY = ViewPortSizeY * 0.5f;
		const float EdgeViewPortSizeY = ( 0.5f - EdgeScrollingEdgeSize ) * ViewPortSizeY;
		const float MouseEdgeDistanceY = HalfViewPortSizeY - MouseLocationY;
		UIInputUpDown = (CameraMoveSpeed / 20.0f) * ( (FMath::Abs(MouseEdgeDistanceY) - EdgeViewPortSizeY ) / ( HalfViewPortSizeY - EdgeViewPortSizeY ) * UKismetMathLibrary::SignOfFloat(MouseEdgeDistanceY) );
	}else
	{
		UIInputUpDown = 0.0f;
	}
}

void UGridTurnBaseMasterComponent::FollowCameraTarget() const
{
	if (bIsPanning) return;

	const UGridTurnManagerComponent* TurnManagerComponent = GetWorld()->GetGameState()->FindComponentByClass<UGridTurnManagerComponent>();
	if (TurnManagerComponent == nullptr) return;;

	if (TurnManagerComponent->GetCurrentUnit() == nullptr) return;

	const FVector TargetLocation = bFollowCustomTarget ? CustomTarget : TurnManagerComponent->GetCurrentUnit()->GetActorLocation();

	AGridCharacter* Character = GetPawn<AGridCharacter>();
	check(Character);

	const FVector ActorLocation = Character->GetActorLocation();
	Character->SetActorLocation({ UKismetMathLibrary::Lerp(ActorLocation.X, TargetLocation.X, 0.1f), UKismetMathLibrary::Lerp(ActorLocation.Y, TargetLocation.Y, 0.1f), UKismetMathLibrary::Lerp(ActorLocation.Z, TargetLocation.Z, 0.1f) });
}

void UGridTurnBaseMasterComponent::CameraControllerMove()
{
	AGridCharacter* Character = GetPawn<AGridCharacter>();
	check(Character);

	if (!bCanPan) return;
	float UIInputRightLeft = 0.0f, UIInputUpDown = 0.0f;
	EdgeScrollingTick(UIInputRightLeft, UIInputUpDown);
	if (CameraMove.Y != 0.0f)
	{
		UIInputUpDown += CameraMove.Y;
		CameraMove.Y = 0.0f;
	}
	if (CameraMove.X != 0.0f)
	{
		UIInputRightLeft += CameraMove.X;
		CameraMove.X = 0.0f;
	}
	if (UIInputUpDown != 0.0f)
	{
		const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::MakeRotator(0.0f, 0.0f, Character->GetCameraComponent()->GetComponentRotation().Yaw));
		FVector NewLocation = ForwardVector * ( CameraMoveSpeed * (Character->GetSpringArmComponent()->TargetArmLength / 2000.0f) * UIInputUpDown );
		NewLocation += Character->GetActorLocation();
		//NewLocation = CheckOuterBounds(NewLocation);
		Character->SetActorLocation(NewLocation);
		bIsPanning = true;
	}
	if (UIInputRightLeft != 0.0f)
	{
		const FVector RightVector = UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotator(0.0f, 0.0f, Character->GetCameraComponent()->GetComponentRotation().Yaw));
		FVector NewLocation = RightVector * ( CameraMoveSpeed * (Character->GetSpringArmComponent()->TargetArmLength / 2000.0f) * UIInputRightLeft );
		NewLocation += Character->GetActorLocation();
		//NewLocation = CheckOuterBounds(NewLocation);
		Character->SetActorLocation(NewLocation);
		bIsPanning = true;
	}
}

// void UGridTurnBaseMasterComponent::OnGridMapInitialized(AGridMapManager* InGridMapManager)
// {
// 	check(InGridMapManager);
//
// 	if (GridMapManager != InGridMapManager)
// 	{
// 		GridMapManager = InGridMapManager;
// 	}
//
// 	bEdgeScrolling = true;
// 	bCanPan = true;
// }
//
// FVector UGridTurnBaseMasterComponent::CheckOuterBounds(const FVector& InLocation) const
// {
// 	check(GridMapManager);
// 	
// 	FVector NewLocation;
//
// 	const float XValue = GridMapManager->GridSizeX * GridMapManager->TileBoundsX;
// 	const float YValue = GridMapManager->GridSizeY * GridMapManager->TileBoundsY;
//
// 	NewLocation.X = FMath::Max(InLocation.X < XValue ? InLocation.X : XValue, 0.0f);
// 	NewLocation.Y = FMath::Max(InLocation.Y < YValue ? InLocation.Y : YValue, 0.0f);
// 	NewLocation.Z = InLocation.Z;
//
// 	return NewLocation;
// }
