// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointPopup.h"

#include <Widgets/Layout/SUniformGridPanel.h>

int32 SAnchorpointPopup::Open(const FText& InTitle, const FText& InMessage, const TArray<FText>& InButtons)
{
	TSharedRef<SWindow> ModalWindow = SNew(SWindow)
		.Title(InTitle)
		.SizingRule(ESizingRule::Autosized)
		.AutoCenter(EAutoCenter::PreferredWorkArea)
		.SupportsMinimize(false)
		.SupportsMaximize(false);

	TSharedRef<SAnchorpointPopup> MessageBox = SNew(SAnchorpointPopup)
		.ParentWindow(ModalWindow)
		.Message(InMessage)
		.Buttons(InButtons);

	ModalWindow->SetContent(MessageBox);

	GEditor->EditorAddModalWindow(ModalWindow);

	return MessageBox->Response;
}

void SAnchorpointPopup::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow.Get();
	ParentWindow->SetWidgetToFocusOnActivate(SharedThis(this));

	FText Message = InArgs._Message.Get();
	TArray<FText> Buttons = InArgs._Buttons.Get();

	TSharedPtr<SUniformGridPanel> ButtonBox;

	this->ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoHeight()
			.Padding(12.0f)
			[
				SNew(STextBlock)
				.Text(Message)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(12.0f, 0.0f, 12.0f, 12.0f)
			[
				SAssignNew(ButtonBox, SUniformGridPanel)
				.SlotPadding(FAppStyle::GetMargin("StandardDialog.SlotPadding"))
				.MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
				.MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
			]
		]
	];

	for (int32 Idx = 0; Idx < Buttons.Num(); Idx++)
	{
		ButtonBox->AddSlot(Idx, 0)
		[
			SNew(SButton)
			.Text(Buttons[Idx])
			.OnClicked(this, &SAnchorpointPopup::HandleButtonClicked, Idx)
			.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
			.HAlign(HAlign_Center)
		];
	}
}

FReply SAnchorpointPopup::HandleButtonClicked(int32 InResponse)
{
	Response = InResponse;
	ParentWindow->RequestDestroyWindow();
	return FReply::Handled();
}