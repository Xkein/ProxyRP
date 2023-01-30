#pragma once

#include "function/framework/component/component.h"
#include "function/render/render_object.h"
#include "resource/res_type/data/mesh.h"

CLASS(MeshComponent : public Component)
{
    REFLECTION_CLASS_BODY(MeshComponent);

public:
    virtual void PostLoadResource(std::weak_ptr<GameObject> parent_object) override;

    virtual void Tick(float delta_time) override;

private:
    PROPERTY()
    MeshComponentResource MeshRes;

    std::vector<GameObjectPartDesc> RawMeshes;
};