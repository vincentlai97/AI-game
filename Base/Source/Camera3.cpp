#include "Camera3.h"
#include "Mtx44.h"

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();

	// Initialise the camera movement flags
	for (int i = 0; i < 255; i++)
		myKeys[i] = false;
}

void Camera3::Update(double dt)
{
	int k = 'a';
	if(myKeys['w'])
	{
		MoveForward(dt);
		myKeys['w'] = false;
	}
	if(myKeys['a'])
	{
		MoveLeft(dt);
		myKeys['a'] = false;
	}
	if(myKeys['s'])
	{
		MoveBackward(dt);
		myKeys['s'] = false;
	}
	if(myKeys['d'])
	{
		MoveRight(dt);
		myKeys['d'] = false;
	}

}

void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

// Update Camera status
void Camera3::UpdateStatus(const unsigned char key)
{
	myKeys[key] = true;
}

void Camera3::MoveForward(const float dist)
{
	Vector3 view = (target - position);
	view.y = 0;
	view.Normalize();
	position += view * dist;
	target += view * dist;
}

void Camera3::MoveBackward(const float dist)
{
	Vector3 view = (target - position);
	view.y = 0;
	view.Normalize();
	position -= view * dist;
	target -= view * dist;
}

void Camera3::MoveLeft(const float dist)
{
	Vector3 view = (target - position);
	view.y = 0;
	view.Normalize();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	position -= right * dist;
	target -= right * dist;
}

void Camera3::MoveRight(const float dist)
{
	Vector3 view = (target - position);
	view.y = 0;
	view.Normalize();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	position += right * dist;
	target += right * dist;
}

void Camera3::LookUp(const float angle)
{
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	Mtx44 rotation;
	rotation.SetToRotation(-angle, right.x, right.y, right.z);
	view = (rotation * view).Normalized();
	target = position + view;
	up = right.Cross(view);
}

void Camera3::LookDown(const float angle)
{
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	Mtx44 rotation;
	rotation.SetToRotation(angle, right.x, right.y, right.z);
	view = (rotation * view).Normalized();
	target = position + view;
	up = right.Cross(view);
}

void Camera3::LookLeft(const float angle)
{
	Vector3 view = (target - position).Normalized();
	Mtx44 rotation;
	rotation.SetToRotation(-angle, 0, 1, 0);
	view = (rotation * view).Normalized();
	target = position + view;
	up = rotation * up;
}

void Camera3::LookRight(const float angle)
{
	Vector3 view = (target - position).Normalized();
	Mtx44 rotation;
	rotation.SetToRotation(angle, 0, 1, 0);
	view = (rotation * view).Normalized();
	target = position + view;
	up = rotation * up;
}