
#include <memory>
#include <filesystem>

#include "engine.h"
#include "core/macro.h"

int main(int argc, char** argv)
{
    getchar();
    std::filesystem::path exe_path   = argv[0];

    std::shared_ptr<Engine> engine = std::make_shared<Engine>();

    engine->Start();

    engine->Run();

    engine->Shutdown();

    return 0;
}
