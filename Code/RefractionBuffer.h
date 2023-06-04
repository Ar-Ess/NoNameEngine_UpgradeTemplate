#pragma once

struct RefractionBuffer
{
    GLuint albedoAttachHandle = 0;
    GLuint depthAttachHandle = 0;

    GLuint finalAttachHandle = 0;

    GLuint handle = 0;
};
