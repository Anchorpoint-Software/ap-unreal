// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointHacks.h"

#define private public
// Need to access SPackagesDialog::OnSourceControlStateChanged
#include "SPackagesDialog.h"
#undef private

void FAnchorpointHacksModule::RefreshOpenPackagesDialog()
{
	const TSharedPtr<SWindow> ActiveModalWindow = FSlateApplication::Get().GetActiveModalWindow();
	const TSharedPtr<SWidget> ModalContent = ActiveModalWindow ? ActiveModalWindow->GetContent().ToSharedPtr() : nullptr;
	if (!ModalContent || ModalContent->GetType() != TEXT("SPackagesDialog"))
	{
		return;
	}

	TSharedPtr<SPackagesDialog> PackagesDialog = StaticCastSharedPtr<SPackagesDialog>(ModalContent);
	PackagesDialog->OnSourceControlStateChanged.ExecuteIfBound();
}

IMPLEMENT_MODULE(FAnchorpointHacksModule, AnchorpointHacks)
