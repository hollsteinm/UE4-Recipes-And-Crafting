// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "IngredientCraftableFactory.h"
#include "GameFramework/Actor.h"
#include "RecipeMakerComponent.h"

UIngredientCraftableFactory::UIngredientCraftableFactory() :
	Count(1),
	bApplyToActorRecipeMakerComponent(true)
{

}

UObject* UIngredientCraftableFactory::CreateInternal_Implementation(class AController* Instigator, class AActor* Owner) const
{
	UIngredient* IngredientToAdd = Cast<UIngredient>(Ingredient.ResolveObject());
	if (!IngredientToAdd)
	{
		IngredientToAdd = Cast<UIngredient>(Ingredient.TryLoad());
	}

	if (IngredientToAdd)
	{
		if (bApplyToActorRecipeMakerComponent)
		{
			URecipeMakerComponent* Crafting = Owner->FindComponentByClass<URecipeMakerComponent>();
			if (Crafting)
			{
				Crafting->AddIngredient(IngredientToAdd, Count);
			}
		}
	}
	
	return IngredientToAdd;
}