// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <Styling/SlateStyle.h>

/**
 * Slate style set for Anchorpoint plugin
 */
class FAnchorpointStyle final : public FSlateStyleSet
{
public:
	FAnchorpointStyle();
	virtual ~FAnchorpointStyle() override;

	/**
	 * Access the singleton instance of this style set
	 */
	static FAnchorpointStyle& Get();

	/**
	 * Sets the FallbackStyleName (ideally to the previous Active Revision Control Style)
	 */
	void SetFallbackStyleName(const FName& InName);

private:
	//~Begin FSlateStyleSet interface
	virtual const FSlateBrush* GetBrush(const FName PropertyName, const ANSICHAR* Specifier = nullptr, const ISlateStyle* RequestingStyle = nullptr) const override;
	virtual const FSlateBrush* GetOptionalBrush(const FName PropertyName, const ANSICHAR* Specifier = nullptr, const FSlateBrush* const InDefaultBrush = FStyleDefaults::GetNoBrush()) const override;
	//~End FSlateStyleSet interface

	/**
	 * Name of the initial ActiveRevisionControlStyle, where we will forward our calls to if we cannot find an icon
	 */
	FName FallbackStyleName;
};
