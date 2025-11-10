#include "audio/AudioManager.h"
#include <iostream>

AudioManager::AudioManager() :
    menuMusicPlaying(false),
    gameMusicPlaying(false),
    soundsEnabled(true) {
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::initialize() {
    std::cout << "Audio Manager initialized" << std::endl;

    // Load sounds
    loadSound("block_land", "assets/sounds/block_land.wav");
    loadSound("line_clear", "assets/sounds/line_clear.wav");

    return true;
}

void AudioManager::shutdown() {
    stopAllMusic();
    soundFiles.clear();
    std::cout << "Audio Manager shutdown" << std::endl;
}

void AudioManager::loadSound(const std::string& name, const std::string& filename) {
    soundFiles[name] = filename;
    std::cout << "Loaded sound: " << name << " -> " << filename << std::endl;
}

void AudioManager::playSound(const std::string& name) {
    if (!soundsEnabled) return;

    auto it = soundFiles.find(name);
    if (it != soundFiles.end()) {
        playWaveFile(it->second);
    }
    else {
        std::cout << "Sound not found: " << name << std::endl;
    }
}

void AudioManager::playMenuMusic() {
    if (!menuMusicPlaying && soundsEnabled) {
        stopGameMusic();
        menuMusicPlaying = true;
        playMusicFile("assets/sounds/menu_music.wav", true);
        std::cout << "Playing menu music" << std::endl;
    }
}

void AudioManager::playGameMusic() {
    if (!gameMusicPlaying && soundsEnabled) {
        stopMenuMusic();
        gameMusicPlaying = true;
        // For now, just log - implement game music later
        std::cout << "Playing game music" << std::endl;
    }
}

void AudioManager::stopMenuMusic() {
    if (menuMusicPlaying) {
        menuMusicPlaying = false;
        stopMusic();
        std::cout << "Stopped menu music" << std::endl;
    }
}

void AudioManager::stopGameMusic() {
    if (gameMusicPlaying) {
        gameMusicPlaying = false;
        stopMusic();
        std::cout << "Stopped game music" << std::endl;
    }
}

void AudioManager::stopAllMusic() {
    stopMenuMusic();
    stopGameMusic();
}

#ifdef _WIN32
void AudioManager::playMusicFile(const std::string& filename, bool loop) {
    DWORD flags = SND_FILENAME | SND_ASYNC;
    if (loop) {
        flags |= SND_LOOP;
    }
    PlaySoundA(filename.c_str(), NULL, flags);
}

void AudioManager::stopMusic() {
    PlaySoundA(NULL, NULL, 0);
}

#ifdef _WIN32
void AudioManager::playWaveFile(const std::string& filename) {
    // Use PlaySound with SND_FILENAME flag only
    BOOL result = PlaySoundA(filename.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    if (!result) {
        DWORD error = GetLastError();
        std::cout << "Failed to play sound: " << filename << " Error: " << error << std::endl;

        // Fallback: try to play system sound
        MessageBeep(MB_ICONEXCLAMATION);
    }
}
#endif
#else
void AudioManager::playMusicFile(const std::string& filename, bool loop) {
    std::cout << "Would play music: " << filename << " (loop: " << loop << ")" << std::endl;
}

void AudioManager::stopMusic() {
    std::cout << "Would stop music" << std::endl;
}

void AudioManager::playWaveFile(const std::string& filename) {
    std::cout << "Would play WAV: " << filename << std::endl;
}
#endif