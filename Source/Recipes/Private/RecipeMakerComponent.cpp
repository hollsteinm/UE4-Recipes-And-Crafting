// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "Recipe.h"
#include "Ingredient.h"
#include "RecipeMakerComponent.h"
#include "CraftableFactory.h"
#include "UnrealNetwork.h"
#include "Templates/SharedPointer.h"
#include "GameFramework/Actor.h"
#include "Engine/ActorChannel.h"


//**Start Helper Algorithms**//
struct FSortByNameCaseIgnored
{
	bool operator()(const UIngredientInstance& A, const UIngredientInstance& B) const
	{
		return A.GetNameText().CompareToCaseIgnored(B.GetNameText()) < 0;
	}
};

struct FFindByNameCaseIgnored
{
	FFindByNameCaseIgnored() = delete;
	FFindByNameCaseIgnored(const FFindByNameCaseIgnored& RHS)
		: Name(RHS.Name)
	{}

	FFindByNameCaseIgnored(const FText& NameToSearch)
		: Name(NameToSearch)
	{}

	FText Name;

	bool operator()(const UIngredientInstance* A) const
	{
		check(A);
		return A->GetNameText().EqualToCaseIgnored(Name);
	}
};

struct FFindByNonZeroCountAndNameCaseIgnored
{
	FFindByNonZeroCountAndNameCaseIgnored() = delete;
	FFindByNonZeroCountAndNameCaseIgnored(const FFindByNameCaseIgnored& RHS)
		: Name(RHS.Name)
	{}

	FFindByNonZeroCountAndNameCaseIgnored(const FText& NameToSearch)
		: Name(NameToSearch)
	{}

	FText Name;

	bool operator()(const UIngredientInstance* A) const
	{
		check(A);
		return A->GetNameText().EqualToCaseIgnored(Name) && A->Count > 0;
	}
};

struct FFindAndReduce
{
	FFindAndReduce() {}
	FFindAndReduce(const FFindAndReduce& RHS) = delete;
	void operator=(const FFindAndReduce& RHS) = delete;

	FORCEINLINE void operator()(TArray<UIngredientInstance*>& Ingredients, const TArray<UIngredientInstance*> RecipeIngredients, const bool bRemoveIngredientOnZeroCount)
	{
		for (const UIngredientInstance* Info : RecipeIngredients)
		{
			int32 CountNeeded = Info->Count;
			UIngredientInstance** WasFound = Ingredients.FindByPredicate(FFindByNonZeroCountAndNameCaseIgnored(Info->GetNameText()));
			while (WasFound && CountNeeded > 0)
			{
				UIngredientInstance* ComponentIngredient = *WasFound;
				int32 ReduceBy = FMath::Min(CountNeeded, ComponentIngredient->Count);
				ComponentIngredient->Count -= ReduceBy;
				if (bRemoveIngredientOnZeroCount && ComponentIngredient->Count <= 0)
				{
					Ingredients.Remove(ComponentIngredient);
				}
				CountNeeded -= ReduceBy;
				WasFound = Ingredients.FindByPredicate(FFindByNonZeroCountAndNameCaseIgnored(Info->GetNameText()));
			}
		}
	}
};

struct FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount
{
	FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount() {}
	FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount(const FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount& RHS) = delete;
	void operator=(const FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount& RHS) = delete;

	typedef TBaseDelegate<void, FText, int32, int32> FNeedMoreNotify;

	FORCEINLINE bool operator()(const TArray<UIngredientInstance*> Ingredients, const TArray<UIngredientInstance*> RecipeIngredients, int32 DesiredCount, FNeedMoreNotify OnNotEnough = TBaseDelegate<void, FText, int32, int32>())
	{
		bool bResult = true;
		TMap<FString, int32> Merge;
		TMap<FString, FText> KeyToTextMap;
		for (const auto Ingredient : RecipeIngredients)
		{
			if (Merge.Contains(Ingredient->GetNameText().ToString()))
			{
				Merge[Ingredient->GetNameText().ToString()] += Ingredient->Count;
			}
			else
			{
				Merge.Add(Ingredient->GetNameText().ToString(), Ingredient->Count);
				KeyToTextMap.Add(Ingredient->GetNameText().ToString(), Ingredient->GetNameText());
			}
		}

		for (auto KVP : Merge)
		{
			int32 Needed = KVP.Value * DesiredCount;
			int32 Have = 0;
			bool bLocalResult = false;

			for (auto ComponentIngredient : Ingredients.FilterByPredicate(FFindByNonZeroCountAndNameCaseIgnored(KeyToTextMap[KVP.Key])))
			{
				Have += ComponentIngredient->Count;
			}

			bLocalResult = Have >= Needed;

			if (!bLocalResult)
			{
				OnNotEnough.ExecuteIfBound(KeyToTextMap[KVP.Key], Needed, Have);
			}

			bResult &= bLocalResult;
		}
		return bResult;
	}
};

struct FAddCountAndIngredients
{
	FAddCountAndIngredients() {}
	FAddCountAndIngredients(const FAddCountAndIngredients& RHS) = delete;
	void operator=(const FAddCountAndIngredients& RHS) = delete;

	FORCEINLINE void operator()(UObject* Owner, TArray<UIngredientInstance*>& Ingredients, UIngredientInstance* First, const UIngredient* NewIngredients, int32 Count, const int32 Max, const TSubclassOf<UIngredientInstance> DefaultIngredientClass)
	{
		if (First)
		{
			if ((First->Count + Count) > Max)
			{
				int32 Add = Max - First->Count;
				First->Count += Add;
				if (Count - Add > 0)
				{
					UIngredientInstance* Next = UIngredientInstance::FromIngredient(Owner, NewIngredients, 0, DefaultIngredientClass);
					if (Next)
					{
						Ingredients.Add(Next);
						FAddCountAndIngredients()(Owner, Ingredients, Next, NewIngredients, Count - Add, Max, DefaultIngredientClass);
					}
				}
			}
			else
			{
				First->Count += Count;
			}
		}
	}
};

//**Start Helper Algorithm Automation Tests**//
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRecipeMakerReductionAlgorithmTests, "Recipes.RecipeMakerComponent Reduction Tests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)
bool FRecipeMakerReductionAlgorithmTests::RunTest(const FString& Parameters)
{
	UIngredient* TestIngredient_One = NewObject<UIngredient>();
	TestIngredient_One->Name = NSLOCTEXT("RecipeTests", "IngredientOne_Name", "Apple");
	UIngredient* TestIngredient_Two = NewObject<UIngredient>();
	TestIngredient_Two->Name = NSLOCTEXT("RecipeTests", "IngredientTwo_Name", "Chewed Bread");
	URecipe* TestRecipe = NewObject<URecipe>();
	TestRecipe->Ingredients =
	{
		UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 3),
		UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 1)
	};

	//Find and Reduce Ingredients, does not remove 0 counts
	{
		TArray<UIngredientInstance*> Ingredients =
		{
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 3),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 10),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 3)
		};

		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);
		TestEqual(TEXT("Find and Reduce Ingredients, does not remove 0 counts: Reduces ingredients to zero, does not remove"), Ingredients.Num(), 3);
		TestEqual(TEXT("Find and Reduce Ingredients, does not remove 0 counts: First Match ingredient is 0"), Ingredients[0]->Count, 0);
		TestEqual(TEXT("Find and Reduce Ingredients, does not remove 0 counts: Second Match ingredient is 10"), Ingredients[1]->Count, 10);
		TestEqual(TEXT("Find and Reduce Ingredients, does not remove 0 counts: Third match ingredient is 2"), Ingredients[2]->Count, 2);
	}

	//Find and Reduce Ingredients removes 0 counts
	{
		TArray<UIngredientInstance*> Ingredients =
		{
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 3),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 10),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 3)
		};

		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, true);
		TestEqual(TEXT("Find and Reduce Ingredients removes 0 counts: Reduces ingredients to zero, does remove"), Ingredients.Num(), 2);
		TestEqual(TEXT("Find and Reduce Ingredients removes 0 counts: Second Match ingredient is 10"), Ingredients[0]->Count, 10);
		TestEqual(TEXT("Find and Reduce Ingredients removes 0 counts: Third match ingredient is 2"), Ingredients[1]->Count, 2);
	}

	//Find and Reduce Ingredients multi pass
	{
		TArray<UIngredientInstance*> Ingredients =
		{
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 3),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 10),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 3)
		};

		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);
		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);
		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);

		TestEqual(TEXT("Find and Reduce Ingredients multi pass: First Match ingredient is 0"), Ingredients[0]->Count, 0);
		TestEqual(TEXT("Find and Reduce Ingredients multi pass: Second Match ingredient is 4"), Ingredients[1]->Count, 4);
		TestEqual(TEXT("Find and Reduce Ingredients multi pass: Third match ingredient is 0"), Ingredients[2]->Count, 0);
	}

	//Find and Reduce Ingredients multi pass, no negatives
	{
		TArray<UIngredientInstance*> Ingredients =
		{
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 3),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 10),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 3)
		};

		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);
		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);
		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);
		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);

		TestEqual(TEXT("Find and Reduce Ingredients multi pass, no negatives: First Match ingredient is 0"), Ingredients[0]->Count, 0);
		TestEqual(TEXT("Find and Reduce Ingredients multi pass, no negatives: Second Match ingredient is 1"), Ingredients[1]->Count, 1);
		TestEqual(TEXT("Find and Reduce Ingredients multi pass, no negatives: Third match ingredient is 0"), Ingredients[2]->Count, 0);
	}

	//Find and Reduce Ingredients multi pass, reduces next available
	{
		TArray<UIngredientInstance*> Ingredients =
		{
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 2),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 10),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 3)
		};

		FFindAndReduce()(Ingredients, TestRecipe->Ingredients, false);

		TestEqual(TEXT("Find and Reduce Ingredients multi pass, reduces next available: First Match ingredient is 0"), Ingredients[0]->Count, 0);
		TestEqual(TEXT("Find and Reduce Ingredients multi pass, reduces next available: Second Match ingredient is 9"), Ingredients[1]->Count, 9);
		TestEqual(TEXT("Find and Reduce Ingredients multi pass, reduces next available: Third match ingredient is 2"), Ingredients[2]->Count, 2);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRecipeMakerCounterAlgorithmTests, "Recipes.RecipeMakerComponent Counter Tests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)
bool FRecipeMakerCounterAlgorithmTests::RunTest(const FString& Parameters)
{
	UIngredient* TestIngredient_One = NewObject<UIngredient>();
	TestIngredient_One->Name = NSLOCTEXT("RecipeTests", "IngredientOne_Name", "Apple");
	UIngredient* TestIngredient_Two = NewObject<UIngredient>();
	TestIngredient_Two->Name = NSLOCTEXT("RecipeTests", "IngredientTwo_Name", "Chewed Bread");
	URecipe* TestRecipe = NewObject<URecipe>();
	TestRecipe->Ingredients =
	{
		UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 3),
		UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 1),
		UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 1)
	};

	//Basic value checks.
	{
		TArray<UIngredientInstance*> Ingredients =
		{
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 1),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 10),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 6)
		};

		TestTrue(TEXT("Should have enough ingredients for one recipe"), FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount()(Ingredients, TestRecipe->Ingredients, 1));
		TestTrue(TEXT("Should have enough ingredients for two recipes"), FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount()(Ingredients, TestRecipe->Ingredients, 2));
		TestTrue(TEXT("Should have enough ingredients for three recipes"), FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount()(Ingredients, TestRecipe->Ingredients, 3));
		TestFalse(TEXT("Should not have enough ingredients for four recipe"), FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount()(Ingredients, TestRecipe->Ingredients, 4));
	}

	//Calls delegate with correct data
	{
		TArray<UIngredientInstance*> Ingredients =
		{
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 1),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 1),
			UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_Two, 99)
		};

		FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount()(Ingredients, TestRecipe->Ingredients, 1, FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount::FNeedMoreNotify::CreateLambda([this](FText Name, int32 Needed, int32 Have)
		{
			TestEqual(TEXT("Name of missing ingredients should be Apple"), Name.ToString(), FString(TEXT("Apple")));
			TestEqual(TEXT("Should need 3 Apples"), Needed, 3);
			TestEqual(TEXT("Should have 2 Apples"), Have, 2);
		}));

		FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount()(Ingredients, TestRecipe->Ingredients, 5, FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount::FNeedMoreNotify::CreateLambda([this](FText Name, int32 Needed, int32 Have)
		{
			TestEqual(TEXT("Name of missing ingredients should be Apple"), Name.ToString(), FString(TEXT("Apple")));
			TestEqual(TEXT("Should need 15 Apples"), Needed, 15);
			TestEqual(TEXT("Should have 2 Apples"), Have, 2);
		}));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRecipeMakerAddCountInstanceAlgorithmTests, "Recipes.RecipeMakerComponent Adding Count and Instances Tests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)
bool FRecipeMakerAddCountInstanceAlgorithmTests::RunTest(const FString& Parameters)
{
	UIngredient* TestIngredient_One = NewObject<UIngredient>();
	TestIngredient_One->Name = NSLOCTEXT("RecipeTests", "IngredientOne_Name", "Apple");
	TArray<UIngredientInstance*> Ingredients =
	{
		UIngredientInstance::FromIngredient(GetTransientPackage(), TestIngredient_One, 8)
	};
	FAddCountAndIngredients()(((UObject*)GetTransientPackage()), Ingredients, Ingredients[0], TestIngredient_One, 91, 10, UIngredientInstance::StaticClass());

	TestEqual(TEXT("Should have 10 instances"), Ingredients.Num(), 10);
	TestEqual(TEXT("Should have 10 instances of the same name"), Ingredients.FilterByPredicate(FFindByNameCaseIgnored(TestIngredient_One->Name)).Num(), 10);
	TestEqual(TEXT("Ingredient at Index 0 should have maxed out at 10"), Ingredients[0]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 1 should have maxed out at 10"), Ingredients[1]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 2 should have maxed out at 10"), Ingredients[2]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 3 should have maxed out at 10"), Ingredients[3]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 4 should have maxed out at 10"), Ingredients[4]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 5 should have maxed out at 10"), Ingredients[5]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 6 should have maxed out at 10"), Ingredients[6]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 7 should have maxed out at 10"), Ingredients[7]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 8 should have maxed out at 10"), Ingredients[8]->Count, 10);
	TestEqual(TEXT("Ingredient at Index 9 should have stopped at 9"), Ingredients[9]->Count, 9);

	return true;
}
//**End Helper Algorithm Automation Tests**//
//**End Helper Algorithms**//

// Sets default values for this component's properties
URecipeMakerComponent::URecipeMakerComponent() :
	DefaultIngredientClass(UIngredientInstance::StaticClass()),
	bRemoveIngredientOnZeroCount(false)
{
	// DEPRECATED (unused, inconsistent enforcement) bWantsBeginPlay = true;
	bAutoActivate = true;
	SetIsReplicated(true);
	SetNetAddressable();
}

void URecipeMakerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (GetOwner()->HasAuthority() || IsNetMode(NM_DedicatedServer))
	{
		for (auto Recipe : Recipes)
		{
			Client_AddRecipe(Recipe);
		}
	}
}

void URecipeMakerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(URecipeMakerComponent, Ingredients, COND_OwnerOnly);
}

bool URecipeMakerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	if (Channel && Bunch && RepFlags)
	{
		bWroteSomething |= Channel->ReplicateSubobjectList(Ingredients, *Bunch, *RepFlags);
	}
	return bWroteSomething;
}

bool URecipeMakerComponent::HasRecipe(URecipe* Recipe) const
{
	return Recipes.Contains(Recipe);
}

bool URecipeMakerComponent::CanMakeRecipeInternal_Implementation(URecipe* Recipe, int32 DesiredCount) const
{
	return HasRecipe(Recipe) && HasAllIngredientsForRecipe(Recipe, DesiredCount);
}

bool URecipeMakerComponent::CanMakeRecipe(URecipe* Recipe, int32 DesiredCount) const
{
	return CanMakeRecipeInternal(Recipe, DesiredCount);
}

bool URecipeMakerComponent::HasAllIngredientsForRecipe(URecipe* Recipe, int32 DesiredCount) const
{
	if (Recipe)
	{
		return FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount()(Ingredients, Recipe->Ingredients, DesiredCount, FIngredientCountIsGreaterThanOrEqualToRecipeIngredientCount::FNeedMoreNotify::CreateLambda([this](FText Name, int32 Needed, int32 Have)
		{
			OnMissingIngredients(Name, Needed, Have);
		}));
	}
	else
	{
		return false;
	}
}

void URecipeMakerComponent::MakeRecipe(URecipe* Recipe, int32 DesiredCount)
{
	if (CanMakeRecipe(Recipe, DesiredCount))
	{
		if (GetOwner()->HasAuthority() || IsNetMode(NM_DedicatedServer))
		{
			TArrayBuilder<UObject*> CraftedItems;
			for (int32 i = 0; i < DesiredCount; ++i)
			{
				FFindAndReduce()(Ingredients, Recipe->Ingredients, bRemoveIngredientOnZeroCount);
				CraftedItems.Append(Recipe->Craft(GetOwner()->GetInstigatorController(), GetOwner()));
			}
			OnItemsCrafted(CraftedItems.Build());
		}
		else
		{
			Server_MakeRecipe(Recipe, DesiredCount);
		}
	}
}

void URecipeMakerComponent::AddRecipe(URecipe* NewRecipe)
{
	if (GetOwner()->HasAuthority() || IsNetMode(NM_DedicatedServer))
	{
		Client_AddRecipe(NewRecipe);
		OnRecipeAdded(NewRecipe);
	}
	else
	{
		Server_AddRecipe(NewRecipe);
	}
}

void URecipeMakerComponent::AddIngredient(const UIngredient* NewIngredients, int32 Count)
{
	if (GetOwner()->HasAuthority() || IsNetMode(NM_DedicatedServer))
	{
		UIngredientInstance** WasFound = Ingredients.FindByPredicate(FFindByNameCaseIgnored(NewIngredients->Name));
		if (WasFound)
		{
			(*WasFound)->Count += Count;
		}
		else
		{
			AddNewIngredient(NewIngredients, Count);
		}
		OnIngredientsAdded(NewIngredients, Count);
	}
	else
	{
		Server_AddIngredient(NewIngredients, Count);
	}
}

void URecipeMakerComponent::RemoveIngredient(UIngredientInstance* Ingredient)
{
	if (GetOwner()->HasAuthority() || IsNetMode(NM_DedicatedServer))
	{
		Ingredients.Remove(Ingredient);
	}
	else
	{
		Server_RemoveIngredient(Ingredient);
	}
}

void URecipeMakerComponent::ReduceIngredient(const UIngredient* Ingredient, int32 Count)
{
	if (GetOwner()->HasAuthority() || IsNetMode(NM_DedicatedServer))
	{
		TArray<UIngredientInstance*> Temp = { UIngredientInstance::FromIngredient(((UObject*)GetTransientPackage()), Ingredient, Count, DefaultIngredientClass) };
		FFindAndReduce()(Ingredients, Temp, bRemoveIngredientOnZeroCount);
		Temp.Reset();
	}
	else
	{
		Server_ReduceIngredient(Ingredient, Count);
	}
}

void URecipeMakerComponent::Server_MakeRecipe_Implementation(URecipe* Recipe, int32 DesiredCount)
{
	MakeRecipe(Recipe, DesiredCount);
}

bool URecipeMakerComponent::Server_MakeRecipe_Validate(URecipe* Recipe, int32 DesiredCount) const
{
	return CanMakeRecipe(Recipe, DesiredCount);
}

void URecipeMakerComponent::Server_AddRecipe_Implementation(URecipe* NewRecipe)
{
	AddRecipe(NewRecipe);
}

bool URecipeMakerComponent::Server_AddRecipe_Validate(URecipe* NewRecipe) const
{
	return NewRecipe != nullptr;
}

void URecipeMakerComponent::Server_AddIngredient_Implementation(const UIngredient* NewIngredients, int32 Count)
{
	AddIngredient(NewIngredients, Count);
}

bool URecipeMakerComponent::Server_AddIngredient_Validate(const UIngredient* NewIngredients, int32 Count) const
{
	return NewIngredients != nullptr && Count >= 0;
}

void URecipeMakerComponent::Server_RemoveIngredient_Implementation(UIngredientInstance* Ingredient)
{
	RemoveIngredient(Ingredient);
}

bool URecipeMakerComponent::Server_RemoveIngredient_Validate(UIngredientInstance* Ingredient) const
{
	return Ingredient != nullptr;
}

void URecipeMakerComponent::Server_ReduceIngredient_Implementation(const UIngredient* Ingredient, int32 Count)
{
	ReduceIngredient(Ingredient, Count);
}

bool URecipeMakerComponent::Server_ReduceIngredient_Validate(const UIngredient* Ingredient, int32 Count) const
{
	return Ingredient != nullptr && Count >= 0;
}

void URecipeMakerComponent::Client_AddRecipe_Implementation(class URecipe* NewRecipe)
{
	Recipes.AddUnique(NewRecipe);
}

void URecipeMakerComponent::AddNewIngredient(const UIngredient* NewIngredients, int32 Count)
{
	UIngredientInstance* NewIngredient = UIngredientInstance::FromIngredient(this, NewIngredients, Count, DefaultIngredientClass);
	if (NewIngredient)
	{
		Ingredients.Add(NewIngredient);
		Ingredients.Sort(FSortByNameCaseIgnored());
	}
}