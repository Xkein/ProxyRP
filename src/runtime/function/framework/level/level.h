#pragma once

#include "function/framework/object/game_object.h"

#include <memory>
#include <unordered_map>

using LevelObjectsMap = std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>;

class Level
{
public:
    ~Level();

    bool Load(const String& level_res_url);
    void Unload();

    void Tick(float delta_time);

    const LevelObjectsMap GetAllGObjects() const
    {
        return GameObjects;
    }

    std::weak_ptr<GameObject> GetGObjectByID(GameObjectID id) const;
    
    GameObjectID CreateGObject();
    GameObjectID CreateGObject(const ObjectInstanceResource& object_instance_res);
    void         DeleteGObjectByID(GameObjectID id);

private:
    void Clear();

    bool IsLoaded {false};

    String LevelResourceUrl;

    LevelObjectsMap GameObjects;
};
