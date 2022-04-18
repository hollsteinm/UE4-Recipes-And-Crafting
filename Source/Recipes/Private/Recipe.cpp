// © 2016 Martin Hollstein, All Rights Reserved.
#include "RecipesPrivatePCH.h"
#include "Recipe.h"

TArray<UObject*> URecipe::Craft(class AController* Instigator, class AActor* Owner) const
{
	TArray<UObject*> Results;
	for (const UCraftableFactory* CraftableFactory : CraftablesCreated)
	{
		UObject* Created = CraftableFactory->Create(Instigator, Owner);
		if (Created)
		{
			Results.Add(Created);
		}
	}
	return Results;
}