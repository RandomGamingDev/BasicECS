#ifndef BASIC_ECS
#define BASIC_ECS

#include <cstddef>
#include <array>

#include "LinkedList.h"

class SceneManager;
class Scene;

class GameObject {
public:
	void (*start)(SceneManager&, Scene&, GameObject&);
	void (*update)(SceneManager&, Scene&, GameObject&);

	GameObject(
		void (*start)(SceneManager&, Scene&, GameObject&),
		void (*update)(SceneManager&, Scene&, GameObject&)
	) {
		this->start = start;
		this->update = update;
	}
};

class Scene {
public:
	Linked::List<GameObject*> objects;
	void (*startScene)(SceneManager&, Scene&);
	void (*updateScene)(SceneManager&, Scene&);

	Scene(
		Linked::List<GameObject*> objects,
		void (*startScene)(SceneManager&, Scene&),
		void (*updateScene)(SceneManager&, Scene&)
		)
	{
		this->objects = objects;
		this->startScene = startScene;
		this->updateScene = updateScene;
	}

	void startObjects(SceneManager& manager) {
		for (Linked::Node<GameObject*>* i = this->objects.head; i != nullptr; i = i->next)
			i->value->start(manager, *this, *i->value);
	}

	void updateObjects(SceneManager& manager) {
		for (Linked::Node<GameObject*>* i = this->objects.head; i != nullptr; i = i->next)
			i->value->update(manager, *this, *i->value);
	}

	void start(SceneManager& manager) {
		this->startScene(manager, *this);
		this->startObjects(manager);
	}

	void update(SceneManager& manager) {
		this->updateScene(manager, *this);
		this->updateObjects(manager);
	}
};

class SceneManager {
public:
	Scene** scenes;
	size_t currentScene;

	SceneManager(
		Scene** scenes,
		size_t currentScene
		)
	{
		this->scenes = scenes;
		this->currentScene = currentScene;
	}

	Scene& getCurrentScene() {
		return *this->scenes[this->currentScene];
	}

	void start() {
		this->getCurrentScene().start(*this);
	}

	void update() {
		this->getCurrentScene().update(*this);
	}
};

#endif