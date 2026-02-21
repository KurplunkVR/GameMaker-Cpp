#include "Audio.h"
#include <cstdio>
#include <algorithm>

namespace GM {

// Sound implementation
Sound::Sound(uint32_t id, const std::string& name)
    : id(id), name(name) {
}

Sound::~Sound() {
}

// AudioInstance implementation
AudioInstance::AudioInstance(uint32_t id, const Sound* sound)
    : id(id), sound(sound) {
}

AudioInstance::~AudioInstance() {
}

void AudioInstance::Update(double delta_time) {
    if (playing && sound) {
        play_position += delta_time * playback_speed;
        
        uint32_t duration = sound->GetDuration();
        if (duration > 0 && play_position >= duration / 1000.0) {
            if (looping) {
                loop_count++;
                play_position = 0.0;
            } else {
                playing = false;
            }
        }
    }
}

// AudioManager implementation
AudioManager::AudioManager() {
}

AudioManager::~AudioManager() {
    Shutdown();
}

void AudioManager::AddSound(std::shared_ptr<Sound> sound) {
    if (!sound) return;
    sounds.push_back(sound);
    sound_map[sound->GetID()] = sound;
}

void AudioManager::RemoveSound(uint32_t id) {
    auto it = sound_map.find(id);
    if (it != sound_map.end()) {
        auto sound = it->second;
        sound_map.erase(it);
        sounds.erase(std::find(sounds.begin(), sounds.end(), sound));
    }
}

std::shared_ptr<Sound> AudioManager::GetSound(uint32_t id) {
    auto it = sound_map.find(id);
    if (it != sound_map.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<AudioInstance> AudioManager::PlaySound(uint32_t sound_id, bool loop,
                                                        double volume, double pan) {
    auto sound = GetSound(sound_id);
    if (!sound) return nullptr;

    auto instance = std::make_shared<AudioInstance>(next_instance_id++, sound.get());
    instance->SetLooping(loop);
    
    if (volume >= 0) {
        instance->SetVolume(volume);
    } else {
        instance->SetVolume(sound->GetVolume());
    }
    
    instance->SetPan(pan != 0 ? pan : sound->GetPan());
    instance->SetPlaying(true);

    instances.push_back(instance);
    instance_map[instance->GetID()] = instance;

    printf("[Audio] Playing sound %d (instance %d)\n", sound_id, instance->GetID());
    return instance;
}

void AudioManager::StopSound(uint32_t instance_id) {
    auto it = instance_map.find(instance_id);
    if (it != instance_map.end()) {
        auto inst = it->second;
        inst->SetPlaying(false);
        instances.erase(std::find(instances.begin(), instances.end(), inst));
        instance_map.erase(it);
    }
}

void AudioManager::StopAllSounds() {
    instances.clear();
    instance_map.clear();
}

void AudioManager::PauseSound(uint32_t instance_id) {
    auto inst = GetInstance(instance_id);
    if (inst) {
        inst->SetPlaying(false);
    }
}

void AudioManager::ResumeSound(uint32_t instance_id) {
    auto inst = GetInstance(instance_id);
    if (inst) {
        inst->SetPlaying(true);
    }
}

std::shared_ptr<AudioInstance> AudioManager::GetInstance(uint32_t id) {
    auto it = instance_map.find(id);
    if (it != instance_map.end()) {
        return it->second;
    }
    return nullptr;
}

void AudioManager::Update(double delta_time) {
    // Update and clean up finished instances
    std::vector<std::shared_ptr<AudioInstance>> to_remove;
    
    for (auto& inst : instances) {
        inst->Update(delta_time);
        if (!inst->IsPlaying()) {
            to_remove.push_back(inst);
        }
    }

    for (auto& inst : to_remove) {
        instance_map.erase(inst->GetID());
        instances.erase(std::find(instances.begin(), instances.end(), inst));
    }
}

bool AudioManager::Initialize() {
    printf("[Audio] Initializing audio manager\n");
    initialized = true;
    return true;
}

void AudioManager::Shutdown() {
    StopAllSounds();
    sounds.clear();
    sound_map.clear();
    printf("[Audio] Audio manager shutdown\n");
}

} // namespace GM
