#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Camera.h"

class Camera3 : public Camera
{
public:
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Camera3();
	~Camera3();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();

	// Update Camera status
	virtual void UpdateStatus(const unsigned char key);

	virtual void MoveForward(const float dist);
	virtual void MoveBackward(const float dist);
	virtual void MoveLeft(const float dist);
	virtual void MoveRight(const float dist);

	virtual void LookUp(const float angle);
	virtual void LookDown(const float angle);
	virtual void LookLeft(const float angle);
	virtual void LookRight(const float angle);

private:
	bool myKeys[255];
};

#endif