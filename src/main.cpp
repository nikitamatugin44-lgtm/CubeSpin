#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

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

static void applySpin(PlayerObject* p, float& angle, float dt, bool groundMode, float speed) {
	if (!p) return;

	// gamemode is turned off, or (ground mode) player is in the air:
	// let the game rotate normally and remember the current angle
	if (!gamemodeEnabled(p) || (groundMode && !p->m_isOnGround)) {
		angle = p->getRotation();
		return;
	}

	angle += speed * dt;
	angle = std::fmod(angle, 360.f);
	p->setRotation(angle);
}

class $modify(CubeSpinLayer, PlayLayer) {
	struct Fields {
		float angle1 = 0.f;
		float angle2 = 0.f;
	};

	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);

		auto mod = Mod::get();
		auto mode = mod->getSettingValue<std::string>("spin-mode");
		if (mode == "Off") return;

		float speed = static_cast<float>(mod->getSettingValue<double>("spin-speed"));
		bool groundMode = (mode == "Ground");

		auto fields = m_fields.self();
		applySpin(m_player1, fields->angle1, dt, groundMode, speed);
		applySpin(m_player2, fields->angle2, dt, groundMode, speed);
	}
};
