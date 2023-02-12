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

    /**
     * Get game object in level.
     *
     * \param id
     * \return
     */
    std::weak_ptr<GameObject> GetGObjectByID(GameObjectID id) const;
    /**
     * Create game object that put into level next frame.
     *
     * \param out_game_object
     * \return
     */
    GameObjectID CreateGObject(std::shared_ptr<GameObject>* out_game_object = nullptr);
    /**
     * Create game object that put into level next frame.
     *
     * \param object_instance_res
     * \return
     */
    GameObjectID CreateGObject(const ObjectInstanceResource& object_instance_res);
    /**
     * Remove game object next frame.
     *
     * \param id
     */
    void         DeleteGObjectByID(GameObjectID id);

private:
    void PushGObjectToAdd(std::shared_ptr<GameObject> game_object);
    void PushGObjectToRemove(std::shared_ptr<GameObject> game_object);

    void Clear();

    bool IsLoaded {false};

    String LevelResourceUrl;

    LevelObjectsMap GameObjects;

    std::vector<std::shared_ptr<GameObject>> NewObjects;
    std::vector<std::shared_ptr<GameObject>> ExpiredObjects;
};
