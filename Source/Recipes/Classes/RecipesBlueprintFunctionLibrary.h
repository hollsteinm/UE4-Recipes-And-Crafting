// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RecipesBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RECIPES_API URecipesBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	* Helper function to create Ingredient Instances outside of the Component.
	**/
	UFUNCTION(BlueprintCallable, Category = "Ingredient", meta = (DeterminesOutputType = "TemplateClass"))
		static class UIngredientInstance* SpawnIngredient(class UObject* Owner, class UIngredient* Ingredient, int32 Count, TSubclassOf<class UIngredientInstance> TemplateClass);
};
