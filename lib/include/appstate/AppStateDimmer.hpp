#pragma once

#include <DGM/classes/App.hpp>
#include <DGM/classes/AppState.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Time.hpp>
#include <algorithm>
#include <tuple>
#include <types/BrandedType.hpp>

struct [[nodiscard]] FadeOptions final
{
    sf::Time duration = sf::seconds(1.f);
    sf::Color color = sf::Color::Black;
    bool shouldSimulateUnderlyingState = false;
};

using FadeInOptions = BrandedType<FadeOptions, struct FadeInOptionsTag>;
using FadeOutOptions = BrandedType<FadeOptions, struct FadeOutOptionsTag>;

namespace AppStateTransitions
{
    class [[nodiscard]] AppStateFadeIn final : public dgm::AppState
    {
    public:
        AppStateFadeIn(dgm::App& app, const FadeInOptions& options)
            : dgm::AppState(
                  app,
                  dgm::AppStateConfig {
                      .shouldUpdateUnderlyingState =
                          options.get().shouldSimulateUnderlyingState,
                      .shouldDrawUnderlyingState = true,
                  })
            , targetTime(options.get().duration)
            , overlay(sf::Vector2f(app.window.getSize()))
        {
            overlay.setFillColor(options.get().color);
        }

    public:
        void input() override
        {
            if (timer >= targetTime) app.popState();
        }

        void update() override
        {
            timer += app.time.getElapsed();
        }

        void draw() override
        {
            const auto factor = std::clamp(timer / targetTime, 0.f, 1.f);

            auto color = overlay.getFillColor();
            color.a = static_cast<uint8_t>(255 * (1.f - factor));

            overlay.setFillColor(color);
            app.window.draw(overlay);
        }

    private:
        sf::Time timer;
        sf::Time targetTime;
        sf::RectangleShape overlay;
    };

    template<class TargetAppState, class... Params>
    class [[nodiscard]] AppStateFadeOut final : public dgm::AppState
    {
    public:
        AppStateFadeOut(
            dgm::App& app, const FadeOutOptions& options, Params&&... params)
            : dgm::AppState(
                  app,
                  dgm::AppStateConfig {
                      .shouldUpdateUnderlyingState =
                          options.get().shouldSimulateUnderlyingState,
                      .shouldDrawUnderlyingState = true,
                  })
            , params(std::forward<Params>(params)...)
            , targetTime(options.get().duration)
            , overlay(sf::Vector2f(app.window.getSize()))
        {
            overlay.setFillColor(options.get().color);
        }

    public:
        void input() override
        {
            if (timer >= targetTime)
            {
                std::apply(
                    [&](Params&&... ps)
                    {
                        app.pushState<TargetAppState>(
                            std::forward<Params>(ps)...);
                    },
                    params);
            }
        }

        void update() override
        {
            timer += app.time.getElapsed();
        }

        void draw() override
        {
            const auto factor = std::clamp(timer / targetTime, 0.f, 1.f);

            auto color = overlay.getFillColor();
            color.a = static_cast<uint8_t>(255 * factor);

            overlay.setFillColor(color);
            app.window.draw(overlay);
        }

    private:
        void restoreFocusImpl(const std::string& message) override
        {
            app.popState(message);
        }

    private:
        std::tuple<Params...> params;
        sf::Time timer;
        sf::Time targetTime;
        sf::RectangleShape overlay;
    };

    template<class TargetState, class... Params>
    static void applyFadeInTransition(
        dgm::App& app, const FadeInOptions& options, Params&&... params);

    template<class TargetAppState, class... Params>
    class [[nodiscard]] AppStateFadeOutThenIn final : public dgm::AppState
    {
    public:
        AppStateFadeOutThenIn(
            dgm::App& app,
            const FadeOutOptions& options,
            const FadeInOptions& fadeInOptions,
            Params&&... params)
            : dgm::AppState(
                  app,
                  dgm::AppStateConfig {
                      .shouldUpdateUnderlyingState =
                          options.get().shouldSimulateUnderlyingState,
                      .shouldDrawUnderlyingState = true,
                  })
            , params(std::forward<Params>(params)...)
            , targetTime(options.get().duration)
            , overlay(sf::Vector2f(app.window.getSize()))
            , fadeInOptions(fadeInOptions)
        {
            overlay.setFillColor(options.get().color);
        }

    public:
        void input() override
        {
            if (timer >= targetTime)
            {
                std::apply(
                    [&](Params&&... ps)
                    {
                        applyFadeInTransition<TargetAppState>(
                            app, fadeInOptions, std::forward<Params>(ps)...);
                    },
                    params);
            }
        }

        void update() override
        {
            timer += app.time.getElapsed();
        }

        void draw() override
        {
            const auto factor = std::clamp(timer / targetTime, 0.f, 1.f);

            auto color = overlay.getFillColor();
            color.a = static_cast<uint8_t>(255 * factor);

            overlay.setFillColor(color);
            app.window.draw(overlay);
        }

    private:
        void restoreFocusImpl(const std::string& message) override
        {
            app.popState(message);
        }

    private:
        std::tuple<Params...> params;
        sf::Time timer;
        sf::Time targetTime;
        sf::RectangleShape overlay;
        FadeInOptions fadeInOptions;
    };

    template<class TargetState, class... Params>
    static void applyFadeInTransition(
        dgm::App& app, const FadeInOptions& options, Params&&... params)
    {
        app.pushState<TargetState>(std::forward<Params>(params)...);
        app.pushState<AppStateFadeIn>(options);
    }

    template<class TargetState, class... Params>
    static void applyFadeOutTransition(
        dgm::App& app, const FadeOutOptions& options, Params&&... params)
    {
        app.pushState<AppStateFadeOut<TargetState, Params...>>(
            options, std::forward<Params>(params)...);
    }

    template<class TargetState, class... Params>
    static void applyFadeOutThenInTransition(
        dgm::App& app,
        const FadeInOptions& fadeInOptions,
        const FadeOutOptions& fadeOutOptions,
        Params&&... params)
    {
        app.pushState<AppStateFadeOutThenIn<TargetState, Params...>>(
            fadeOutOptions, fadeInOptions, std::forward<Params>(params)...);
    }
} // namespace AppStateTransitions
