// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointStyle.h"

#include <Styling/SlateStyleRegistry.h>
#include <IPluginManager.h>

FAnchorpointStyle::FAnchorpointStyle() : FSlateStyleSet(TEXT("AnchorpointStyle"))
{
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Anchorpoint"));
	if (ensure(Plugin.IsValid()))
	{
		SetContentRoot(FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources")));
	}

#define IMAGE_BRUSH_SVG(RelativePath, ...) FSlateVectorImageBrush(RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)

	Set("Icons.Lock", new IMAGE_BRUSH_SVG("Icons/lock", CoreStyleConstants::Icon16x16));

#undef IMAGE_BRUSH_SVG

	FSlateStyleRegistry::RegisterSlateStyle(*this);
}

FAnchorpointStyle::~FAnchorpointStyle()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}

FAnchorpointStyle& FAnchorpointStyle::Get()
{
	static FAnchorpointStyle Inst;
	return Inst;
}
