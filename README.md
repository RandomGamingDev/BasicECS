# BasicECS
A basic ECS system, with SceneManagers, Scenes and GameObjects with their own Start and Update functions. 

There are example(s) of how to use the library in [`examples`](https://github.com/RandomGamingDev/BasicECS/blob/main/examples)

## Example
Here's the example from [`example.cpp`](https://github.com/RandomGamingDev/BasicECS/blob/main/examples/example.cpp)
```cpp
#include <iostream>
#include <array>

#include "BasicECS.h"

/* 
You oftentimes don't need to have classes other than the
gameobject know what type the gameobject is 
However, it can still be useful for certain cases in
which case you can create a class containing an enum
defining the type
*/
enum CGOType {
	Person
};

class ComplexGameObject : public GameObject {
public:
	CGOType type;

	ComplexGameObject(
		void (*start)(SceneManager&, Scene&, GameObject&),
		void (*update)(SceneManager&, Scene&, GameObject&),
		CGOType type
	) : GameObject(start, update) {
		this->type = type;
	}
};

class PersonObject : public ComplexGameObject {
public:
	int age;
	int height;

	PersonObject(
		void (*start)(SceneManager&, Scene&, GameObject&),
		void (*update)(SceneManager&, Scene&, GameObject&),
		int age,
		int height
	) : ComplexGameObject(start, update, CGOType::Person) {
		this->age = age;
		this->height = height;
	}

	void say_hello() {
		std::cout << "Hello!" << std::endl;
	}

	void say_age() {
		std::cout << "I'm " << age << " age old!" << std::endl;
	}

	void say_height() {
		std::cout << "I'm " << height << " feet tall!" << std::endl;
	}
};

int main() {
	// Create the scene(s)
	Scene** scenes = new Scene*[]{
		new Scene(Linked::List<GameObject*>(),
			[](SceneManager& sceneManager, Scene& scene) { // start
				std::cout << "start!" << std::endl;
			},
			[](SceneManager& sceneManager, Scene& scene) { // update
				std::cout << "update!" << std::endl;
			}
		)
	};

	// Create a person
	PersonObject* person = new PersonObject(
		[](SceneManager& sceneManager, Scene& scene, GameObject& gameObject) { // start()
			PersonObject& person = (PersonObject&)gameObject;
			person.say_hello();
			person.say_height();
		},
		[](SceneManager& sceneManager, Scene& scene, GameObject& gameObject) { // update()
			PersonObject& person = (PersonObject&)gameObject;
			person.say_age();
		},
		3, // age
		3 // height
	);

	// Add a person
	scenes[0]->objects.push((GameObject*)person);

	// Create the scene manager
	SceneManager sceneManager = SceneManager(scenes, 0);

	// Run the scene manager
	sceneManager.start();
	for (int i = 0; i < 3; i++)
		sceneManager.update();
}
```

## The Structure
Simply spawn a `SceneManager` with an Array of `Scene`s and the integer/id pointing to the active scene. Create `Scene`s by specifying a `Linked::List` of `GameObject`s and then specifying the `sceneStart` and `sceneUpdate` functions. Then just push whatever `GameObject`s you want to the `Linked::List` used to store all the GameObjects by the `Scene`. When creating the `GameObject` just specify its `start` and `update` functions. The way the system works is that they all have their own `start` and `update` functions, with the calls cascading down from the `SceneManager` to its `Scene`s to its `GameObject`s allowing for a easy way to manage your game and a easy framework to build on top of.
To add things like variables to any of the classes: `SceneManager`s, `Scene`s, and `GameObject`s you can just use C++ inheritance, to add what are basically components allowing for an easy and nice to manage system for managing your code!

## How to import
This library depends on the Linked List implementation from https://github.com/RandomGamingDev/OneWayLinkedListLib. To use it you can simply download the header file from the github repo and place it next to BasicECS.

## Addressing concerns about the efficiency
### Won't the Linked List based structure lead to majorly performance-degrading cache misses?
While it's true that linked are very well known for cache misses, this is oftentimes mitigated by a variety of factors.
- Batch Instantiation and Pooling: If you're optimizing for performance one thing you'll want to do is have specified times to spawn a ton of entities and pool them to save on CPU resources by not having to repeatedly reallocated and deallocate memory. By doing this (which is an expected portion of optimization) you'll already be drastically reducing the amount of cache misses.
- Not just a linked list: Because of how the system works (an array of pointers), you don't have to use just a linked list. The goal of the library is to be as dynamic as possible, which means that if you truly need the cache efficiency you can simply store the objects, or even just a part of the object on an array or a different data structure to, for instance, avoid cache misses.

### The Advantages of using a Linked List based system (we'll be comparing against sparse sets)
- Guaranteed linear order: Oftentimes you'll want objects to run things in a guaranteed order whether it be for execution or something else, while sparse sets don't guarantee a specific index order
- Being more dynamic: By using a linked list, it's a lot easier to integrate certain design patterns into the library
- Easy to understand: Linked lists are conceptually easy to understand and mess with
- Easy and dynamic parallelization: With sparse sets, removing and adding objects is expected to always operateon the end, since sparse sets would lose their advantage over linked lists otherwise. However, this can make instant and dynamic parallelization hard since this means that this process, at least for a singular sparse set is constrained to a single sparse set. A linked list solves this by allowing for O(1) performance in time efficiency for both adding objects and removing them, but anywhere in the list. This means that you can easily divide up a linked list by node and length based sections dynamically over multiple threads (the number of threads can differ over time), which would otherwise require a tremendous amouont of effort dividing up different pipelines, and components, which now requires only basic protection against race conditions. You can also save on memory by simply storing gameobjects in a certain order in the linked list, which sparse sets cannot reliably do. This also means that dividing up different components that manage different resources for different threads is not only easily possible with this linked list based system, but also much more memory efficient too.
- Dynamic memory usage for memory constrained systems: Where memory usage is more important oftentimes it's nicer to use only the memory that you need and not have to allocate more datastructures for more components. Of course, where that doesn't matter you can always use a separate datastructure like a sparse set alongside the linked list. There's also the advantage of not storing everything on sparse sets by default, which means that you can use the more memory expensive sparse sets only for the data that you need to access, improving the memory efficiency, performance, and general developer experience.
- Linear AND Sparse Set like performance: Reallocating large amounts of memory can oftentimes cause major lag spikes that can massively impede the exprience especially when done at a certain scale, and be even worse than a decently small decrease in performance like that of a linked list. By using a linked list based system you're allowed to more easily choose between both.
- It shouldn't matter that much with good coding practices like clustering(clustering things together into a singular gameobject): Lets say you have a screen made up of pixels. Should each pixel be a game object? Probably not. While we can hope to create a dynamic system to compensate for developer decisions like relying on the ECS for things that shouldn't rely on the ECS, oftentimes it just isn't worth it. For a lot of tasks it's more performant to, like with the screen analogy rely on a more specific system made for your use, rather than rely on the ECS. The ECS should manage dynamic and different game objects sure, but when dealing with large amounts of game objects that can have their performance better increased with a more specific systems, we should trust that developers will be able to choose those more specific decisions.
