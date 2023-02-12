#include "level.h"
#include "core/macro.h"
#include "core/log/log_system.h"
#include "function/global/global_context.h"
#include "resource/asset/asset_manager.h"
#include "resource/res_type/common/object.h"
#include "resource/res_type/common/level.h"

Level::~Level() {}

bool Level::Load(const String& level_res_url)
{
    LOG_INFO("load level: {}", level_res_url);

    LevelResourceUrl = level_res_url;

    LevelResource level_res;
    const bool    is_load_success = GAssetManager->LoadAsset(level_res_url, level_res);
    if (!is_load_success)
    {
        return false;
    }

    for (const ObjectInstanceResource& object_instance_res : level_res.Objects)
    {
        CreateGObject(object_instance_res);
    }

    IsLoaded = true;

    LOG_INFO("{} loaded.", level_res_url);

    return true;
}

void Level::Unload()
{
    Clear();
    LOG_INFO("unload level");
}

void Level::Tick(float delta_time)
{
    for (auto& game_object : ExpiredObjects)
    {
        GameObjects.erase(game_object->GetID());
    }

    for (auto& game_object : NewObjects)
    {
        GameObjects.emplace(game_object->GetID(), game_object);
    }

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

GameObjectID Level::CreateGObject(std::shared_ptr<GameObject>* out_game_object)
{
    GameObjectID object_id = ObjectIDAllocator::Alloc();
    ASSERT(object_id != InvalidGObjectID);

    std::shared_ptr<GameObject> game_object = std::make_shared<GameObject>(object_id);
    if (out_game_object)
    {
        *out_game_object = game_object;
    }
    PushGObjectToAdd(std::move(game_object));

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

    PushGObjectToAdd(game_object);

    return object_id;
}

void Level::DeleteGObjectByID(GameObjectID id)
{
    if (auto iter = GameObjects.find(id); iter != GameObjects.end())
    {
        PushGObjectToRemove(iter->second);
    }
}

void Level::PushGObjectToAdd(std::shared_ptr<GameObject> game_object)
{
    NewObjects.emplace_back(std::move(game_object));
}

void Level::PushGObjectToRemove(std::shared_ptr<GameObject> game_object)
{
    ExpiredObjects.emplace_back(std::move(game_object));
}

void Level::Clear()
{
    GameObjects.clear();

    NewObjects.clear();
    ExpiredObjects.clear();
}
