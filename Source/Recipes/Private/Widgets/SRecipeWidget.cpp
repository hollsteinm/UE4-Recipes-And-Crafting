// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "Widgets/SRecipeWidget.h"
#include "SlateOptMacros.h"
#include "Ingredient.h"
#include "Recipe.h"
#include "Engine/Texture.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SRecipe::Construct(const FArguments& InArgs)
{
	URecipe* Recipe = InArgs._Recipe;
	if (Recipe)
	{
		if (Recipe && Recipe->Icon)
		{
			UTexture* Resource = Recipe->Icon;
			RecipeBrush.SetResourceObject(Resource);
			RecipeBrush.DrawAs = ESlateBrushDrawType::Image;
			RecipeBrush.ImageSize.X = Resource->GetSurfaceWidth();
			RecipeBrush.ImageSize.Y = Resource->GetSurfaceHeight();
		}

		auto UniformGrid = SNew(SUniformGridPanel)
			.SlotPadding(FMargin(3.0f, 3.0f));

		int32 CurrentColumn = 0;
		for (auto Ingredient : Recipe->Ingredients)
		{
			UTexture* Resource = Ingredient->GetIcon();
			int32 AddIndex = INDEX_NONE;
			if (Resource)
			{
				TSharedPtr<FSlateBrush> Temp(new FSlateBrush());
				Temp->SetResourceObject(Resource);
				Temp->DrawAs = ESlateBrushDrawType::Image;
				Temp->ImageSize.X = Resource->GetSurfaceWidth();
				Temp->ImageSize.Y = Resource->GetSurfaceHeight();
				AddIndex = IngredientBrushes.Add(Temp);
			}

			if(AddIndex != INDEX_NONE)
			{
				UniformGrid->AddSlot(CurrentColumn++, 0)
					[
						SNew(SOverlay)
						.ToolTipText(Ingredient->GetNameText())
						+ SOverlay::Slot()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Top)
						[
							SNew(SImage)
							.Image(IngredientBrushes[AddIndex].Get())
						]
						+ SOverlay::Slot()
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Bottom)
						[
							SNew(STextBlock)
							.TextStyle(InArgs._BodyTextStyle)
							.Text(FText::AsNumber(Ingredient->Count))
						]
					];
			}
		}

		ChildSlot
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					.AutoHeight()
					[
						SNew(STextBlock)
						.TextStyle(InArgs._TitleTextStyle)
						.Text(Recipe->Name)
					]
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SNew(SImage)
						.Image(&RecipeBrush)
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					.AutoHeight()
					[
						UniformGrid
					]
					+ SVerticalBox::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					[
						SNew(STextBlock)
						.TextStyle(InArgs._BodyTextStyle)
						.Text(Recipe->Description)
					]
				]
			];
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION