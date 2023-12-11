#pragma once

#include "GameObject.h"

using namespace gpr460;

const size_t MAX_GAMEOBJECTS = 20;

template<typename T>
class ComponentPool
{
public:

	T components[MAX_GAMEOBJECTS];
	bool isActive[MAX_GAMEOBJECTS];

    ComponentPool()
    {
        for (int i = 0; i < MAX_GAMEOBJECTS; i++)
        {
            isActive[i] = false;
        }
    };

    void Update()
    {
        for (int i = 0; i < MAX_GAMEOBJECTS; i++)
        {
            if (isActive[i])
            {
                components[i].Update();
            }
        }
    };

    void Render(SDL_Renderer* renderer)
    {
        for (int i = 0; i < MAX_GAMEOBJECTS; i++)
        {
            if (isActive[i])
            {
                components[i].Render(renderer);
            }
        }
    };

    T* New(GameObject* go)
    {
        for (int i = 0; i < MAX_GAMEOBJECTS; i++)
        {
            if (!isActive[i])
            {
                T* next = &components[i];
                next->obj = go;
                isActive[i] = true;
                return next;
            }
        }
    };

    void Delete(GameObject* go)
    {
        for (int i = 0; i < MAX_GAMEOBJECTS; i++)
        {
            if (components[i].obj == go)
            {
                components[i].obj = NULL;
                isActive[i] = false;
            }
        }
    };
};