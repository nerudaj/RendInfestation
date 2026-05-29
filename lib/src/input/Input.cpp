#include "input/Input.hpp"
#include "types/Overloads.hpp"
#include <SFML/Window/Mouse.hpp>

void Input::updateBindings(const BindingsSettings& settings)
{
    controller = configureController(settings);
}

void Input::forceRelease(InputKind action)
{
    controller.forceRelease(action);
}

sf::Vector2f Input::getForward() const
{
    return sf::Vector2f { -controller.readAnalog(InputKind::Left)
                              + controller.readAnalog(InputKind::Right),
                          -controller.readAnalog(InputKind::Up)
                              + controller.readAnalog(InputKind::Down) }
           + touchController.getForward();
}

sf::Vector2f Input::getAimDirection() const
{
    const sf::Vector2f controllerAim {
        -controller.readAnalog(InputKind::AimLeft)
            + controller.readAnalog(InputKind::AimRight),
        -controller.readAnalog(InputKind::AimUp)
            + controller.readAnalog(InputKind::AimDown)
    };
    const sf::Vector2f touchAim = touchController.getAimDirection();

    if (controllerAim != sf::Vector2f {} || touchAim != sf::Vector2f {})
        return (controllerAim + touchAim) * 100.f;

    // Fallback: direction from screen center to mouse cursor
    const sf::Vector2f center { sf::Vector2f(window.getSize()) / 2.f };
    const sf::Vector2f mousePos { sf::Vector2f(
        sf::Mouse::getPosition(window)) };
    const sf::Vector2f delta = mousePos - center;
    if (delta == sf::Vector2f {}) return {};
    return delta;
};

bool Input::isShootPressed() const
{
    return controller.readDigital(InputKind::Shoot)
           || touchController.getAimDirection().length() >= 0.01f;
}

bool Input::isSwapWeaponsPressed() const
{
    return readAndRelease(InputKind::SwitchWeapons)
           || touchController.isSwitchWeaponsPressed();
}

bool Input::isInteractPressed() const
{
    return readAndRelease(InputKind::Interact)
           || touchController.isInteractPressed();
}

[[nodiscard]] bool Input::isMenuCycleLeftPressed() const
{
    return readAndRelease(InputKind::MenuCycleLeft);
}

[[nodiscard]] bool Input::isMenuCycleRightPressed() const
{
    return readAndRelease(InputKind::MenuCycleRight);
}

bool Input::isPauseButtonPressed() const
{
    return readAndRelease(InputKind::PauseButton)
           || touchController.isBackPressed();
}

bool Input::isBackButtonPressed() const
{
    return readAndRelease(InputKind::BackButton);
}

bool Input::isConfirmPressed() const
{
    return readAndRelease(InputKind::ConfirmButton);
}

sf::Vector2f Input::getCursorDelta() const
{
    return sf::Vector2f {
        -controller.readAnalog(InputKind::CursorLeft)
            + controller.readAnalog(InputKind::CursorRight),
        -controller.readAnalog(InputKind::CursorUp)
            + controller.readAnalog(InputKind::CursorDown),
    };
}

bool Input::readAndRelease(InputKind i) const
{
    return controller.readDigital(i, dgm::DigitalReadKind::OnPress);
}

dgm::Controller<InputKind>
Input::configureController(const BindingsSettings& settings)
{
    auto controller = dgm::Controller<InputKind>();

    auto bindActions = [&](auto bindings)
    {
        for (auto&& [action, binding] : bindings)
        {
            auto&& [kmbBinding, gamepadBinding] = binding;

            std::visit(
                overloads { [&](sf::Keyboard::Key key)
                            { controller.bindInput(action, key); },
                            [&](sf::Mouse::Button mbtn)
                            { controller.bindInput(action, mbtn); },
                            [&](auto) {} },
                kmbBinding);
            std::visit(
                overloads {
                    [&](GamepadButton btn)
                    { controller.bindInput(action, btn.get()); },
                    [&](std::pair<sf::Joystick::Axis, dgm::AxisHalf> joy)
                    { controller.bindInput(action, joy.first, joy.second); },
                    [&](auto) {} },
                gamepadBinding);
        }
    };

    bindActions(settings.ingameBindings);
    bindActions(settings.menuBindings);

    return controller;
}
