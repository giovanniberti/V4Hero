#include <filesystem>
#include <iostream>
#include "Engine/V4Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;
namespace fs = std::filesystem;

#ifdef __linux__
#include <X11/Xlib.h>
void setupPlatform() {
    XInitThreads();
}
#else
void setupPlatform() {}
#endif

int main(int argc, char *argv[])
{
    spdlog::cfg::load_env_levels();
    auto rhythm_logger = spdlog::stdout_color_mt("rhythm");

    std::cout << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << std::endl;
    setupPlatform();

    cout << "V4" << endl;
    auto executable_path = weakly_canonical(fs::path(argv[0]));
    cout << "my directory is " << executable_path.parent_path() << "\n";
    //_setmode(_fileno(stdout), _O_U16TEXT);
    //wchar_t * unicode_text = L"aäbcdefghijklmnoöpqrsßtuüvwxyz";
    //wprintf(L"%s", unicode_text);

    fs::current_path(executable_path.parent_path());

    V4Core V4;
    V4.exePath = executable_path;
    V4.init();

    return 0;
}
