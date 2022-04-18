// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once


#include "Widgets/SCompoundWidget.h"
#include "SIngredientWidget.h"
#include "Ingredient.h"

DECLARE_DELEGATE_RetVal_OneParam(const FSlateBrush*, FOnGetBorderBrushForIngredient, UIngredientInstance*)
DECLARE_DELEGATE_RetVal_OneParam(FSlateColor, FOnGetBorderBackgroundColorForIngredient, UIngredientInstance*)

/**
 * 
 */
class RECIPES_API SIngredientListWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SIngredientListWidget) :
		_bIsTiled(true)
		, _bShowDetails(true)
		, _DefaultBorderBackgroundColor(FLinearColor::White)
		, _DefaultBorderImage(FCoreStyle::Get().GetBrush("Border"))
		, _NameTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _CountTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _CategoryTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _DetailsTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _TableRowStyle(&FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"))
		, _MaxImageHeight(128.0f)
		, _MaxImageWidth(128.0f)
		, _ItemWidth(128.0f)
		, _ItemHeight(128.0f)
		, _SectionPadding(10.0f)
		, _ContentPadding(3.0f)
		, _IngredientSectionPadding(10.0f)
		, _IngredientContentPadding(3.0f)
	{}
		SLATE_ATTRIBUTE(TArray<UIngredientInstance*>*, Items)
		SLATE_ATTRIBUTE(const FSlateBrush*, DefaultBorderImage)
		SLATE_ATTRIBUTE(FSlateColor, DefaultBorderBackgroundColor)
		SLATE_ATTRIBUTE(FMargin, SectionPadding)
		SLATE_ATTRIBUTE(FMargin, ContentPadding)
		SLATE_ATTRIBUTE(FMargin, IngredientSectionPadding)
		SLATE_ATTRIBUTE(FMargin, IngredientContentPadding)
		
		SLATE_EVENT(FOnGetBorderBrushForIngredient, OnGetBrushForIngredient)
		SLATE_EVENT(FOnGetBorderBackgroundColorForIngredient, OnGetBackgroundColorForIngredient)

		SLATE_STYLE_ARGUMENT(FTextBlockStyle, NameTextStyle)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, CountTextStyle)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, CategoryTextStyle)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, DetailsTextStyle)
		SLATE_STYLE_ARGUMENT(FTableRowStyle, TableRowStyle)

		SLATE_ARGUMENT(float, MaxImageWidth)
		SLATE_ARGUMENT(float, MaxImageHeight)
		SLATE_ARGUMENT(float, ItemWidth)
		SLATE_ARGUMENT(float, ItemHeight)
		SLATE_ARGUMENT(bool, bIsTiled)
		SLATE_ARGUMENT(bool, bShowDetails)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetOnGetBrushForIngredient(const FOnGetBorderBrushForIngredient& InDelegate);

	void SetOnGetBackgroundColorForIngredient(const FOnGetBorderBackgroundColorForIngredient& InDelegate);

	FORCEINLINE TSharedRef<SListView<UIngredientInstance*>> GetListWidget() { return ListWidget.ToSharedRef(); }

	FORCEINLINE TSharedRef<STextBlock> GetDetailsWidget() { return DetailsWidget.ToSharedRef(); }

	FORCEINLINE TSharedRef<STextBlock> GetCategoryWidget() { return CategoryWidget.ToSharedRef(); }

	FORCEINLINE TSharedRef<ITableRow> GetIngredientWidget(UIngredientInstance* Instance) { return ListWidget->WidgetFromItem(Instance).ToSharedRef(); }

protected:
	virtual TSharedRef<ITableRow> OnGenerateIngredient(UIngredientInstance* Info, const TSharedRef<STableViewBase>& OwnerTable, const FTableRowStyle* RowStyle, SIngredientWidget::FArguments In);

	virtual void OnSelectionChanged(UIngredientInstance* Info, ESelectInfo::Type SelectionType);

	virtual const FSlateBrush* GetBrushForIngredient(UIngredientInstance* Item) const;

	virtual FSlateColor GetBackgroundColorForIngredient(UIngredientInstance* Item) const;

	virtual TArray<UIngredientInstance*>* GetIngredients();

private:
	TAttribute<TArray<UIngredientInstance*>*> Items;

	TSharedPtr<SListView<UIngredientInstance*>> ListWidget;

	TSharedPtr<STextBlock> DetailsWidget;

	TSharedPtr<STextBlock> CategoryWidget;

	FOnGetBorderBrushForIngredient OnGetBrushForIngredient;

	FOnGetBorderBackgroundColorForIngredient OnGetBackgroundColorForIngredient;

	TAttribute<const FSlateBrush*> DefaultBorderImage;

	TAttribute<FSlateColor> DefaultBorderBackgroundColor;

	TArray<UIngredientInstance*> NullList;
};