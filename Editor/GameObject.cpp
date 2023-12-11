#include "GameObject.h"

using namespace gpr460;

void GameObject::Move(float x, float y, float z)
{
	transform->pos.x += x;
	transform->pos.y += y;
	transform->pos.z += z;
}

void GameObject::SetX(float x)
{
	transform->pos.x = x;
}

void GameObject::SetY(float y)
{
	transform->pos.y = y;
}

void GameObject::SetZ(float z)
{
	transform->pos.z = z;
}

void Component::Update()
{

}

void Component::Render(SDL_Renderer* renderer)
{

}

Transform* GameObject::getTransform()
{
	return transform;
}

RectangleRenderer* GameObject::GetRenderer(...)
{
	return renderer;
}

RectangleCollider* GameObject::GetCollider(...)
{
	return collider;
}

PlayerController* GameObject::GetPlayerController(...)
{
	return player;
}

ColliderColorChanger* GameObject::GetColliderColorChanger(...)
{
	return colorChanger;
}

RectangleRenderer* GameObject::CreateRenderer(...)
{
	renderer = new RectangleRenderer;
	return renderer;
}

RectangleCollider* GameObject::CreateCollider(...)
{
	collider = new RectangleCollider;
	return collider;
}

PlayerController* GameObject::CreatePlayerController(...)
{
	player = new PlayerController;
	return player;
}

ColliderColorChanger* GameObject::CreateColliderColorChanger(...)
{
	colorChanger = new ColliderColorChanger;
	return colorChanger;
}

Transform* GameObject::CreateTransform()
{
	transform = new Transform;
	return transform;
}

Transform::Transform()
{
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
}

Transform::Transform(float newX, float newY)
{
	pos.x = newX;
	pos.y = newY;
	pos.z = 0;
}

Transform::Transform(float newX, float newY, float newZ)
{
	pos.x = newX;
	pos.y = newY;
	pos.z = newZ;
}

void Transform::Move(float x, float y, float z)
{
	pos.x += x;
	pos.y += y;
	pos.z += z;
}

Vector3 Vector3::operator=(const Transform& item)
{
	Vector3 vec;
	vec.x = vec.flooredVal(item.pos.x);
	vec.y = vec.flooredVal(item.pos.y);
	vec.z = vec.flooredVal(item.pos.z);
	return vec;
}

float Vector3::flooredVal(float bec)
{
	if (bec > 255)
		return 255;

	if (bec < 0)
		return 0;
	
	return bec;
}

RectangleRenderer::RectangleRenderer()
{
	width = 20;
	height = 20;
	color.x = 0;
	color.y = 0;
	color.z = 0;
}

RectangleRenderer::RectangleRenderer(float newWidth, float newHeight, float r, float g, float b)
{
	width = newWidth;
	height = newHeight;
	color.x = color.flooredVal(r);
	color.y = color.flooredVal(g);
	color.z = color.flooredVal(b);
}

void RectangleRenderer::Render(SDL_Renderer* renderer)
{
	SDL_Rect r = {
			obj->transform->pos.x - (width/2.0),
			obj->transform->pos.y - (height / 2.0),
			width,
			height
	};

	SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &r);
}

RectangleCollider::RectangleCollider()
{

}

bool RectangleCollider::CheckCollision(RectangleCollider* other)
{
	float myWidth = 0;
	if(obj->GetRenderer() != NULL)
		myWidth = (obj->GetRenderer()->width) / 2.0f;

	float myHeight = (obj->GetRenderer()->height) / 2.0f;

	Vector3 myForm;
	myForm.x = 0;
	myForm.y = 0;
	myForm.z = 0;

	if(obj->getTransform() != NULL)
		myForm = obj->getTransform()->pos;

	float otherWidth = (other->obj->GetRenderer()->width) / 2.0f;
	float otherHeight = (other->obj->GetRenderer()->height) / 2.0f;

	Vector3 otherForm;
	otherForm.x = 0;
	otherForm.y = 0;
	otherForm.z = 0;

	if (obj->getTransform() != NULL)
		myForm = other->obj->getTransform()->pos;
	

	//(myForm.x + myWidth >= otherForm.x - otherWidth && myForm.x + myWidth <= otherForm.x + otherWidth) || (myForm.x - myWidth >= otherForm.x - otherWidth && myForm.x - myWidth <= otherForm.x + otherWidth)
	if (((myForm.x + myWidth >= otherForm.x - otherWidth && myForm.x + myWidth <= otherForm.x + otherWidth) || (myForm.x - myWidth >= otherForm.x - otherWidth && myForm.x - myWidth <= otherForm.x + otherWidth)) && ((myForm.y + myHeight >= otherForm.y - otherHeight && myForm.y + myHeight <= otherForm.y + otherHeight) || (myForm.y - myHeight >= otherForm.y - otherHeight && myForm.y - myHeight <= otherForm.y + otherHeight)))
		return true;
	return false;
}

bool RectangleCollider::isInside(Vector3 other)
{
	Vector3 bas;
	bas.x = 0;
	bas.y = 0;
	bas.z = 0;

	if(obj->getTransform() != NULL)
		bas = obj->getTransform()->pos;

	if (bas.x <= other.x && bas.y <= other.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

PlayerController::PlayerController()
{
	const Uint8* currentKeys = SDL_GetKeyboardState(&numkeys);

	keysLastFrame = new Uint8[numkeys];
	keysThisFrame = new Uint8[numkeys];

	memcpy(keysLastFrame, currentKeys, numkeys);
	memcpy(keysThisFrame, currentKeys, numkeys);
}

int PlayerController::PlayerKey(int keyDown)
{
	if (keyDown < numkeys)
	{
		return keysThisFrame[keyDown];
	}

	// Really, this is an error...
	return -1;
}

void PlayerController::InputMovement(SDL_Event evn, bool isDown)
{
	if (isDown)
	{
		if (evn.key.keysym.sym == SDLK_UP)
		{
			movY = -1;
		}
		else if (evn.key.keysym.sym == SDLK_DOWN)
		{
			movY = 1;
		}
		else if (evn.key.keysym.sym == SDLK_LEFT)
		{
			movX = -1;
		}
		else if (evn.key.keysym.sym == SDLK_RIGHT)
		{
			movX = 1;
		}
		else if (evn.key.keysym.sym == SDLK_l)
		{
			movX = 0;
			movY = 0;
		}
	}
	else
	{
		if (evn.key.keysym.sym == SDLK_UP || evn.key.keysym.sym == SDLK_DOWN)
		{
			movY = 0;
		}
		else if (evn.key.keysym.sym == SDLK_LEFT || evn.key.keysym.sym == SDLK_RIGHT)
		{
			movX = 0;
		}
	}
}

void PlayerController::Update()
{
	float moveSpd = 1;
	//movX = (PlayerKey(SDL_SCANCODE_RIGHT) - PlayerKey(SDL_SCANCODE_LEFT)) * moveSpd;
	//movY = (PlayerKey(SDL_SCANCODE_DOWN) - PlayerKey(SDL_SCANCODE_UP)) * moveSpd;

	obj->Move(movX * moveSpd, movY * moveSpd, 0);
	/*movX = 0;
	movY = 0;*/
}

ColliderColorChanger::ColliderColorChanger()
{
	//colColor = obj->GetRenderer()->color;
	//nonColColor = obj->GetRenderer()->color;
}

ColliderColorChanger::ColliderColorChanger(float r, float g, float b)
{
	colColor.x = colColor.flooredVal(r);
	colColor.y = colColor.flooredVal(g);
	colColor.z = colColor.flooredVal(b);
	nonColColor = obj->GetRenderer()->color;
}

void ColliderColorChanger::Update()
{
	if (obj->GetRenderer() != NULL)
	{
		nonColColor = obj->GetRenderer()->color;
		if (obj->GetCollider() != NULL)
		{
			if (obj->GetCollider()->isColliding)
				obj->GetRenderer()->color = colColor;
			else
				obj->GetRenderer()->color = nonColColor;
		}
	}
}