#pragma once

enum WaterBufferType
{
    WBT_REFLECTION,
    WBT_REFRACTION
};

struct WaterBuffer
{
    // Check WaterBufferType to know which is reflection/refraction

    GLuint depthAttachHandle[2] = {};
    GLuint albedoAttachHandle[2] = {};

    GLuint normalsTexIndex = 0;
    GLuint distortTexIndex = 0;

    GLuint handle[2] = {};
};
