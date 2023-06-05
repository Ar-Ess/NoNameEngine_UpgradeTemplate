#pragma once

enum WaterBufferType
{
    WBT_REFLECTION,
    WBT_REFRACTION
};

struct WaterBuffer
{
    // Check WaterBufferType to know which is reflection/refraction

    GLuint albedoAttachHandle[2] = {};
    GLuint depthAttachHandle[2] = {};

    GLuint handle[2] = {};
};
