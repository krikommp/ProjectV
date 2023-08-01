// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "Character/GridPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Input/GridInputComponent.h"
#include "Input/GridMappableConfigPair.h"
#include "Player/GridLocalPlayer.h"
#include "Player/GridPlayerController.h"
#include "Player/GridPlayerState.h"
#include "PlayerMappableInputConfig.h"
#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

const FName UMasterInputComponent::NAME_BindInputsNow("BindInputsNow");
const FName UMasterInputComponent::NAME_ActorFeatureName("MasterInput");

UMasterInputComponent::UMasterInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bReadyToBindInputs(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UMasterInputComponent::OnRegister()
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
		// 向GameFramework注册初始化状态
		RegisterInitStateFeature();
	}
}

void UMasterInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// 监听 PawnExtComp 的状态改变事件
	BindOnActorInitStateChanged(UGridPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// 当前我们已经完成了 Spawn，需要重置当前的初始化状态
	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UMasterInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}


void UMasterInputComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UMasterInputComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                               FGameplayTag DesiredState) const
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
	}else if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
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

		// 这里是判断 TilemapExtComp 的数据是否准备完毕
		// master 和 棋子对象上面都会挂载一个 TilemapExtComp
		return GridPS && Manager->HasFeatureReachedInitState(Pawn, UGridPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		// 当绑定完成也就说明该组件完成了初始化操作
		return bReadyToBindInputs;
	}
	return false;
}

void UMasterInputComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	// 当执行到第三步初始化，即该组件依赖的其他组件都完成了相应的数据初始化操作
	if (CurrentState == FGridGameplayTags::Get().InitState_DataAvailable && DesiredState == FGridGameplayTags::Get().InitState_DataInitialized)
	{
		// 那么就开始该组件自身的初始化流程，这里是执行按键绑定
		APawn* Pawn = GetPawn<APawn>();
		if (AGridPlayerController* PC = GetController<AGridPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void UMasterInputComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UGridPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGridGameplayTags::Get().InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UMasterInputComponent::CheckDefaultInitialization()
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}

void UMasterInputComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
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
				GridIC->BindNativeAction(InputConfig, GameplayTags.InputTag_TileBaseCameraMove, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
			}
		}
	}

	// 通知
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UMasterInputComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
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

void UMasterInputComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
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

void UMasterInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

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