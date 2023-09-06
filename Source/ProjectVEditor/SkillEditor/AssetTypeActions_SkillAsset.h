#pragma once
#include "AssetTypeActions_Base.h"

class FAssetTypeActions_SkillAsset : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_SkillAsset(EAssetTypeCategories::Type InAssetCategory);

	//~ Begin IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
	                             TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;
	//~ Begin IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;
};
