#pragma once

class Camera
{
public:

    Camera(float aspectRatio, float znear, float zfar)
    {
        this->aspectRatio = aspectRatio;
        this->znear = znear;
        this->zfar = zfar;
        this->target = glm::vec3(0, 0, 0);

        this->projection = glm::perspective(glm::radians(60.f), aspectRatio, znear, zfar);
        this->view = glm::lookAt(glm::vec3(0, 0, 0), target, glm::vec3(0, 1, 0));
        this->world = glm::mat4(1);
    }

    glm::vec3 Position() const
    {
        return glm::vec3(world[0].w, world[1].w, world[2].w);
    }

    void Translate(float x, float y, float z)
    {
        world = glm::translate(world, glm::vec3(x, y, z));
    }
    void Translate(glm::vec3 delta)
    {
        world = glm::translate(world, delta);
    }

    void LookAt(glm::vec2 delta, float velocity)
    {
        //if (delta.x + delta.y < 0.0001f) return;
        //target.x += delta.x * velocity;
        //target.z += delta.y * velocity;

        //worldViewProjectionMatrix = glm::lookAt(Position(), target, glm::vec3(0, 1, 0));
    }

    const glm::f32* WorldPointerValue() const
    {
        return glm::value_ptr(world);
    }

    const glm::f32* GlobalPointerValue()
    {
        global = projection * view * world;
        return glm::value_ptr(global);
    }

private:

    glm::mat4 projection; // const
    glm::mat4 view;
    glm::mat4 world;
    glm::mat4 global;

    glm::vec3 target;
    float aspectRatio = 0;
    float znear = 0;
    float zfar = 0;

};
