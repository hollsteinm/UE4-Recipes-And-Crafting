// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once

#include "Object.h"
#include "CraftableFactory.generated.h"

/**
 * Base class used to create objects for Recipes.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class RECIPES_API UCraftableFactory : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	* Public function that creates an Object in game from this Factory
	**/
	UFUNCTION(BlueprintCallable, Category = "CraftableFactory")
		UObject* Create(class AController* Instigator, class AActor* Owner) const;

protected:
	/**
	* Overridable implementation for Create.
	**/
	UFUNCTION(BlueprintNativeEvent, Category = "CraftableFactory")
		UObject* CreateInternal(class AController* Instigator, class AActor* Owner) const;
	
};
