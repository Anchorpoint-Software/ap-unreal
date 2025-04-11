// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointStyle.h"

#include <Styling/SlateStyleRegistry.h>
#include <Interfaces/IPluginManager.h>
#include <Misc/EngineVersionComparison.h>
#include <Styling/StyleColors.h>

FAnchorpointStyle::FAnchorpointStyle() : FSlateStyleSet(TEXT("AnchorpointStyle"))
{
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Anchorpoint"));
	if (ensure(Plugin.IsValid()))
	{
		SetContentRoot(FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources")));
	}

	FSlateColor IconColor =
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5)
		// Needs to match FDefaultRevisionControlStyle::StatusCheckedOutColor
		FLinearColor::FromSRGBColor(FColor::FromHex("#1FE44B"));
#else 
		FStyleColors::AccentRed;
#endif

#define IMAGE_BRUSH_SVG(RelativePath, ...) FSlateVectorImageBrush(RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)

	Set("Icons.Lock", new IMAGE_BRUSH_SVG("Icons/lock", CoreStyleConstants::Icon16x16, IconColor));

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
