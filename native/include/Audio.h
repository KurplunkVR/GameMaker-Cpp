#pragma once

#include "GMLTypes.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cstdint>

namespace GM {

// Sound types
enum class SoundType {
    WAV = 0,
    MP3 = 1,
    OGG = 2,
    FLAC = 3,
};

// Sound effects
enum class SoundEffect {
    None = 0,
    Echo = 1,
    Reverb = 2,
    Chorus = 3,
};

// Forward declarations
class AudioBuffer;

// Represents a single sound/audio file
class Sound {
public:
    Sound(uint32_t id, const std::string& name);
    ~Sound();

    uint32_t GetID() const { return id; }
    const std::string& GetName() const { return name; }

    // Properties
    SoundType GetType() const { return type; }
    void SetType(SoundType t) { type = t; }

    const std::string& GetExtension() const { return extension; }
    void SetExtension(const std::string& ext) { extension = ext; }

    double GetVolume() const { return volume; }
    void SetVolume(double vol) { volume = vol; }

    double GetPan() const { return pan; }
    void SetPan(double p) { pan = p; }

    bool GetPreload() const { return preload; }
    void SetPreload(bool val) { preload = val; }

    SoundEffect GetEffect() const { return effect; }
    void SetEffect(SoundEffect eff) { effect = eff; }

    // Audio buffer
    const std::shared_ptr<AudioBuffer>& GetBuffer() const { return audio_buffer; }
    void SetBuffer(std::shared_ptr<AudioBuffer> buf) { audio_buffer = buf; }

    // Duration in milliseconds
    uint32_t GetDuration() const { return duration_ms; }
    void SetDuration(uint32_t dur) { duration_ms = dur; }

private:
    uint32_t id;
    std::string name;
    SoundType type = SoundType::WAV;
    std::string extension;
    double volume = 1.0;
    double pan = 0.0;  // -1 to 1, 0 is center
    bool preload = true;
    SoundEffect effect = SoundEffect::None;
    std::shared_ptr<AudioBuffer> audio_buffer;
    uint32_t duration_ms = 0;
};

// Represents a playing sound instance
class AudioInstance {
public:
    AudioInstance(uint32_t id, const Sound* sound);
    ~AudioInstance();

    uint32_t GetID() const { return id; }
    const Sound* GetSound() const { return sound; }

    // Playback state
    bool IsPlaying() const { return playing; }
    void SetPlaying(bool val) { playing = val; }

    double GetPlayPosition() const { return play_position; }
    void SetPlayPosition(double pos) { play_position = pos; }

    double GetPlaybackSpeed() const { return playback_speed; }
    void SetPlaybackSpeed(double speed) { playback_speed = speed; }

    // Volume and pan
    double GetVolume() const { return volume; }
    void SetVolume(double vol) { volume = vol; }

    double GetPan() const { return pan; }
    void SetPan(double p) { pan = p; }

    // Looping
    bool GetLooping() const { return looping; }
    void SetLooping(bool val) { looping = val; }

    uint32_t GetLoopCount() const { return loop_count; }
    void SetLoopCount(uint32_t count) { loop_count = count; }

    // Update
    void Update(double delta_time);

private:
    uint32_t id;
    const Sound* sound;
    bool playing = false;
    double play_position = 0.0;
    double playback_speed = 1.0;
    double volume = 1.0;
    double pan = 0.0;
    bool looping = false;
    uint32_t loop_count = 0;
};

// Main audio manager
class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    // Sound management
    void AddSound(std::shared_ptr<Sound> sound);
    void RemoveSound(uint32_t id);
    std::shared_ptr<Sound> GetSound(uint32_t id);
    const std::vector<std::shared_ptr<Sound>>& GetSounds() const { return sounds; }

    // Sound playback
    std::shared_ptr<AudioInstance> PlaySound(uint32_t sound_id, bool loop = false, 
                                              double volume = -1.0, double pan = 0.0);
    void StopSound(uint32_t instance_id);
    void StopAllSounds();
    void PauseSound(uint32_t instance_id);
    void ResumeSound(uint32_t instance_id);

    // Get playing instances
    std::shared_ptr<AudioInstance> GetInstance(uint32_t id);
    const std::vector<std::shared_ptr<AudioInstance>>& GetInstances() const { return instances; }

    // Master volume
    double GetMasterVolume() const { return master_volume; }
    void SetMasterVolume(double vol) { master_volume = vol; }

    // Update all playing sounds
    void Update(double delta_time);

    // Initialize/Shutdown
    bool Initialize();
    void Shutdown();

    bool IsInitialized() const { return initialized; }

private:
    std::vector<std::shared_ptr<Sound>> sounds;
    std::map<uint32_t, std::shared_ptr<Sound>> sound_map;

    std::vector<std::shared_ptr<AudioInstance>> instances;
    std::map<uint32_t, std::shared_ptr<AudioInstance>> instance_map;

    double master_volume = 1.0;
    uint32_t next_instance_id = 1;
    bool initialized = false;
};

} // namespace GM
