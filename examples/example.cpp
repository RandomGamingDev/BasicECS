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
