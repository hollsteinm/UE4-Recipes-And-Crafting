// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "Recipe.generated.h"

/**
 * Basic data classed used to describe Recipes and defined rules for the amount and type of Ingredients needed. It also includes
 * The factory needed to create Objects from a Recipe.
 */
UCLASS(BlueprintType, Blueprintable)
class RECIPES_API URecipe : public UDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Recipe")
	TArray<UObject*> Craft(class AController* Instigator, class AActor* Owner) const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Description;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Category;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	class UTexture* Icon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced)
	TArray<class UIngredientInstance*> Ingredients;

protected:
	UPROPERTY(Instanced, BlueprintReadOnly, EditAnywhere)
	TArray<class UCraftableFactory*> CraftablesCreated;
};
