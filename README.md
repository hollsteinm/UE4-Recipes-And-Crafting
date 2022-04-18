# UE4-Recipes-And-Crafting

A UE4 Plugin that enables you to use data assets to add crafting and recipes into your game.

## Usage

Create data assets in your UE4 project via the Editor. The assets can be derived for *UIngredient* and *URecipe*. A *URecipe* can have a list of *UIngredient* subclass types and values related to how many of each of those items are needed. You can interact with this data by creating the appropriate factory classes. There are also blueprint library function helpers for interaction. There is a component class that is used as a sort of inventory manager and crafting logic component. All of the instances of objects and the component are replicated and support interactions via RPC.

## More Help

For more information feel free to [join our discord](https://discord.gg/bQ47YbF)