// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"
#include "ActorCraftableFactory.h"
#include "CraftableFactory.h"
#include "IngredientCraftableFactory.h"
#include "Ingredient.h"
#include "Recipe.h"
#include "RecipeMakerComponent.h"
#include "RecipesBlueprintFunctionLibrary.h"

class FRecipesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};