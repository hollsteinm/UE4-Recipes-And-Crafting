// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once


#include "Widgets/SCompoundWidget.h"
#include "SRecipeWidget.h"
#include "SListView.h"
#include "Recipe.h"

class RECIPES_API SRecipeList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRecipeList)
		: _TitleTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _BodyTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
	{}
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TitleTextStyle)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, BodyTextStyle)
		SLATE_ARGUMENT(TArray<class URecipe*>, Recipes)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};