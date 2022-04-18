// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "RecipesBlueprintFunctionLibrary.h"
#include "Ingredient.h"

UIngredientInstance* URecipesBlueprintFunctionLibrary::SpawnIngredient(UObject* Owner, UIngredient* Ingredient, int32 Count, TSubclassOf<UIngredientInstance> TemplateClass)
{
	return UIngredientInstance::FromIngredient(Owner, Ingredient, Count, TemplateClass);
}