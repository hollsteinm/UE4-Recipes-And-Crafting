// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "SIngredientListWidget.h"
#include "SlateOptMacros.h"
#include "SListView.h"
#include "STileView.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SIngredientListWidget::Construct(const FArguments& InArgs)
{
	Items = InArgs._Items;
	DefaultBorderBackgroundColor = InArgs._DefaultBorderBackgroundColor;
	DefaultBorderImage = InArgs._DefaultBorderImage;

	SIngredientWidget::FArguments ItemArgs;
	ItemArgs.bIsTile(InArgs._bIsTiled)
		.CountTextStyle(InArgs._CountTextStyle)
		.NameTextStyle(InArgs._NameTextStyle)
		.MaxImageHeight(InArgs._MaxImageHeight)
		.MaxImageWidth(InArgs._MaxImageWidth)
		.ContentPadding(InArgs._IngredientContentPadding)
		.SectionPadding(InArgs._IngredientSectionPadding);

	if (InArgs._bIsTiled)
	{
		SAssignNew(ListWidget, STileView<UIngredientInstance*>)
			.OnSelectionChanged(this, &SIngredientListWidget::OnSelectionChanged)
			.ItemHeight(InArgs._ItemHeight)
			.ItemWidth(InArgs._ItemWidth)
			.ListItemsSource(GetIngredients())
			.SelectionMode(InArgs._bShowDetails ? ESelectionMode::Single : ESelectionMode::None)
			.OnGenerateTile(this, &SIngredientListWidget::OnGenerateIngredient, InArgs._TableRowStyle, ItemArgs);
	}
	else
	{
		SAssignNew(ListWidget, SListView<UIngredientInstance*>)
			.OnSelectionChanged(this, &SIngredientListWidget::OnSelectionChanged)
			.ItemHeight(InArgs._ItemHeight)
			.ListItemsSource(GetIngredients())
			.SelectionMode(InArgs._bShowDetails ? ESelectionMode::Single : ESelectionMode::None)
			.OnGenerateRow(this, &SIngredientListWidget::OnGenerateIngredient, InArgs._TableRowStyle, ItemArgs);
	}


	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(4)
		.Padding(InArgs._SectionPadding)
		[
			GetListWidget()
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1)
		.Padding(InArgs._SectionPadding)
		[
			SNew(SVerticalBox)
			.Visibility(InArgs._bShowDetails ? EVisibility::Visible : EVisibility::Collapsed)

			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.AutoHeight()
			.Padding(InArgs._ContentPadding)
			[
				SAssignNew(CategoryWidget, STextBlock)
				.TextStyle(InArgs._CategoryTextStyle)
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.AutoHeight()
			.Padding(InArgs._ContentPadding)
			[
				SAssignNew(DetailsWidget, STextBlock)
				.TextStyle(InArgs._DetailsTextStyle)
				.AutoWrapText(true)
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SIngredientListWidget::OnSelectionChanged(UIngredientInstance* Info, ESelectInfo::Type SelectionType)
{
	if (Info && CategoryWidget.IsValid() && DetailsWidget.IsValid())
	{
		CategoryWidget->SetText(Info->GetCategory());
		DetailsWidget->SetText(Info->GetDescription());
	}
}

TSharedRef<ITableRow> SIngredientListWidget::OnGenerateIngredient(UIngredientInstance* Info, const TSharedRef<STableViewBase>& OwnerTable, const FTableRowStyle* RowStyle, SIngredientWidget::FArguments In)
{
	return SNew(STableRow<UIngredient*>, OwnerTable)
		.Style(RowStyle)
		.Content()
		[
			SNew(SIngredientWidget)
			.Ingredient(Info)
			.bIsTile(In._bIsTile)
			.CountTextStyle(In._CountTextStyle)
			.NameTextStyle(In._NameTextStyle)
			.MaxImageHeight(In._MaxImageHeight)
			.MaxImageWidth(In._MaxImageWidth)
			.BorderBackgroundColor(this, &SIngredientListWidget::GetBackgroundColorForIngredient, Info)
			.BorderImage(this, &SIngredientListWidget::GetBrushForIngredient, Info)		
			.ContentPadding(In._ContentPadding)
			.SectionPadding(In._SectionPadding)
		];
}

void SIngredientListWidget::SetOnGetBrushForIngredient(const FOnGetBorderBrushForIngredient& InDelegate)
{
	OnGetBrushForIngredient = InDelegate;
}

void SIngredientListWidget::SetOnGetBackgroundColorForIngredient(const FOnGetBorderBackgroundColorForIngredient& InDelegate)
{
	OnGetBackgroundColorForIngredient = InDelegate;
}

const FSlateBrush* SIngredientListWidget::GetBrushForIngredient(UIngredientInstance* Item) const
{
	if (OnGetBrushForIngredient.IsBound() && Item)
	{
		return OnGetBrushForIngredient.Execute(Item);
	}
	else
	{
		return DefaultBorderImage.Get();
	}
}

FSlateColor SIngredientListWidget::GetBackgroundColorForIngredient(UIngredientInstance* Item) const
{
	if (OnGetBackgroundColorForIngredient.IsBound() && Item)
	{
		return OnGetBackgroundColorForIngredient.Execute(Item);
	}
	else
	{
		return DefaultBorderBackgroundColor.Get();
	}
 }

TArray<UIngredientInstance*>* SIngredientListWidget::GetIngredients()
{
	if (Items.IsSet() || Items.IsBound())
	{
		TArray<UIngredientInstance*>* Result = Items.Get();
		return Result == nullptr ? &NullList : Result;
	}
	else
	{
		return &NullList;
	}
}