#pragma once

class Camera
{
public:

    Camera(glm::vec3 position, float aspectRatio, float znear, float zfar)
    {
        this->aspectRatio = aspectRatio;
        this->znear = znear;
        this->zfar = zfar;
        this->position = position;
        this->target = glm::vec3(1.0f);

        this->projection = glm::perspective(glm::radians(60.f), aspectRatio, znear, zfar);
        this->view = glm::lookAt(position, target, glm::vec3(0, 1, 0));
    }

    void Update()
    {
        view = glm::lookAt(position, target, glm::vec3(0, 1, 0));
    }

    glm::vec3 Position() const
    {
        return position;
    }

    void Translate(float side, float y, float zoom)
    {
        glm::vec3 forward = Forward();
        glm::vec3 right = Right();
        glm::vec3 up = Up();

        glm::vec3 z = zoom * forward;
        glm::vec3 s = side * right;
        glm::vec3 u = y * up;

        position += z + s + u;
        target += z + s + u;
    }
    void Translate(glm::vec3 delta)
    {
        glm::vec3 forward = Forward();
        glm::vec3 right = Right();
        glm::vec3 up = Up();

        glm::vec3 z = delta.z * forward;
        glm::vec3 s = delta.x * right;
        glm::vec3 u = delta.y * up;

        position += z + s + u;
        target += z + s + u;
    }

    void LookAt(glm::vec2 delta, float velocity)
    {
        if (glm::abs(delta.x) + glm::abs(delta.y) < 0.0001f) return;
        target.x += delta.x * velocity;
        target.y += delta.y * -velocity;
    }

    glm::vec3 Forward() const
    {
        return glm::normalize(glm::vec3(target - position));
    }

    glm::vec3 Right() const
    {
        return glm::cross(Forward(), Up());
    }

    glm::vec3 Up() const
    {
        return glm::vec3(view[1].x, view[1].y, view[1].z);
    }

private:

    friend struct App;

    glm::mat4 projection; // const
    glm::mat4 view;

    glm::vec3 position;
    glm::vec3 target;

    float aspectRatio = 0;
    float znear = 0;
    float zfar = 0;

};
