// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapDrawRangeComponent.h"

#include "GridComponents.h"
#include "GridGameplayTags.h"
#include "TilemapExtensionComponent.h"
#include "Components/DecalComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

const FName UTilemapDrawRangeComponent::NAME_ActorFeatureName("TilemapDrawRange");

UTilemapDrawRangeComponent::UTilemapDrawRangeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;	
}

void UTilemapDrawRangeComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("UTilemapDrawRangeComponent on [%s] can only be added to Pawn actors."),
		*GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UTilemapDrawRangeComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(UTilemapExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), nullptr);

	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

bool UTilemapDrawRangeComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
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
	}
	else if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		return true;
	}
	else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// 这里是判断 PawnExtComp 的数据是否准备完毕
		// 注意 PawnExtComp 将会首先进入 InitState_DataInitialized 状态，然后该 Feature 再进
		// 默认可控制对象上面都会挂载一个 PawnExtComp
		return Manager->HasFeatureReachedInitState(Pawn, UTilemapExtensionComponent::NAME_ActorFeatureName,
															 InitTags.InitState_DataInitialized);
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	return false;
}

void UTilemapDrawRangeComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// 当执行到第三步初始化，即该组件依赖的其他组件都完成了相应的数据初始化操作
	if (CurrentState == FGridGameplayTags::Get().InitState_DataAvailable && DesiredState == FGridGameplayTags::Get().
		InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();

		DisplaySplineComponent = Cast<USplineComponent>(Pawn->AddComponentByClass(USplineComponent::StaticClass(), false, FTransform::Identity, false));
	}
}

void UTilemapDrawRangeComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UTilemapExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGridGameplayTags::Get().InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UTilemapDrawRangeComponent::CheckDefaultInitialization()
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = {
		InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}

void UTilemapDrawRangeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

TArray<FVector> UTilemapDrawRangeComponent::CreateSplinePath(const TArray<int32>& PathIndexArray) const
{
	const APawn* Pawn = GetPawnChecked<APawn>();
	
	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
		return {};
	
	TArray<FVector> PathVectorArray;
	for (const int32 PathIndex : PathIndexArray)
	{
		PathVectorArray.Add(TilemapExtensionComponent->GetPathfindingLocation(PathIndex, 0.1f));
	}

	return PathVectorArray;
}

void UTilemapDrawRangeComponent::DisplayPathfindingDecal(TArray<int32> Indexes)
{
	const APawn* Pawn = GetPawnChecked<APawn>();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
		return;

	for (const int32 Index : Indexes)
	{
		const FVector DecalLocation = TilemapExtensionComponent->GetTilemap()->GetPathfindingBlockLocation(Index);
		UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), TilemapExtensionComponent->GetTilemap()->GetTilemap()->MoveRangeMat, DecalSize, DecalLocation, {90.0f, 0.0f, 0.0f});

		DecalComponents.Add(DecalComponent);
	}
}

void UTilemapDrawRangeComponent::ClearAllDecalComponents()
{
	for (const auto& DecalComp : DecalComponents)
	{
		DecalComp->DestroyComponent();
	}
	DecalComponents.Empty();
}

void UTilemapDrawRangeComponent::DisplayPathfindingSplinePath(const TArray<int32>& PathIndexArray)
{
	APawn* Pawn = GetPawnChecked<APawn>();
	
	if (DisplaySplineComponent == nullptr)
		return;

 	const TArray<FVector> PathVectors = CreateSplinePath(PathIndexArray);
	DisplaySplineComponent->SetSplinePoints(PathVectors, ESplineCoordinateSpace::World, true);
	for (int32 Index = 0; Index < PathVectors.Num(); ++Index)
	{
		DisplaySplineComponent->SetSplinePointType(Index, ESplinePointType::Linear, true);
	}
	double Remainder = 0.0f;
	constexpr float SplineMeshLength = 100.0f;
	const int32 EndPoint = UKismetMathLibrary::FMod64(DisplaySplineComponent->GetSplineLength(), SplineMeshLength, Remainder);
	for (int32 Index = 0; Index < EndPoint; ++Index)
	{
		FTransform SplineTransform;
		auto SplineMesh = Cast<USplineMeshComponent>(
			Pawn->AddComponentByClass(USplineMeshComponent::StaticClass(), true, SplineTransform, false));
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetStaticMesh(SplinePathMesh);
		SplineMesh->SetMaterial(0, PathMaterial);
		SplineMesh->SetStartAndEnd(
			DisplaySplineComponent->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World),
			DisplaySplineComponent->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World) *
			SplineMeshLength,
			DisplaySplineComponent->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength,
															ESplineCoordinateSpace::World),
			DisplaySplineComponent->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength,
															 ESplineCoordinateSpace::World) * SplineMeshLength,
			true
		);
		SplineMeshArray.Add(SplineMesh);
	}
}

