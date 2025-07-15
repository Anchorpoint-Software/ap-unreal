// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <Widgets/SCompoundWidget.h>

/**
 * Generic popup widget for Anchorpoint dialogs
 */
class ANCHORPOINT_API SAnchorpointPopup : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAnchorpointPopup)
		{
		}

		SLATE_ATTRIBUTE(TSharedPtr<SWindow>, ParentWindow)
		SLATE_ATTRIBUTE(FText, Message)
		SLATE_ATTRIBUTE(TArray<FText>, Buttons)
	SLATE_END_ARGS()

	static int32 Open(const FText& InTitle, const FText& InMessage, const TArray<FText>& InButtons);

	void Construct(const FArguments& InArgs);
	FReply HandleButtonClicked(int32 InResponse);

	int32 Response = INDEX_NONE;
	TSharedPtr<SWindow> ParentWindow;
};

namespace AnchorpointPopups
{
void ShowConflictPopup();
void ShowConflictPopupAnyThread();
}
