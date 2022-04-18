// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once


#include "Widgets/SCompoundWidget.h"
#include "SIngredientListWidget.h"
#include "SNumericEntryBox.h"

DECLARE_DELEGATE_TwoParams(FOnRecipeMakePressed, class URecipe*, int32)
DECLARE_DELEGATE_RetVal_TwoParams(bool, FCanMakeRecipe, class URecipe*, int32)
/**
 * 
 */

//refactor
class RECIPES_API SRecipe : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRecipe)
		: _TitleTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _BodyTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _Recipe(nullptr)
	{}
		SLATE_ARGUMENT(class URecipe*, Recipe)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TitleTextStyle)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, BodyTextStyle)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TArray<TSharedPtr<FSlateBrush>> IngredientBrushes;
	FSlateBrush RecipeBrush;
};