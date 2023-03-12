#include "GridMasterComponent.h"

#include "EnhancedInputSubsystems.h"
#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "GridPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Input/GridInputComponent.h"
#include "Input/GridMappableConfigPair.h"
#include "Player/GridLocalPlayer.h"
#include "Player/GridPlayerController.h"
#include "Player/GridPlayerState.h"
#include "PlayerMappableInputConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridMasterComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace GridHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
}

const FName UGridMasterComponent::NAME_BindInputsNow("BindInputsNow");
const FName UGridMasterComponent::NAME_ActorFeatureName("Master");

UGridMasterComponent::UGridMasterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReadyToBindInputs = false;
}

void UGridMasterComponent::OnRegister()
{
	Super::OnRegister();
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[UGridMasterComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("GridMasterComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("GridMasterComponent");

			FMessageLog(HeroMessageLogName).Error()
			                               ->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
			                               ->AddToken(FTextToken::Create(Message));

			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UGridMasterComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// pawn 生成完毕，准备进入下一步初始化流程
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// 等待 PlayerState 初始化完毕
		if (!GetPlayerState<AGridPlayerState>())
		{
			return false;
		}
		
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			AGridPlayerController* GridPC = GetController<AGridPlayerController>();

			// 需要等待 InputComponent 完成初始化
			if (!Pawn->InputComponent || !GridPC || !GridPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		AGridPlayerState* GridPS = GetPlayerState<AGridPlayerState>();

		return GridPS && Manager->HasFeatureReachedInitState(Pawn, UGridPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		// todo...
		return true;
	}
	return false;
}

void UGridMasterComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	if (CurrentState == FGridGameplayTags::Get().InitState_DataAvailable && DesiredState == FGridGameplayTags::Get().InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AGridPlayerState* GridPS = GetPlayerState<AGridPlayerState>();
		if (!ensure(Pawn && GridPS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UGridPawnData* PawnData = nullptr;

		if (UGridPawnExtensionComponent* PawnExtComp = UGridPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UGridPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(GridPS->GetGridAbilitySystemComponent(), GridPS);
		}

		if (AGridPlayerController* GridPC = GetController<AGridPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// todo...
	}
}

void UGridMasterComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UGridPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGridGameplayTags::Get().InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UGridMasterComponent::CheckDefaultInitialization()
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UGridMasterComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UGridPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UGridMasterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UGridMasterComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
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
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
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

void UGridMasterComponent::AddAdditionalInputConfig(const UGridInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	UGridInputComponent* GridIC = Pawn->FindComponentByClass<UGridInputComponent>();
	check(GridIC);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UGridPawnExtensionComponent* PawnExtComp = UGridPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		GridIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	}
}

void UGridMasterComponent::RemoveAdditionalInputConfig(const UGridInputConfig* InputConfig)
{
	
}

bool UGridMasterComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UGridMasterComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UGridPawnExtensionComponent* PawnExtComp = UGridPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UGridAbilitySystemComponent* GridASC = PawnExtComp->GetGridAbilitySystemComponent())
			{
				//GridASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void UGridMasterComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UGridPawnExtensionComponent* PawnExtComp = UGridPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UGridAbilitySystemComponent* GridASC = PawnExtComp->GetGridAbilitySystemComponent())
		{
			//GridASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UGridMasterComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (AGridPlayerController* GridController = Cast<AGridPlayerController>(Controller))
	{
		//GridController->SetIsAutoRunning(false);
	}
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UGridMasterComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UGridMasterComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * GridHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * GridHero::LookPitchRate * World->GetDeltaSeconds());
	}
}








