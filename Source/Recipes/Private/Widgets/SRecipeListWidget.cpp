// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "SRecipeListWidget.h"
#include "SlateOptMacros.h"
#include "SListView.h"
#include "Recipe.h"
#include "Engine/Texture.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SRecipeList::Construct(const FArguments& InArgs)
{
	auto UniformGrid = SNew(SUniformGridPanel)
		.SlotPadding(FMargin(3.0f, 3.0f));

	int32 CurrentRow = 0;
	for (auto Recipe : InArgs._Recipes)
	{
		UniformGrid->AddSlot(0, CurrentRow++)
			[
				SNew(SRecipe)
				.TitleTextStyle(InArgs._TitleTextStyle)
				.BodyTextStyle(InArgs._BodyTextStyle)
				.Recipe(Recipe)
			];
	}

	ChildSlot
		[
			UniformGrid
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION