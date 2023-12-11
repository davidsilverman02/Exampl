#pragma once

#include <stdlib.h>
#include <iostream>
#include <SDL.h>
#include<string>

using namespace std;

namespace gpr460
{
	enum IDs
	{
		other = 0, rRenderer = 1, rCollider = 2, controller = 3, rChecker = 4,
	};
	class GameObject;
	class Transform;

	class Component
	{
	public:
		virtual void Update();
		virtual void Render(SDL_Renderer* renderer);
		IDs kCompID;
	};

	struct Vector3
	{
		float x;
		float y;
		float z;
		Vector3()
		{

		};
		Vector3(int setX, int setY)
		{
			x = setX;
			y = setY;
		};
		Vector3(int setX, int setY, int setZ)
		{
			x = setX;
			y = setY;
			z = setZ;
		};
		Vector3 operator=(const Transform& item);
		float flooredVal(float bec);
	};

	class Transform : public Component 
	{
	public:
		enum {kCompID = 0};
		GameObject* obj;
		Transform();
		Transform(float newX, float newY);
		Transform(float newX, float newY, float newZ);
		void Move(float x, float y, float z);
		Vector3 pos;
	};
	// gameObject->rectRenderer->color = blahblah
	// add constructors
	class RectangleRenderer : public Component
	{
	public:
		enum { kCompID = 1 };
		GameObject* obj;
		RectangleRenderer();
		RectangleRenderer(float newWidth, float newHeight, float r, float g, float b);
		float width;
		float height;
		Vector3 color;
		void Render(SDL_Renderer* renderer) override;
	};

	class RectangleCollider : public Component
	{
	public:
		enum { kCompID = 2 };
		GameObject* obj;
		RectangleCollider();
		bool isColliding;
		bool CheckCollision(RectangleCollider* other);
		bool isInside(Vector3 other);
	};

	class PlayerController : public Component
	{
	public:
		enum { kCompID = 3 };
		//SDL_Event evn;
		GameObject* obj;
		PlayerController();
		int PlayerKey(int keyDown);
		void InputMovement(SDL_Event evn, bool isDown);
		void Update() override;
	private:
		Uint8* keysLastFrame;
		Uint8* keysThisFrame;
		float movX;
		float movY;
		int numkeys;
	};

	class ColliderColorChanger : public Component
	{
	public:
		enum { kCompID = 4 };
		GameObject* obj;
		ColliderColorChanger();
		ColliderColorChanger(float r, float g, float b);
		Vector3 colColor;
		Vector3 nonColColor;
		void Update() override;
	};

	class GameObject
	{
	public:
		GameObject()
		{
			isActive = false;
		}
		void Move(float x, float y, float z);
		void SetX(float x);
		void SetY(float y);
		void SetZ(float z);
		RectangleRenderer* CreateRenderer(...);
		RectangleCollider* CreateCollider(...);
		PlayerController* CreatePlayerController(...);
		ColliderColorChanger* CreateColliderColorChanger(...);
		Transform* CreateTransform();

		Transform* getTransform();
		RectangleRenderer* GetRenderer(...);
		RectangleCollider* GetCollider(...);
		PlayerController* GetPlayerController(...);
		ColliderColorChanger* GetColliderColorChanger(...);
	public:
		bool isActive;
		bool isPlayer;
		Transform* transform;
		RectangleRenderer* renderer;
		RectangleCollider* collider;
		PlayerController* player;
		ColliderColorChanger* colorChanger;
		string name;
	};
}