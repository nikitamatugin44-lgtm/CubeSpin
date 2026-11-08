#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Idea: add a fixed offset to the player's x position as soon as the level
// starts. The float that tracks x never "knows" the offset isn't real
// progress, so it loses precision as if the player had already traveled
// that extra distance -> X pos limit / SSB shows up earlier.
//
// There is no clean "exactly 4x faster" formula: float precision breaks
// down in steps at powers of two, not smoothly. Treat "bias" as a dial to
// calibrate by testing, not a guaranteed multiplier.

class $modify(XPosBiasPlayer, PlayerObject) {
    void update(float dt) {
        PlayerObject::update(dt);
    }
};

class $modify(XPosBiasLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto mod = Mod::get();
        if (mod->getSettingValue<bool>("enable-bias")) {
            double bias = mod->getSettingValue<double>("x-bias");
            if (m_player1) m_player1->setPositionX(m_player1->getPositionX() + bias);
            if (m_player2) m_player2->setPositionX(m_player2->getPositionX() + bias);
        }
        return true;
    }
};
