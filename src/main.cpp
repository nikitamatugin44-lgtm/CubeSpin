#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(CubeSpinPlayer, PlayerObject) {
	struct Fields {
		float angle = 0.f;
	};

	void update(float dt) {
		PlayerObject::update(dt);

		if (!Mod::get()->getSettingValue<bool>("enable-spin")) return;

		if (m_isShip || m_isBall || m_isBird || m_isDart ||
			m_isRobot || m_isSpider || m_isSwing) return;

		auto speed = static_cast<float>(
			Mod::get()->getSettingValue<double>("spin-speed")
		);

		auto fields = m_fields.self();
		fields->angle += speed * dt;

		if (fields->angle > 360.f)  fields->angle = std::fmod(fields->angle, 360.f);
		if (fields->angle < -360.f) fields->angle = std::fmod(fields->angle, 360.f);

		this->setRotation(fields->angle);
	}
};
