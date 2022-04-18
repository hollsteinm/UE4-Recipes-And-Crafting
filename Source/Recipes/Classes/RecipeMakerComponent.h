// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "RecipeMakerComponent.generated.h"

/*
** The Enum that describes how to add Ingredients to the RecipeMakerComponent.Ingredients array.
*/
UENUM(BlueprintType)
enum class EAddIngredientPolicy : uint8
{
	AppendCount						UMETA(Description="Append the Count value to the Count Property of the First IngredientInstance with the same name (Case Ignored) of the Ingredient"),
	AppendInstance					UMETA(Description="Add a new IngreientInstance and leave the Count Properties as they are"),
	AppendCountWithMaxThenInstance	UMETA(Description="Append Count to an IngredientInstance with the same Name (Case Ignored) as the Ingredient, until a max value is hit, then add a new IngredientInstance")
};

/*
Recipe Maker Component is the component that contians all replication, management, and RPC logic for adding 
IngredientInstances, assigning Recipes, and consuming IngredientResources. By default it is set to replicate 
for the Owner actor only. Derived classes can subscribe to various events to listen for changes in state. 
Such changes would be when a recipe is made, an ingredient or recipe is added, and when not enough ingredients 
are present to make a recipe. All validation can be customized via CanMakeRecipeInternal (Blueprints) function 
or the CanMakeRecipeInternal_Implementation (C++) function.
*/
UCLASS( ClassGroup=(Recipes), meta=(BlueprintSpawnableComponent), Blueprintable )
class RECIPES_API URecipeMakerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URecipeMakerComponent();

	//**UObject Interface**//
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//**UObject Interface**//

	//**UActorComponent Interface**//
	virtual void InitializeComponent() override;
	//**UActorComponent Interface**//

	//Simple function to check to see if this Component has a specific recipe.
	UFUNCTION(BlueprintCallable, Category="Recipes")
	bool HasRecipe(class URecipe* Recipe) const;
	
	//Interface function to check for validation of making several counts of a specific recipe
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Recipes")
	virtual bool CanMakeRecipe(class URecipe* Recipe, int32 DesiredCount = 1) const;

	//Checks the instanced ingredient array to validate whether this component's call to MakeRecipe will result in a success.
	UFUNCTION(BlueprintCallable, Category = "Recipes")
	bool HasAllIngredientsForRecipe(class URecipe* Recipe, int32 DesiredCount = 1) const;

	/*
	Calls a validation wrapped function to make a recipe of a specified count. This funciton wraps the Server RPC method,
	so it will automatically call the RPC if run on a Client.
	*/
	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void MakeRecipe(class URecipe* Recipe, int32 DesiredCount = 1);

	/*
	Add a non-instanced Recipe to the Components Recipe array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void AddRecipe(class URecipe* NewRecipe);

	/*
	Add an IngredientInstance to the Ingredient Array that wraps the supplied UIngredient DataAsset. It will add the count and/or
	instances based on the AddIngredientPolicy and MaxIngredientCountPerInstance property values.
	*/
	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void AddIngredient(const UIngredient* NewIngredients, int32 Count);

	/*
	Removes a specific IngredientInstance (and its count) from the Ingredient array. This ignores the bRemoveIngredientOnZeroCount
	boolean value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void RemoveIngredient(UIngredientInstance* Ingredient);

	/*
	Reduces the count of IngredientInstances that have the same Name as the supplied UIngredient DataAsset. It will work to reduce
	an IngredientInstance to zero before moving onto the next Instance. If bRemoveIngredientOnZeroCount is true, it will also
	remove the IngredientInstance(s) from the Ingredient array if it/they reach a Count of zero.
	*/
	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void ReduceIngredient(const UIngredient* Ingredient, int32 Count);

	FORCEINLINE TArray<class UIngredientInstance*> GetIngredients() { return Ingredients; }

	FORCEINLINE TArray<class URecipe*> GetRecipes() { return Recipes; }

protected:
	/*
	An array of Recipes that this Component can make. If set in defaults, all instances will start off with these Recipes. This
	array is not replicated, but is updated via the Server via a Client RPC call if AddRecipe is called to add a Recipe to this
	array. It is available for SaveGame.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Recipes", SaveGame)
	TArray<class URecipe*> Recipes;
	
	/*
	An array of UIngredientInstances that this component owns. This array is Replicated and available for SaveGame.
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Recipes", Replicated, SaveGame, Instanced)
	TArray<class UIngredientInstance*> Ingredients;

	/*
	Overridable function to validate the creation of a Recipe.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Recipes")
	bool CanMakeRecipeInternal(class URecipe* Recipe, int32 DesiredCount = 1) const;

	/*
	Blueprint Event called when MakeRecipe has completed. CraftedItems may or may not contain Objects. Called on Server if a 
	Networked game.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnItemsCrafted(const TArray<class UObject*>& CraftedItems) const;

	/*
	Blueprint Event called when a Recipe has been added via AddRecipe. Called by the Server if a Networked game.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnRecipeAdded(const class URecipe* NewRecipe) const;

	/*
	Blueprint Event called when IngredientInstances / IngredientInstnaces.Count has been updated that make the Name of the
	UIngredient Data Asset supplied. Called on the Server if a Networked game.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnIngredientsAdded(const class UIngredient* NewIngredient, int32 NumAdded) const;

	/*
	Blueprint Event called when a check for HasAllIngredients finds missing ingredients. It supplies the name of the missing
	Ingredient as well as how many are needed, and how many the Component currently has (as an aggregate total).
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnMissingIngredients(const FText& IngredientName, int32 Needed, int32 Have) const;

private:
	/*
	Should instances with a zero count be removed from the Ingredient array?
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Recipes")
	bool bRemoveIngredientOnZeroCount;

	/*
	Default class that will be used to spawn IngredientInstances. This allows for developer specified derived UIngredientInstance
	classes in the plugin.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Recipes")
	TSubclassOf<UIngredientInstance> DefaultIngredientClass;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MakeRecipe(class URecipe* Recipe, int32 DesiredCount);
	virtual void Server_MakeRecipe_Implementation(class URecipe* Recipe, int32 DesiredCount);
	virtual bool Server_MakeRecipe_Validate(class URecipe* Recipe, int32 DesiredCount) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddRecipe(class URecipe* NewBook);
	virtual void Server_AddRecipe_Implementation(class URecipe* NewBook);
	virtual bool Server_AddRecipe_Validate(class URecipe* NewBook) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddIngredient(const UIngredient* NewIngredients, int32 Count);
	virtual void Server_AddIngredient_Implementation(const UIngredient* NewIngredients, int32 Count);
	virtual bool Server_AddIngredient_Validate(const UIngredient* NewIngredients, int32 Count) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveIngredient(UIngredientInstance* Ingredient);
	virtual void Server_RemoveIngredient_Implementation(UIngredientInstance* Ingredient);
	virtual bool Server_RemoveIngredient_Validate(UIngredientInstance* Ingredient) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReduceIngredient(const UIngredient* Ingredient, int32 Count);
	virtual void Server_ReduceIngredient_Implementation(const UIngredient* Ingredient, int32 Count);
	virtual bool Server_ReduceIngredient_Validate(const UIngredient* Ingredient, int32 Count) const;

	UFUNCTION(Client, Reliable)
	void Client_AddRecipe(class URecipe* Recipe);
	virtual void Client_AddRecipe_Implementation(class URecipe* Recipe);

	void AddNewIngredient(const UIngredient* NewIngredients, int32 Count);
};
