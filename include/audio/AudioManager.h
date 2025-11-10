#pragma once
#include <string>
#include <iostream>
#include <map>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

class AudioManager {
private:
    std::map<std::string, std::string> soundFiles;
    bool menuMusicPlaying;
    bool gameMusicPlaying;
    bool soundsEnabled;

public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    void shutdown();

    void loadSound(const std::string& name, const std::string& filename);
    void playSound(const std::string& name);
    void playMenuMusic();
    void playGameMusic();
    void stopMenuMusic();
    void stopGameMusic();
    void stopAllMusic();

    void enableSounds(bool enable) { soundsEnabled = enable; }
    bool areSoundsEnabled() const { return soundsEnabled; }

private:
    void playMusicFile(const std::string& filename, bool loop);
    void stopMusic();
    void playWaveFile(const std::string& filename);
};