#pragma once

struct Vertex
{
    Vertex(float x, float y, float z, float uvx, float uvy)
    {
        this->pos = vec3(x, y, z);
        this->uv = vec2(uvx, uvy);
    }

    Vertex(vec3 pos, vec2 uv)
    {
        this->pos = pos;
        this->uv = uv;
    }

    vec3 pos;
    vec2 uv;
};