// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "Widgets/SIngredientWidget.h"
#include "SlateOptMacros.h"
#include "Engine/Texture.h"
#include "Ingredient.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SIngredientWidget::Construct(const FArguments& InArgs)
{
	MyInfo = InArgs._Ingredient;
	MaxWidth = InArgs._MaxImageWidth;
	MaxHeight = InArgs._MaxImageHeight;

	if(MyInfo.Get())
	{
		ConfigureImageBrush();

		SAssignNew(BorderWidget, SBorder)
			.BorderBackgroundColor(InArgs._BorderBackgroundColor)
			.BorderImage(InArgs._BorderImage)
			[
				SAssignNew(CountTextWidget, STextBlock)
				.TextStyle(InArgs._CountTextStyle)
				.Text(this, &SIngredientWidget::GetIngredientCount)
			];

		SAssignNew(NameTextWidget, STextBlock)
			.TextStyle(InArgs._NameTextStyle)
			.Text(this, &SIngredientWidget::GetIngredientName);

		SAssignNew(ImageWidget, SImage)
			.Image(&InfoBrush);
		
		if(InArgs._bIsTile)
		{
			ChildSlot
			.Padding(InArgs._SectionPadding)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					GetImageWidget()
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Bottom)
				.Padding(InArgs._ContentPadding)
				[
					GetNameTextWidget()
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Top)
				.Padding(InArgs._ContentPadding)
				[
					GetBorderWidget()
				]
			];
		}
		else
		{
			ChildSlot
			.Padding(InArgs._SectionPadding)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(InArgs._ContentPadding)
				[
					GetImageWidget()
				]
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.Padding(InArgs._ContentPadding)
				[
					GetNameTextWidget()
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(InArgs._ContentPadding)
				[
					GetBorderWidget()
				]
			];
		}
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SIngredientWidget::ConfigureImageBrush()
{
	if (MyInfo.IsSet())
	{
		UIngredientInstance* Info = MyInfo.Get();
		UTexture* Icon = Info->GetIcon();
		if (Icon)
		{
			InfoBrush.SetResourceObject(Icon);
			InfoBrush.ImageSize.X = FMath::Min(FMath::Max(0.0f, MaxWidth), Icon->GetSurfaceWidth());
			InfoBrush.ImageSize.Y = FMath::Min(FMath::Max(0.0f, MaxHeight), Icon->GetSurfaceHeight());
			InfoBrush.DrawAs = ESlateBrushDrawType::Image;
		}
	}
}

void SIngredientWidget::SetIngredient(TAttribute<UIngredientInstance*>& Ingredient)
{
	MyInfo = Ingredient;
	ConfigureImageBrush();
}

void SIngredientWidget::SetBorderImage(TAttribute<const FSlateBrush*>& NewBorderImage)
{
	GetBorderWidget()->SetBorderImage(NewBorderImage);
}

void SIngredientWidget::SetBorderBackgroundColor(TAttribute<FSlateColor>& NewBorderBackgroundColor)
{
	GetBorderWidget()->SetBorderBackgroundColor(NewBorderBackgroundColor);
}

FText SIngredientWidget::GetIngredientCount() const
{
	if (MyInfo.Get())
	{
		return FText::AsNumber(MyInfo.Get()->Count);
	}
	else
	{
		return FText::AsNumber(0);
	}
}

FText SIngredientWidget::GetIngredientName() const
{
	if (MyInfo.Get())
	{
		return MyInfo.Get()->GetNameText();
	}
	else
	{
		return FText::FromName(NAME_None);
	}
}
