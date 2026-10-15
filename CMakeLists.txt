
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
        Clock::time_point started = Clock::now();
        Clock::time_point lastReport = Clock::now();

        unsigned int frames = 0;
        unsigned int framesInSample = 0;

        double fps = 0.0;
        double lastDt = 0.0;
        double minDt = 1e9;
        double maxDt = 0.0;
        double sumDt = 0.0;
        unsigned int dtSamples = 0;

        CCPoint previousPosition = {0.f, 0.f};
        bool hasPreviousPosition = false;
        double movementSpeed = 0.0;
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

        auto size = CCDirector::get()->getWinSize();

        auto label = CCLabelBMFont::create(
            "PHYSICS DEBUGGER\nStarting...",
            "bigFont.fnt"
        );

        if (label && m_uiLayer) {
            label->setAnchorPoint({0.f, 1.f});
            label->setPosition({
                8.f,
                size.height - 8.f
            });
            label->setScale(0.32f);
            label->setColor({120, 255, 150});

            m_uiLayer->addChild(label, 9999);
            m_fields->label = label;
        }

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        auto f = m_fields;

        ++f->frames;
        ++f->framesInSample;

        f->lastDt = dt;

        if (std::isfinite(dt) && dt > 0.0f) {
            f->minDt = std::min(
                f->minDt,
                static_cast<double>(dt)
            );

            f->maxDt = std::max(
                f->maxDt,
                static_cast<double>(dt)
            );

            f->sumDt += dt;
            ++f->dtSamples;
        }

        auto now = Fields::Clock::now();

        double elapsed = std::chrono::duration<double>(
            now - f->lastReport
        ).count();

        if (elapsed >= 1.0) {
            f->fps = f->framesInSample / elapsed;
            f->framesInSample = 0;
            f->lastReport = now;
        }

        auto player = m_player1;
        CCPoint pos = player
            ? player->getPosition()
            : CCPoint{0.f, 0.f};

        double totalElapsed = std::chrono::duration<double>(
            now - f->started
        ).count();

        if (player && f->hasPreviousPosition && elapsed > 0.0) {
            double dx = pos.x - f->previousPosition.x;
            double dy = pos.y - f->previousPosition.y;

            f->movementSpeed = std::hypot(dx, dy) / elapsed;
        }

        f->previousPosition = pos;
        f->hasPreviousPosition = player != nullptr;

        if (!f->label)
            return;

        double avgDt = f->dtSamples
            ? f->sumDt / f->dtSamples
            : 0.0;

        char text[2048];

        std::snprintf(
            text,
            sizeof(text),
            "PHYSICS DEBUGGER\n"
            "-------------------------\n"
            "FPS:              %.2f\n"
            "FRAME TIME:       %.3f ms\n"
            "PLAYLAYER DT:     %.8f s\n"
            "DT MIN:           %.8f s\n"
            "DT MAX:           %.8f s\n"
            "DT AVG:           %.8f s\n"
            "TOTAL UPDATES:    %u\n"
            "UPTIME:           %.1f s\n"
            "-------------------------\n"
            "LEVEL PROGRESS:   %d%%\n"
            "-------------------------\n"
            "PLAYER X:         %.3f\n"
            "PLAYER Y:         %.3f\n"
            "ROTATION:         %.3f deg\n"
            "POSITION SPEED:   %.3f units/s\n"
            "-------------------------\n"
            "DT x 60:          %.8f\n"
            "DT x 120:         %.8f\n"
            "DT x 240:         %.8f\n"
            "DT x 360:         %.8f\n"
            "DT x 480:         %.8f\n",
            f->fps,
            f->fps > 0.0 ? 1000.0 / f->fps : 0.0,
            f->lastDt,
            f->minDt == 1e9 ? 0.0 : f->minDt,
            f->maxDt,
            avgDt,
            f->frames,
            totalElapsed,
            getCurrentPercent(),
            pos.x,
            pos.y,
            player ? player->getRotation() : 0.f,
            f->movementSpeed,
            dt * 60.0,
            dt * 120.0,
            dt * 240.0,
            dt * 360.0,
            dt * 480.0
        );

        f->label->setString(text);
    }
};
