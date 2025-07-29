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
	Set("Icons.OutdatedModifiedLocked", new IMAGE_BRUSH_SVG("Icons/outdated_modified_locked", CoreStyleConstants::Icon16x16, IconColor));
	Set("Icons.OutdatedModifiedUnlocked", new IMAGE_BRUSH_SVG("Icons/outdated_modified_unlocked", CoreStyleConstants::Icon16x16, IconColor));

#undef IMAGE_BRUSH_SVG

	FSlateStyleRegistry::RegisterSlateStyle(*this);
}

FAnchorpointStyle::~FAnchorpointStyle()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}

const FSlateBrush* FAnchorpointStyle::GetBrush(const FName PropertyName, const ANSICHAR* Specifier, const ISlateStyle* RequestingStyle) const
{
	const FSlateBrush* Result = FSlateStyleSet::GetBrush(PropertyName, Specifier, RequestingStyle);
	if (Result && Result != GetDefaultBrush())
	{
		return Result;
	}

	if (const ISlateStyle* FallbackStyle = FSlateStyleRegistry::FindSlateStyle(FallbackStyleName))
	{
		return FallbackStyle->GetBrush(PropertyName, Specifier, RequestingStyle);
	}

	return nullptr;
}

const FSlateBrush* FAnchorpointStyle::GetOptionalBrush(const FName PropertyName, const ANSICHAR* Specifier, const FSlateBrush* const InDefaultBrush) const
{
	const FSlateBrush* Result = FSlateStyleSet::GetOptionalBrush(PropertyName, Specifier, InDefaultBrush);
	if (Result && Result != InDefaultBrush)
	{
		return Result;
	}

	if (const ISlateStyle* FallbackStyle = FSlateStyleRegistry::FindSlateStyle(FallbackStyleName))
	{
		return FallbackStyle->GetOptionalBrush(PropertyName, Specifier, InDefaultBrush);
	}

	return nullptr;
}

FAnchorpointStyle& FAnchorpointStyle::Get()
{
	static FAnchorpointStyle Inst;
	return Inst;
}

void FAnchorpointStyle::SetFallbackStyleName(const FName& InName)
{
	FallbackStyleName = InName;
}
