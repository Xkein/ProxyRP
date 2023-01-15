#include "level.h"
#include "core/macro.h"
#include "core/log/log_system.h"
#include "resource/res_type/common/object.h"

Level::~Level() {}

bool Level::Load(const String& level_res_url)
{
    LOG_INFO("load level: ", level_res_url);
    return true;
}

void Level::Unload()
{
    Clear();
    LOG_INFO("unload level");
}

void Level::Tick(float delta_time)
{
    for (const auto& [id, game_object] : GameObjects)
    {
        game_object->Tick(delta_time);
    }

}

std::weak_ptr<GameObject> Level::GetGObjectByID(GameObjectID id) const
{
    if (auto iter = GameObjects.find(id); iter != GameObjects.end())
    {
        return iter->second;
    }

    return std::weak_ptr<GameObject>();
}

GameObjectID Level::CreateGObject()
{
    GameObjectID object_id = ObjectIDAllocator::Alloc();
    ASSERT(object_id != InvalidGObjectID);

    std::shared_ptr<GameObject> game_object = std::make_shared<GameObject>(object_id);
    GameObjects.emplace(object_id, game_object);

    return object_id;
}

GameObjectID Level::CreateGObject(const ObjectInstanceResource& object_instance_res)
{
    GameObjectID object_id = ObjectIDAllocator::Alloc();
    ASSERT(object_id != InvalidGObjectID);

    std::shared_ptr<GameObject> game_object = std::make_shared<GameObject>(object_id);

    if (const bool is_loaded = game_object->Load(object_instance_res); !is_loaded)
    {
        LOG_ERROR("loading object {} failed", object_instance_res.Name);
        return InvalidGObjectID;
    }

    GameObjects.emplace(object_id, game_object);

    return object_id;
}

void Level::DeleteGObjectByID(GameObjectID id)
{
    if (auto iter = GameObjects.find(id); iter != GameObjects.end())
    {


        GameObjects.erase(iter);
    }
}

void Level::Clear()
{
    GameObjects.clear();
}
