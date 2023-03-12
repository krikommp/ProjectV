// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Messaging/CommonGameDialog.h"

#include "GridConfirmationScreen.generated.h"

class UCommonTextBlock;
class UCommonRichTextBlock;
class UDynamicEntryBox;
class UCommonBorder;

/**
 * UGridConfirmationScreen
 *
 * 通用的确认会话框
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class PROJECTVGAME_API UGridConfirmationScreen : public UCommonGameDialog
{
	GENERATED_BODY()
public:
	virtual void SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback) override;
	virtual void KillDialog() override;

protected:
	virtual void NativeOnInitialized() override;
	virtual void CloseConfirmationWindow(ECommonMessagingResult Result);

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif

private:

	UFUNCTION()
	FEventReply HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	FCommonMessagingResultDelegate OnResultCallback;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Tile;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonRichTextBlock> RichText_Description;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> EntryBox_Buttons;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonBorder> Border_TapToCloseZone;

	UPROPERTY(EditDefaultsOnly, meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle CancelAction;
};
