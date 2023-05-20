// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuManager.h"

#include "LevelEditor.h"
#include "MenuItem/MenuItem.h"

/**
 * @brief 查询菜单节点
 * @param InMenuNodes 菜单查询数组
 * @param InNodeName 菜单节点名称
 * @return 返回菜单节点指针，如果没有找到则返回nullptr
 */
static FMenuItemNode* FindMenuItemNode(TArray<FMenuItemNode>& InMenuNodes, const FString& InNodeName)
{
	for (auto& Node : InMenuNodes)
	{
		if (Node.NodeName == InNodeName)
		{
			return &Node;
		}
	}
	return nullptr;
}

void UMenuManager::Initialize(FSubsystemCollectionBase& Collection)
{
	// 遍历所有 class, 查询到所有继承自 UMenuItem 的类
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* CurrentClass = *It;
		if (CurrentClass->IsChildOf(UMenuItem::StaticClass()) && !(CurrentClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			AddMenuItemToNodeList(Cast<UMenuItem>(CurrentClass->GetDefaultObject()));
		}
	}

	// 菜单栏扩展
	// 顺序是：1. 找到需要挂载的MenuBar 2. 添加 PullDownMenu 3. 添加 SubMenu 4. 添加 MenuEntry
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuBarExtension("Help",
		                                  EExtensionHook::After,
		                                  nullptr,
		                                  FMenuBarExtensionDelegate::CreateUObject(
			                                  this, &UMenuManager::AddMenuBarExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
}

void UMenuManager::Deinitialize()
{
	Super::Deinitialize();
}

static void AddMenuExtension(FMenuBuilder& Builder, TArray<FMenuItemNode>* NodeList)
{
	for (int32 i = 0; i < NodeList->Num(); ++i)
	{
		FMenuItemNode& Node = (*NodeList)[i];
		if (Node.MenuItem != nullptr)
		{
			Builder.AddMenuEntry(FText::FromString(Node.MenuItem->GetMenuName()),
			                     FText::FromString(Node.MenuItem->GetMenuToolTip()),
			                     FSlateIcon(),
			                     FUIAction(FExecuteAction::CreateUObject(Node.MenuItem, &UMenuItem::OnMenuClick)));
		}
		else if (!Node.NodeName.IsEmpty() && Node.Children.Num() > 0)
		{
			Builder.AddSubMenu(FText::FromString(Node.NodeName),
			                   FText::FromString(TEXT("")),
			                   FNewMenuDelegate::CreateStatic(&AddMenuExtension, &Node.Children));
		}
	}
}

void UMenuManager::AddMenuBarExtension(FMenuBarBuilder& Builder)
{
	// 遍历所有的菜单根节点，分别创建PullDownMenu
	for (FMenuItemNode& Node : RootNodeList)
	{
		if (!Node.NodeName.IsEmpty() && Node.Children.Num() > 0)
		{
			Builder.AddPullDownMenu(FText::FromString(Node.NodeName),
			                        FText::FromString(TEXT("")),
			                        FNewMenuDelegate::CreateStatic(&AddMenuExtension, &Node.Children));
		}
	}
}

void UMenuManager::AddMenuItemToNodeList(UMenuItem* NewMenuItem)
{
	if (NewMenuItem == nullptr)
	{
		return;
	}
	// 根据菜单路径获取需要的菜单节点位置
	TArray<FString> MenuNames;
	FString MenuPath = NewMenuItem->GetMenuPath();
	if (MenuPath.IsEmpty())
	{
		return;
	}
	FString Left;
	while (MenuPath.Split("/", &Left, &MenuPath))
	{
		if (Left.IsEmpty())
		{
			continue;
		}
		MenuNames.Add(Left);
	}
	MenuNames.Add(MenuPath);
	// 根据名字查询根节点，如果不存在则创建
	FMenuItemNode* RootNode = FindMenuItemNode(RootNodeList, MenuNames[0]);
	if (RootNode == nullptr)
	{
		FMenuItemNode MenuItemNode;
		MenuItemNode.NodeName = MenuNames[0];
		int32 Index = RootNodeList.Add(MenuItemNode);
		RootNode = &RootNodeList[Index];
	}
	// 然后根据解析出来的MenuNames，逐层查询节点，如果不存在则创建
	FMenuItemNode* ParentNode = RootNode;
	for (int32 i = 1; i < MenuNames.Num(); ++i)
	{
		FString ChildName = MenuNames[i];
		FMenuItemNode* ChildNode = FindMenuItemNode(ParentNode->Children, ChildName);
		if (ChildNode == nullptr)
		{
			FMenuItemNode MenuItemNode;
			MenuItemNode.NodeName = ChildName;
			int32 Index = ParentNode->Children.Add(MenuItemNode);
			ChildNode = &ParentNode->Children[Index];
		}
		ParentNode = ChildNode;
	}
	// 最后叶子节点赋值为待插入的菜单
	ParentNode->MenuItem = NewMenuItem;
}
