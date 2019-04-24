#pragma once

#include "OpenGL/glew.h"
#include "OpenGL/glm/glm.hpp"
#include "OpenGL/glm/gtc/matrix_transform.hpp"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UPLOOK,
	DOWNLOOK,
	LEFTLOOK,
	RIGHTLOOK,
	ZOOMIN,
	ZOOMOUT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat TSPEED = 30.0f;
const GLfloat RSPEED = 20.0f;
const GLfloat ZOOMSPEED = 1.0f;
const GLfloat FOV = 45.0f;

class Camera {
public:

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	GLfloat Yaw;
	GLfloat Pitch;
	GLfloat Fov;
	//ÉãÏñ»ú
	GLfloat translationSpeed;
	GLfloat rotationSpeed;
	GLfloat zoomSpeed;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH): Front(glm::vec3(0.0f, 0.0f, -1.0f)), translationSpeed(TSPEED), rotationSpeed(RSPEED), Fov(FOV), zoomSpeed(ZOOMSPEED)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
        this->updateCameraVectors();
	}

	Camera(glm::vec3 position, GLfloat frontX, GLfloat frontY, GLfloat frontZ) : WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), translationSpeed(TSPEED), rotationSpeed(RSPEED), Fov(FOV), zoomSpeed(ZOOMSPEED)
	{
		this->Position = position;
		this->Front = glm::normalize(glm::vec3(frontX, frontY, frontZ));
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
		this->Fov = 48.455;
		this->Pitch = glm::degrees(glm::asin(this->Front.y));
		this->Yaw = glm::degrees(glm::acos(this->Front.x / glm::cos(glm::radians(this->Pitch))));
		if (glm::sin(glm::radians(this->Yaw)) * glm::cos(glm::radians(this->Pitch)) * this->Front.z < 0)
			this->Yaw = - this->Yaw;
		//std::cout << this->Front.x << " " << this->Front.y << " " << this->Front.z << std::endl;
		//std::cout << "Yaw: " << this->Yaw << std::endl << "Pitch: " << this->Pitch << std::endl;
		this->updateCameraVectors();
		//std::cout << this->Front.x << " " << this->Front.y << " " << this->Front.z << std::endl;
		//std::cout << "Yaw: " << this->Yaw << std::endl << "Pitch: " << this->Pitch << std::endl;
	}

    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), translationSpeed(TSPEED), rotationSpeed(RSPEED), Fov(FOV), zoomSpeed(ZOOMSPEED)
    {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	void ProcessKeyboard(Camera_Movement movementName, GLfloat deltaTime)
	{
		GLfloat velocity1 = this->translationSpeed * deltaTime;
		if (movementName == FORWARD)
			this->Position += this->Front * velocity1;
		if (movementName == BACKWARD)
			this->Position -= this->Front * velocity1;
		if (movementName == LEFT)
			this->Position -= this->Right * velocity1;
		if (movementName == RIGHT)
			this->Position += this->Right * velocity1;

		GLfloat velocity2 = this->rotationSpeed * deltaTime;
		if (movementName == UPLOOK)
		{
			this->Pitch += velocity2;
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
			this->updateCameraVectors();
		}
		if (movementName == DOWNLOOK)
		{
			this->Pitch -= velocity2;
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
			this->updateCameraVectors();
		}
		if (movementName == LEFTLOOK)
		{
			this->Yaw += velocity2;
			this->updateCameraVectors();
		}
		if (movementName == RIGHTLOOK)
		{
			this->Yaw -= velocity2;
			this->updateCameraVectors();
		}

		GLfloat velocity3 = this->zoomSpeed * deltaTime;
		if (movementName == ZOOMIN)
		{
			if (this->Fov >= 40.0f && this->Fov <= 50.0f)
				this->Fov -= velocity3;
			if (this->Fov <= 40.0f)
				this->Fov = 40.0f;
			if (this->Fov >= 50.0f)
				this->Fov = 50.0f;
		}
		if (movementName == ZOOMOUT)
		{
			if (this->Fov >= 40.0f && this->Fov <= 50.0f)
				this->Fov += velocity3;
			if (this->Fov <= 40.0f)
				this->Fov = 40.0f;
			if (this->Fov >= 50.0f)
				this->Fov = 50.0f;
		}
	}

	void Update()
	{
		this->updateCameraVectors();
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);

		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));

		this->Position = -glm::sqrt(this->Position[0] * this->Position[0] + this->Position[1] * this->Position[1] + this->Position[2] * this->Position[2]) * this->Front;
		
		//std::cout << "camera pos: " << this->Position[0] << " " << this->Position[1] << " " << this->Position[2] << std::endl;
	}
};