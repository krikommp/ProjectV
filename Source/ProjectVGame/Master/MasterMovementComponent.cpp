// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterMovementComponent.h"

#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "GridTraceChannel.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Kismet/GameplayStatics.h"
#include "Tilemap/TilemapExtensionComponent.h"
#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

const FName UMasterMovementComponent::NAME_ActorFeatureName("MasterMove");

UMasterMovementComponent::UMasterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bNeedJump(false), bPanning(true)
{
	PrimaryComponentTick.bCanEverTick = true;
}


bool UMasterMovementComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
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
		return true;
	}else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// 这里是判断 TilemapExtComp 的数据是否准备完毕
		// master 和 棋子对象上面都会挂载一个 TilemapExtComp
		return Manager->HasFeatureReachedInitState(Pawn, UTilemapExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	return false;
}

void UMasterMovementComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	// 当执行到第三步初始化，即该组件依赖的其他组件都完成了相应的数据初始化操作
	if (CurrentState == FGridGameplayTags::Get().InitState_DataAvailable && DesiredState == FGridGameplayTags::Get().InitState_DataInitialized)
	{
		bPanning = false;
		const APawn* Pawn = GetPawn<APawn>();
		if (const UTilemapExtensionComponent* TilemapExtensionComponent = UTilemapExtensionComponent::FindTilemapExtensionComponent(Pawn))
		{
			Tilemap3DActorRef = TilemapExtensionComponent->GetTilemap();
		}
	}
}

void UMasterMovementComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UTilemapExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGridGameplayTags::Get().InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UMasterMovementComponent::CheckDefaultInitialization()
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}

void UMasterMovementComponent::OnRegister()
{
	Super::OnRegister();
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogGrid, Error,
			   TEXT(
				   "[UMasterMovementComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
			   ));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("UMasterMovementComponent", "NotOnPawnError",
												   "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("MasterMovementComponent");

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

void UMasterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// 监听 TilemapExtComp 的状态改变事件
	BindOnActorInitStateChanged(UTilemapExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), nullptr);

	// 当前我们已经完成了 Spawn，需要重置当前的初始化状态
	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UMasterMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UMasterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Falling(DeltaTime);
	bNeedJump = CheckNeedJump();
	Moving(DeltaTime);
	FinishMovement();
}

void UMasterMovementComponent::Falling(float DeltaTime) const
{
	if (CheckUnderGround())
		return;

	const FVector ActorLocation = GetPawn<APawn>()->GetActorLocation();
	const FVector NewLocation = ActorLocation - FVector(0, 0, FallingSpeed * DeltaTime);
	GetPawn<APawn>()->SetActorLocation(NewLocation);
}

bool UMasterMovementComponent::CheckUnderGround() const
{
	const FVector ActorLocation = GetPawn<APawn>()->GetActorLocation();

	const FVector RayStart = ActorLocation;
	const FVector RayEnd = ActorLocation - FVector::UpVector * GroundCheckLength;

	FHitResult HitResult;
	TArray<AActor*> IgnoreActors;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), RayStart, RayEnd, UEngineTypes::ConvertToTraceType(PathTrace), false, IgnoreActors, EDrawDebugTrace::None, HitResult, false);

	return HitResult.bBlockingHit;
}

void UMasterMovementComponent::Moving(float DeltaTime) const
{
	APawn* Pawn = GetPawn<APawn>();

	const FVector InputVector = Pawn->GetPendingMovementInputVector();
	if (InputVector == FVector::Zero())
		return;

	const FVector CurrentLocation = GetOwner()->GetActorLocation();
	FVector NewLocation = CurrentLocation + InputVector * MoveSpeed * DeltaTime + (bNeedJump ? FVector::UpVector * 100.0f : FVector::Zero());
	Tilemap3DActorRef->CheckLocationOutBound(NewLocation);
	Pawn->SetActorLocation(NewLocation);
}

bool UMasterMovementComponent::CheckNeedJump() const
{
	APawn* Pawn = GetPawn<APawn>();

	FVector InputVector = Pawn->GetPendingMovementInputVector();
	if (InputVector == FVector::Zero())
		return false;
	InputVector.Normalize();

	const FVector ActorLocation = Pawn->GetActorLocation();

	const FVector RayStart = ActorLocation;
	const FVector RayEnd = ActorLocation - InputVector * MoveCheckLength;

	FHitResult HitResult;
	TArray<AActor*> IgnoreActors;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), RayStart, RayEnd, UEngineTypes::ConvertToTraceType(PathTrace), false, IgnoreActors, EDrawDebugTrace::None, HitResult, false);

	return HitResult.bBlockingHit;
}

void UMasterMovementComponent::FinishMovement()
{
	APawn* Pawn = GetPawn<APawn>();
	Pawn->ConsumeMovementInputVector();
	bNeedJump = false;
}
