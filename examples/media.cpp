#include "probe/media.h"

#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1'000);
#endif

    auto audio_sources = probe::media::audio_sources();

    std::cout << "Audio Sources: \n";
    for (const auto& audio : audio_sources) {
        std::cout << "    Name             : " << audio.name << '\n'
                  << "    ID               : " << audio.id << '\n'
                  << "    Description      : " << audio.description << '\n'
                  << "    Type             : " << probe::to_string(audio.type) << '\n'
                  << "    State            : " << probe::to_string(audio.state) << "\n\n";
    }

    auto audio_sinks = probe::media::audio_sinks();

    std::cout << "Audio Sinks: \n";
    for (const auto& audio : audio_sinks) {
        std::cout << "    Name             : " << audio.name << '\n'
                  << "    ID               : " << audio.id << '\n'
                  << "    Description      : " << audio.description << '\n'
                  << "    Type             : " << probe::to_string(audio.type) << '\n'
                  << "    State            : " << probe::to_string(audio.state) << "\n\n";
    }

    auto cameras = probe::media::cameras();

    std::cout << "Cameras: \n";
    for (const auto& cam : cameras) {
        std::cout << "    Name             : " << cam.name << '\n'
                  << "    ID               : " << cam.id << '\n'
                  << "    Description      : " << cam.description << '\n'
                  << "    Type             : " << probe::to_string(cam.type) << "\n\n";
    }
    return 0;
}