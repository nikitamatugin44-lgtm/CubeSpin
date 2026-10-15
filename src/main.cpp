
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>

using namespace geode::prelude;

class $modify(PhysicsDebugger, PlayLayer) {
    struct Fields {
        CCLabelBMFont* label = nullptr;

        using Clock = std::chrono::steady_clock;

        Clock::time_point lastSample = Clock::now();

        unsigned int postUpdates = 0;
        unsigned int lastPostUpdates = 0;

        double dtSum = 0.0;
        double dtMin = 999999.0;
        double dtMax = 0.0;
        double dtAverage = 0.0;
        double postUpdateRate = 0.0;

        CCPoint previousPosition = {0.f, 0.f};
        bool hasPreviousPosition = false;

        double playerSpeed = 0.0;
    };

    bool init(
        GJGameLevel* level,
        bool useReplay,
        bool dontCreateObjects
    ) {
        if (!PlayLayer::init(
            level,
            useReplay,
            dontCreateObjects
        )) {
            return false;
        }

        auto winSize = CCDirector::get()->getWinSize();

        auto label = CCLabelBMFont::create(
            "PHYSICS DEBUGGER",
            "bigFont.fnt"
        );

        if (label && m_uiLayer) {
            label->setAnchorPoint({0.f, 1.f});
            label->setPosition({
                8.f,
                winSize.height - 8.f
            });
            label->setScale(0.32f);
            label->setColor({130, 255, 160});

            m_uiLayer->addChild(label, 9999);
            m_fields->label = label;
        }

        log::info("[PhysicsDebugger] Started");
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto f = m_fields.self();
        auto now = Fields::Clock::now();

        ++f->postUpdates;

        if (dt > 0.f && std::isfinite(dt)) {
            f->dtSum += dt;
            f->dtMin = std::min(
                f->dtMin,
                static_cast<double>(dt)
            );
            f->dtMax = std::max(
                f->dtMax,
                static_cast<double>(dt)
            );
        }

        auto player = m_player1;

        CCPoint pos = player
            ? player->getPosition()
            : CCPoint{0.f, 0.f};

        float rotation = player
            ? player->getRotation()
            : 0.f;

        if (player && f->hasPreviousPosition && dt > 0.f) {
            double dx = pos.x - f->previousPosition.x;
            double dy = pos.y - f->previousPosition.y;

            f->playerSpeed = std::hypot(dx, dy) / dt;
        }

        f->previousPosition = pos;
        f->hasPreviousPosition = player != nullptr;

        double elapsed = std::chrono::duration<double>(
            now - f->lastSample
        ).count();

        if (elapsed >= 1.0) {
            f->postUpdateRate =
                (f->postUpdates - f->lastPostUpdates)
                / elapsed;

            f->lastPostUpdates = f->postUpdates;
            f->lastSample = now;

            f->dtAverage = f->dtSum > 0.0
                ? f->dtSum
                    / static_cast<double>(
                        std::max(1u, f->postUpdates)
                    )
                : 0.0;

            f->dtSum = 0.0;
            f->dtMin = 999999.0;
            f->dtMax = 0.0;
        }

        auto label = f->label;

        if (!label)
            return;

        char text[2048];

        std::snprintf(
            text,
            sizeof(text),
            "PHYSICS DEBUGGER\n"
            "--------------------------\n"
            "POSTUPDATE RATE: %.2f/s\n"
            "POSTUPDATE DT:   %.7f s\n"
            "DT MIN:          %.7f s\n"
            "DT MAX:          %.7f s\n"
            "DT AVG:          %.7f s\n"
            "TOTAL CALLS:     %u\n"
            "--------------------------\n"
            "LEVEL PROGRESS:  %d%%\n"
            "--------------------------\n"
            "PLAYER 1 X:      %.4f\n"
            "PLAYER 1 Y:      %.4f\n"
            "ROTATION:        %.4f deg\n"
            "POSITION SPEED:  %.4f units/s\n"
            "--------------------------\n"
            "DT * 60:         %.7f\n"
            "DT * 240:        %.7f\n"
            "DT * 360:        %.7f\n"
            "DT * 480:        %.7f\n",
            f->postUpdateRate,
            dt,
            f->dtMin == 999999.0 ? 0.0 : f->dtMin,
            f->dtMax,
            f->dtAverage,
            f->postUpdates,
            getCurrentPercent(),
            pos.x,
            pos.y,
            rotation,
            f->playerSpeed,
            dt * 60.f,
            dt * 240.f,
            dt * 360.f,
            dt * 480.f
        );

        label->setString(text);
    }
};
