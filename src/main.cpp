
#include <memory>
#include <filesystem>

#include "engine.h"
#include "core/macro.h"

int main(int argc, char** argv)
{
    std::filesystem::path exe_path = argv[0];
    std::filesystem::path config_dir = exe_path.parent_path() / ENGINE_NAME;

    std::shared_ptr<Engine> engine = std::make_shared<Engine>();

    engine->Start(config_dir.string());

    engine->Run();

    engine->Exit();

    return 0;
}
