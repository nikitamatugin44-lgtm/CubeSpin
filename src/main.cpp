#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <cmath>

using namespace geode::prelude;

static bool gamemodeEnabled(PlayerObject* p) {
	auto mod = Mod::get();
	if (p->m_isShip)   return mod->getSettingValue<bool>("spin-ship");
	if (p->m_isBird)   return mod->getSettingValue<bool>("spin-ufo");
	if (p->m_isBall)   return mod->getSettingValue<bool>("spin-ball");
	if (p->m_isDart)   return mod->getSettingValue<bool>("spin-wave");
	if (p->m_isRobot)  return mod->getSettingValue<bool>("spin-robot");
	if (p->m_isSpider) return mod->getSettingValue<bool>("spin-spider");
	if (p->m_isSwing)  return mod->getSettingValue<bool>("spin-swing");
	return mod->getSettingValue<bool>("spin-cube");
}

// On the floor = standing on a surface AND low enough to be at ground level
static bool onFloor(PlayerObject* p, float floorHeight) {
	return p->m_isOnGround && p->getPositionY() < floorHeight + 8.f;
}

static void applySpin(PlayerObject* p, float& angle, float dt, bool floorOnly, float floorHeight, float speed) {
	if (!p) return;

	if (!gamemodeEnabled(p) || (floorOnly && !onFloor(p, floorHeight))) {
		angle = p->getRotation();
		return;
	}

	angle += speed * dt;
	angle = std::fmod(angle, 360.f);
	p->setRotation(angle);
}

class $modify(IconSpinLayer, PlayLayer) {
	struct Fields {
		float angle1 = 0.f;
		float angle2 = 0.f;
	};

	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);

		auto mod = Mod::get();
		if (!mod->getSettingValue<bool>("enable-spin")) return;

		float speed = static_cast<float>(mod->getSettingValue<double>("spin-speed"));
		bool floorOnly = mod->getSettingValue<bool>("spin-floor-only");
		float floorHeight = static_cast<float>(mod->getSettingValue<double>("floor-height"));

		auto fields = m_fields.self();
		applySpin(m_player1, fields->angle1, dt, floorOnly, floorHeight, speed);
		applySpin(m_player2, fields->angle2, dt, floorOnly, floorHeight, speed);
	}
};
