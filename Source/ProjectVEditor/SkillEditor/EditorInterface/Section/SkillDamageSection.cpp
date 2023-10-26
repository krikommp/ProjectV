// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDamageSection.h"

#include "GameplayEffectExecutionCalculation.h"
#include "ISinglePropertyView.h"
#include "SlateOptMacros.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"
#include "Skill/SkillAsset.h"
#include "Skill/SkillEffect.h"
#include "UObject/SavePackage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillDamageSection"
void SSkillDamageSection::Construct(const FArguments& InArgs, TWeakObjectPtr<USkillAsset> InSkillAsset)
{
	using ThisType = TDecay<decltype(*this)>::Type;
	
	SkillAsset = InSkillAsset;
	CreateDamageSkillEffectAsset();
	InitializeSkillDamageEffect();
	
	DamageTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageTypeItemLabel", "None").ToString())));
	DamageTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageTypeItemLabel", "HP Damage").ToString())));
	DamageTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageTypeItemLabel", "TP Damage").ToString())));

	DamageAttributeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageAttributeItemLabel", "None").ToString())));
	DamageAttributeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageAttributeItemLabel", "Normal Attack").ToString())));

	DamageCriticalItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageCriticalItemLabel", "Yes").ToString())));
	DamageCriticalItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageCriticalItemLabel", "No").ToString())));
	
	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);
	TSharedRef<SHorizontalBox> SkillDamageHBox_1 = SNew(SHorizontalBox);
	
	TitleSlot(WidgetVerticalBox, LOCTEXT("SkillDamgeTitleLabel", "Damage"));
	NextHSlot(SkillDamageHBox_1, LOCTEXT("SkillDamageTypeLabel", "Type"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&DamageTypeItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		{
			return SNew(STextBlock).Text(FText::FromString(*Item));
		})
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		{
			if (InSelection.IsValid() && DamageTypeBlock.IsValid())
			{
				DamageTypeBlock->SetText(FText::FromString(*InSelection));
			}
		})
		[
			SAssignNew(DamageTypeBlock, STextBlock).Text(LOCTEXT("SkillDamageTypeItemLabel", "None"))
		]
	];
	NextHSlot(SkillDamageHBox_1, LOCTEXT("SkillDamageAttributeLabel", "Attribute"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&DamageAttributeItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		{
			return SNew(STextBlock).Text(FText::FromString(*Item));
		})
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		{
			if (InSelection.IsValid() && DamageAttributeBlock.IsValid())
			{
				DamageAttributeBlock->SetText(FText::FromString(*InSelection));
			}
		})
		[
			SAssignNew(DamageAttributeBlock, STextBlock).Text(LOCTEXT("SkillDamageAttributeItemLabel", "None"))
		]
	];
	NextVSlot(WidgetVerticalBox)
	[
		SkillDamageHBox_1
	];
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FSinglePropertyParams Params;
	Params.NamePlacement = EPropertyNamePlacement::Hidden;
			
	const TSharedPtr<ISinglePropertyView> SinglePropertyView =
		PropertyEditorModule.CreateSingleProperty(Cast<UObject>(SkillAsset), GET_MEMBER_NAME_CHECKED(USkillAsset, DamageExecution), Params
	);
	FSimpleDelegate OnDamageExecuteChanged = FSimpleDelegate::CreateSP(this, &ThisType::OnDamageExecuteChanged);
	SinglePropertyView->SetOnPropertyValueChanged(OnDamageExecuteChanged);
	NextVSlot(WidgetVerticalBox, LOCTEXT("SkillDamageFormula", "Damage Formula"))
	[
		SinglePropertyView.ToSharedRef()
	];
	NextVSlot(WidgetVerticalBox, LOCTEXT("SkillDamageCriticalLabel", "Critical"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&DamageCriticalItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		{
			return SNew(STextBlock).Text(FText::FromString(*Item));
		})
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		{
			if (InSelection.IsValid() && DamageCriticalBlock.IsValid())
			{
				DamageCriticalBlock->SetText(FText::FromString(*InSelection));
			}
		})
		[
			SAssignNew(DamageCriticalBlock, STextBlock).Text(LOCTEXT("SkillDamageCriticalItemLabel", "No"))
		]
	];

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.Header"), 6.0f))
		[
			WidgetVerticalBox.ToSharedRef()
		]
	];
}

void SSkillDamageSection::CreateDamageSkillEffectAsset() const
{
	if (!SkillAsset.IsValid())
		return;

	if (SkillAsset->DamageSkillEffect != nullptr)
		return;

	auto AssetName = SkillAsset->GetName();

	UBlueprint* NewBlueprint = FKismetEditorUtilities::CreateBlueprint(
		USkillEffect::StaticClass(),
		SkillAsset->GetOutermost(),
		TEXT(""),
		BPTYPE_Const,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass());

	FString PackageName = FString::Format(TEXT("/Game/Skills/Effects/{0}/GE_{1}_DamageEffect"), { AssetName, AssetName });
	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();
	NewBlueprint->Modify();
	NewBlueprint->Rename(*FString::Format(TEXT("GE_{0}_DamageEffect"), { AssetName }), Package);
	FAssetRegistryModule::AssetCreated(NewBlueprint);
	Package->SetDirtyFlag(false);
	FSavePackageArgs SavePackageArgs;
	SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	UPackage::SavePackage(Package, NewBlueprint, *PackageName, SavePackageArgs);

	SkillAsset->DamageSkillEffect = NewBlueprint->GeneratedClass;
}

void SSkillDamageSection::InitializeSkillDamageEffect() const
{
	if (!SkillAsset.IsValid())
		return;

	if (SkillAsset->DamageSkillEffect == nullptr)
		return;

	if (SkillAsset->DamageExecution == nullptr)
		return;

	USkillEffect* SkillEffect = SkillAsset->DamageSkillEffect->GetDefaultObject<USkillEffect>();
	SkillEffect->Executions.Empty();

	FGameplayEffectExecutionDefinition ExecutionDefinition;
	ExecutionDefinition.CalculationClass = SkillAsset->DamageExecution;
	SkillEffect->Executions.Add(ExecutionDefinition);
}

void SSkillDamageSection::OnDamageExecuteChanged()
{
	InitializeSkillDamageEffect();
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
