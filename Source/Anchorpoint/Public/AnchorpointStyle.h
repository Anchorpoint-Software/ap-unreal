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
};
