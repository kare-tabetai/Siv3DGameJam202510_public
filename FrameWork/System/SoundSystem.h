#pragma once
#include "CD/AudioCD.h"
#include "FrameWork/flecs/flecs.h"

class SoundSystem {
  public:
  static void Register(flecs::world& world) {
    world.add<AudioCD>();
    auto* audio_cd = world.get_mut<AudioCD>();
    audio_cd->key = U"BGM";
    audio_cd->path = U"Asset/Scott-Joplin-TheEntertainer-2020-AR.mp3";
    audio_cd->audio = Audio{audio_cd->path};
    audio_cd->audio.setLoop(true);   // ループ再生を設定
    audio_cd->audio.setVolume(0.5);  // 音量を50%に設定
    audio_cd->audio.play();          // オーディオを再生する
  }

  static Audio* GetAudio(flecs::world& world, StringView key) {
    world.each([&world, key](const AudioCD& audio_cd) {
      if (audio_cd.key == key) {
        return &audio_cd.audio;
      }
    });
    return nullptr;
  }
};