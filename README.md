# BasicECS
A basic ECS system, with SceneManagers, Scenes and GameObjects with their own Start and Update functions. 

Simply spawn a `SceneManager` with an Array of `Scene`s and the integer/id pointing to the active scene. Create `Scene`s by specifying a `Linked::List` of `GameObject`s and then specifying the `sceneStart` and `sceneUpdate` functions. Then just push whatever `GameObject`s you want to the `Linked::List` used to store all the GameObjects by the `Scene`. When creating the `GameObject` just specify its `start` and `update` functions. The way the system works is that they all have their own `start` and `update` functions, with the calls cascading down from the `SceneManager` to its `Scene`s to its `GameObject`s allowing for a easy way to manage your game and a easy framework to build on top of.

This library depends on the Linked List implementation from https://github.com/RandomGamingDev/OneWayLinkedListLib. To use it you can simply download the header file from the github repo and place it next to BasicECS.

To add things like variables to any of the classes: `SceneManager`s, `Scene`s, and `GameObject`s you can just use C++ inheritance, to add what are basically components allowing for an easy and nice to manage system for managing your code!
