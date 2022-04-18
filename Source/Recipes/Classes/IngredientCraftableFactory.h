// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once

#include "CraftableFactory.h"
#include "IngredientCraftableFactory.generated.h"

/**
 * Will apply ingredients to the Owner Actor if it has a RecipeMakerComponent. The Object returned is a pointer to the Ingredient
 */
UCLASS()
class RECIPES_API UIngredientCraftableFactory : public UCraftableFactory
{
	GENERATED_BODY()
public:
	UIngredientCraftableFactory();
	
protected:
	/**UCraftableFactory Interface**/
	virtual UObject* CreateInternal_Implementation(class AController* Instigator, class AActor* Owner) const override;
	/**UCraftableFactory End**/
	
	/*
	* The Ingredient Asset to use for the Spawned UIngredientInstance
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowedClasses = "Ingredient", ExactClass = "false"))
	FStringAssetReference Ingredient;

	/*
	* The Count for the UIngredientInstance that will be spawned
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CraftableFactory")
	int32 Count;

	/**
	* Try to search for a URecipeMakerComponent on the Owner Actor passed to the Create function
	**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CraftableFactory")
	bool bApplyToActorRecipeMakerComponent;
};
