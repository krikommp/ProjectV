// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DEditModeWidget.h"
#include "SlateOptMacros.h"
#define LOCTEXT_NAMESPACE "STilemap3DEditStatusWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STilemap3DEditModeWidget::Construct(const FArguments& InArgs)
{
	EditMode = InArgs._EditMode;
	OnEditModeChanged = InArgs._OnEditModeChanged;
	BaseEditMode = InArgs._BaseEditMode;

	SAssignNew(CubePanelControl, SSegmentedControl<int32>)
	.Value(EditMode.Get()) // InitialValue
	.OnValueChanged_Lambda([this](int32 InValue)
	                                                      {
		                                                      EditMode = InValue;
		                                                      // ReSharper disable once CppExpressionWithoutSideEffects
		                                                      OnEditModeChanged.ExecuteIfBound(EditMode.Get());
	                                                      })
	.Value_Lambda([this] { return static_cast<int32>(EditMode.Get()); }) // Bound
		+ SSegmentedControl<int32>::Slot(EEM_Cube_Append)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("AppendCube", "Append Cube"))
		+ SSegmentedControl<int32>::Slot(EEM_Cube_Remove)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.DeleteComponentTool"))
		  .Text(LOCTEXT("RemoveCube", "Remove Cube"));

	SAssignNew(MeshPanelControl, SSegmentedControl<int32>)
	.Value(EditMode.Get()) // InitialValue
	.OnValueChanged_Lambda([this](int32 InValue)
	                                                      {
		                                                      EditMode = static_cast<ETilemap3DEditMode>(InValue);
		                                                      // ReSharper disable once CppExpressionWithoutSideEffects
		                                                      OnEditModeChanged.ExecuteIfBound(EditMode.Get());
	                                                      })
	.Value_Lambda([this] { return static_cast<int32>(EditMode.Get()); }) // Bound
		+ SSegmentedControl<int32>::Slot(EEM_Mesh_Select)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("SelectMesh", "Select Mesh"))
		+ SSegmentedControl<int32>::Slot(EEM_Mesh_Append)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("AppendMesh", "Append Mesh"))
		+ SSegmentedControl<int32>::Slot(EEM_Mesh_Remove)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.DeleteComponentTool"))
		  .Text(LOCTEXT("RemoveMesh", "Remove Mesh"));

	SAssignNew(ChessPanelControl, SSegmentedControl<int32>)
	.Value(EditMode.Get()) // InitialValue
    .OnValueChanged_Lambda([this](int32 InValue)
	                                                       {
		                                                       EditMode = static_cast<ETilemap3DEditMode>(InValue);
		                                                       // ReSharper disable once CppExpressionWithoutSideEffects
		                                                       OnEditModeChanged.ExecuteIfBound(EditMode.Get());
	                                                       })
    .Value_Lambda([this] { return static_cast<int32>(EditMode.Get()); }) // Bound
		+ SSegmentedControl<int32>::Slot(EEM_Chess_Select)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("SelectChess", "Select Chess"))
		+ SSegmentedControl<int32>::Slot(EEM_Chess_Spawn)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("SpawnChess", "Spawn Chess"))
		+ SSegmentedControl<int32>::Slot(EEM_Chess_Remove)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("RemoveChess", "Remove Chess"));

	SAssignNew(PlayerPanelControl, SSegmentedControl<int32>)
	.OnValueChanged_Lambda([this](int32 InValue)
	                                                        {
		                                                        EditMode = static_cast<ETilemap3DEditMode>(InValue);
		                                                        // ReSharper disable once CppExpressionWithoutSideEffects
		                                                        OnEditModeChanged.ExecuteIfBound(EditMode.Get());
	                                                        })
	.Value_Lambda([this] { return static_cast<int32>(EditMode.Get()); }) // Bound
		+ SSegmentedControl<int32>::Slot(EEM_Player_Start)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("PlayerStart", "Set PlayerStart"))
		+ SSegmentedControl<int32>::Slot(EEM_Player_Chess)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("PlayerChess", "Spawn PlayerChess"));

	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSegmentedControl<int32>)
			.Value(BaseEditMode.Get())
			.OnValueChanged_Lambda([this](int32 InValue)
			                              {
				                              BaseEditMode = static_cast<ETilemap3DEditMode>(InValue);
				                              EditMode = BaseEditMode.Get() == EEM_View
					                                         ? EEM_View
					                                         : BaseEditMode.Get() + 1;
				                              // ReSharper disable once CppExpressionWithoutSideEffects
				                              OnEditModeChanged.ExecuteIfBound(EditMode.Get());
			                              })
			.Value_Lambda([this] { return static_cast<int32>(BaseEditMode.Get()); })
			+ SSegmentedControl<int32>::Slot(ETilemap3DEditMode::EEM_View)
			  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.SelectComponentTool"))
			  .Text(LOCTEXT("ViewMode", "View Mode"))
			+ SSegmentedControl<int32>::Slot(ETilemap3DEditMode::EEM_Cube)
			  .Icon(FAppStyle::Get().GetBrush("ModelingToolsManagerCommands.BeginAddBoxPrimitiveTool"))
			  .Text(LOCTEXT("CubeMode", "Edit Cube"))
			+ SSegmentedControl<int32>::Slot(ETilemap3DEditMode::EEM_Mesh)
			  .Icon(FAppStyle::Get().GetBrush("ModelingToolsManagerCommands.BeginAddBoxPrimitiveTool"))
			  .Text(LOCTEXT("MeshMode", "Edit Mesh"))
			+ SSegmentedControl<int32>::Slot(ETilemap3DEditMode::EEM_Chess)
			  .Icon(FAppStyle::Get().GetBrush("ModelingToolsManagerCommands.BeginAddBoxPrimitiveTool"))
			  .Text(LOCTEXT("ChessMode", "Edit Chess"))
			+ SSegmentedControl<int32>::Slot(ETilemap3DEditMode::EEM_Player)
			  .Icon(FAppStyle::Get().GetBrush("ModelingToolsManagerCommands.BeginAddBoxPrimitiveTool"))
			  .Text(LOCTEXT("PlayerMode", "Edit Player"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return BaseEditMode.Get() == EEM_Cube ? EVisibility::Visible : EVisibility::Collapsed;
			             })
			[
				CubePanelControl.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return BaseEditMode.Get() == EEM_Mesh ? EVisibility::Visible : EVisibility::Collapsed;
			             })
			[
				MeshPanelControl.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return BaseEditMode.Get() == EEM_Chess ? EVisibility::Visible : EVisibility::Collapsed;
			             })
			[
				ChessPanelControl.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
						 {
							 return BaseEditMode.Get() == EEM_Player ? EVisibility::Visible : EVisibility::Collapsed;
						 })
			[
				PlayerPanelControl.ToSharedRef()
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
