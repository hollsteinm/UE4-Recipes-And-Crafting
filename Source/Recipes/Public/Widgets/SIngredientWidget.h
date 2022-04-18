// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once


#include "Widgets/SCompoundWidget.h"
#include "SlateBasics.h"
#include "STextBlock.h"
#include "SImage.h"
#include "SBorder.h"

/**
 * 
 */
class RECIPES_API SIngredientWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SIngredientWidget) :
		_bIsTile(false)
		, _BorderBackgroundColor(FLinearColor::White)
		, _BorderImage(FCoreStyle::Get().GetBrush("Border"))
		, _NameTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _CountTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _MaxImageHeight(128.0f)
		, _MaxImageWidth(128.0f)
		, _SectionPadding(10.0f)
		, _ContentPadding(3.0f)
	{}
		SLATE_ATTRIBUTE(class UIngredientInstance*, Ingredient)
		SLATE_ATTRIBUTE(const FSlateBrush*, BorderImage)
		SLATE_ATTRIBUTE(FSlateColor, BorderBackgroundColor)

		SLATE_STYLE_ARGUMENT(FTextBlockStyle, NameTextStyle)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, CountTextStyle)

		SLATE_ARGUMENT(float, MaxImageWidth)
		SLATE_ARGUMENT(float, MaxImageHeight)
		SLATE_ARGUMENT(bool, bIsTile)

		SLATE_ATTRIBUTE(FMargin, ContentPadding)
		SLATE_ATTRIBUTE(FMargin, SectionPadding)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetIngredient(TAttribute<class UIngredientInstance*>& Ingredient);

	void SetBorderImage(TAttribute<const FSlateBrush*>& NewBorderImage);

	void SetBorderBackgroundColor(TAttribute<FSlateColor>& NewBorderBackgroundColor);

	FORCEINLINE TSharedRef<STextBlock> GetNameTextWidget() { return NameTextWidget.ToSharedRef(); }

	FORCEINLINE TSharedRef<STextBlock> GetCountTextWidget() { return CountTextWidget.ToSharedRef(); }

	FORCEINLINE TSharedRef<SImage> GetImageWidget() { return ImageWidget.ToSharedRef(); }

	FORCEINLINE TSharedRef<SBorder> GetBorderWidget() { return BorderWidget.ToSharedRef(); }

protected:
	FText GetIngredientCount() const;

	FText GetIngredientName() const;

	TAttribute<class UIngredientInstance*> MyInfo;

private:
	TSharedPtr<STextBlock> NameTextWidget;

	TSharedPtr<STextBlock> CountTextWidget;

	TSharedPtr<SImage> ImageWidget;

	TSharedPtr<SBorder> BorderWidget;

	FSlateBrush InfoBrush;

	float MaxWidth;

	float MaxHeight;

	void ConfigureImageBrush();
};
