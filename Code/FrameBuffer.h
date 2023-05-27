#pragma once

struct FrameBuffer
{
    GLuint albedoAttachHandle = 0;
    GLuint specularAttachHandle = 0;
    GLuint normalsAttachHandle = 0;
    GLuint positionAttachHandle = 0;
    GLuint depthAttachHandle = 0;
    GLuint lightAttachHandle = 0;
    GLuint bloomAttachHandle = 0;

    GLuint finalAttachHandle = 0;

    GLuint handle = 0;
};
