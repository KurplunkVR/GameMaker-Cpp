#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

namespace GM {

// Asset type IDs
enum class AssetType {
    None = -1,
    Object = 0,
    Sprite = 1,
    Sound = 2,
    Room = 3,
    Path = 4,
    Script = 5,
    Font = 6,
    Timeline = 7,
    Shader = 8,
    Sequence = 9,
    AnimCurve = 10,
    ParticleSystem = 11,
    Tilemap = 12,
    Tileset = 13,
    Instance = 14,
    ParticleSystemInstance = 15
};

// Reference category flags
constexpr uint32_t REFCAT_RESOURCE = 0x01000000;
constexpr uint32_t REFCAT_DATA_STRUCTURE = 0x02000000;
constexpr uint32_t REFCAT_INSTANCE = 0x04000000;
constexpr uint32_t REFCAT_GENERAL = 0x08000000;

// Reference IDs
constexpr uint32_t REFID_INSTANCE = (0x00000001 | REFCAT_INSTANCE);
constexpr uint32_t REFID_DBG = (0x00000002 | REFCAT_INSTANCE);
constexpr uint32_t REFID_PART_SYSTEM = (0x00000004 | REFCAT_INSTANCE);
constexpr uint32_t REFID_PART_EMITTER = (0x00000008 | REFCAT_INSTANCE);
constexpr uint32_t REFID_PART_TYPE = (0x00000010 | REFCAT_INSTANCE);

constexpr uint32_t REFID_OBJECT = ((int)AssetType::Object | REFCAT_RESOURCE);
constexpr uint32_t REFID_SPRITE = ((int)AssetType::Sprite | REFCAT_RESOURCE);
constexpr uint32_t REFID_SOUND = ((int)AssetType::Sound | REFCAT_RESOURCE);
constexpr uint32_t REFID_ROOM = ((int)AssetType::Room | REFCAT_RESOURCE);
constexpr uint32_t REFID_PATH = ((int)AssetType::Path | REFCAT_RESOURCE);

// Data structures
constexpr uint32_t REFID_DS_LIST = (0x00000001 | REFCAT_DATA_STRUCTURE);
constexpr uint32_t REFID_DS_MAP = (0x00000002 | REFCAT_DATA_STRUCTURE);
constexpr uint32_t REFID_DS_GRID = (0x00000004 | REFCAT_DATA_STRUCTURE);
constexpr uint32_t REFID_DS_QUEUE = (0x00000008 | REFCAT_DATA_STRUCTURE);

// Events
enum class EventType {
    Create = 0,
    Destroy = 1,
    Alarm = 2,
    Step = 3,
    Collision = 4,
    Keyboard = 5,
    Mouse = 6,
    Other = 7,
    Draw = 8,
    KeyRelease = 9,
    Trigger = 10,
    CleanUp = 11,
    Gesture = 12,
    PreDraw = 13,
    PostDraw = 14,
};

enum class StepEventType {
    BeginStep = 0,
    NormalStep = 1,
    EndStep = 2,
};

// GameMaker Variable Type
using VariantValue = std::variant<
    std::nullptr_t,  // undefined/null
    double,          // real number
    std::string      // string
>;

class Variant {
public:
    Variant() : value(nullptr) {}
    explicit Variant(std::nullptr_t) : value(nullptr) {}
    explicit Variant(double val) : value(val) {}
    explicit Variant(int val) : value((double)val) {}
    explicit Variant(const std::string& val) : value(val) {}
    explicit Variant(const char* val) : value(std::string(val)) {}

    double AsDouble() const;
    int AsInt() const;
    std::string AsString() const;
    bool AsBool() const;

    bool IsReal() const;
    bool IsString() const;
    bool IsUndefined() const;

    VariantValue value;
};

// Math types
struct Vector2 {
    double x, y;
    Vector2(double x = 0.0, double y = 0.0) : x(x), y(y) {}
};

struct Vector3 {
    double x, y, z;
    Vector3(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}
};

struct Rect {
    double x1, y1, x2, y2;
    Rect(double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0)
        : x1(x1), y1(y1), x2(x2), y2(y2) {}
    
    double Width() const { return x2 - x1; }
    double Height() const { return y2 - y1; }
};

// Color (ARGB)
using Color = uint32_t;

inline Color MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

} // namespace GM
