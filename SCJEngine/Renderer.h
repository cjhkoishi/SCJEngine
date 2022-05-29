#pragma once
#include "Component.h"
class Renderer :
    public Component
{
public:
    void update() override;
    virtual void render()=0;
};

class RendererSystem
{

};

extern RendererSystem renderer_system;

