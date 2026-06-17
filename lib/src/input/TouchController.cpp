#include "input/TouchController.hpp"
#include <SFML/System/Err.hpp>

void TouchInput::reset()
{
    if (kind == TouchObjectKind::Button)
        // place touch position out of touchArea - button is not pressed
        touchPosition = touchArea.getPosition()
                        - sf::Vector2f {
                              touchArea.getRadius(),
                              touchArea.getRadius(),
                          };
    else
        touchPosition = touchArea.getPosition();
}

std::array<TouchInput, 5u>
TouchModel::computeLayout(const sf::Vector2u& windowSize, const Sizer& sizer)
{
    const float buttonRadius = sizer.getBaseContainerHeight();
    const float joystickRadius = windowSize.x * 0.1f;
    const auto windowsSizeF = sf::Vector2f(windowSize);

    const auto rightJoystickOrigin =
        sf::Vector2f { windowSize.x - joystickRadius,
                       windowSize.y - joystickRadius };
    const auto actionButtonBaseDir =
        sf::Vector2f { 0.f, -joystickRadius - buttonRadius };

    return std::array {
        TouchInput(
            TouchObjectKind::Joystick,
            { joystickRadius, windowSize.y - joystickRadius },
            joystickRadius),
        TouchInput(
            TouchObjectKind::Joystick, rightJoystickOrigin, joystickRadius),
        TouchInput( // Pause
            TouchObjectKind::Button,
            { buttonRadius, buttonRadius },
            buttonRadius),
        TouchInput( // Swap
            TouchObjectKind::Button,
            rightJoystickOrigin
                + actionButtonBaseDir.rotatedBy(sf::degrees(-60)),
            buttonRadius),
        TouchInput( // Interact
            TouchObjectKind::Button,
            rightJoystickOrigin
                + actionButtonBaseDir.rotatedBy(sf::degrees(-30)),
            buttonRadius),
    };
}

TouchModel::TouchModel(const sf::Vector2u& windowSize, const Sizer& sizer)
    : sizer(sizer), objects(computeLayout(windowSize, sizer))
{
}

void TouchController::processEvent(const std::optional<sf::Event>& e)
{
    if (e->is<sf::Event::TouchBegan>())
        processEvent(*e->getIf<sf::Event::TouchBegan>());
    else if (e->is<sf::Event::TouchEnded>())
        processEvent(*e->getIf<sf::Event::TouchEnded>());
    else if (e->is<sf::Event::TouchMoved>())
        processEvent(*e->getIf<sf::Event::TouchMoved>());
}

[[nodiscard]] sf::Vector2f TouchController::getForward() const
{
    return model.leftJoystick.readJoystick();
}

sf::Vector2f TouchController::getAimDirection() const
{
    return model.rightJoystick.readJoystick();
}

[[nodiscard]] bool TouchController::isBackPressed() const
{
    const auto value = model.pauseButton.readButton();
    if (value) model.pauseButton.reset();
    return value;
}

bool TouchController::isSwitchWeaponsPressed() const
{
    const auto value = model.swapWeaponButton.readButton();
    if (value) model.swapWeaponButton.reset();
    return value;
}

bool TouchController::isInteractPressed() const
{
    const auto value = model.interactButton.readButton();
    if (value) model.interactButton.reset();
    return value;
}

void TouchController::processEvent(const sf::Event::TouchBegan& e)
{
    for (auto&& [idx, object] : std::ranges::views::enumerate(model.objects))
    {
        if (dgm::Collision::basic(object.touchArea, e.position))
        {
            object.touchPosition = sf::Vector2f(e.position);
            model.fingerToTouchObject[e.finger] = idx;
        }
    }
}

void TouchController::processEvent(const sf::Event::TouchEnded& e)
{
    if (!model.fingerToTouchObject.contains(e.finger)) return;

    auto idx = model.fingerToTouchObject.at(e.finger);
    model.fingerToTouchObject.erase(e.finger);
    model.objects[idx].reset();
}

void TouchController::processEvent(const sf::Event::TouchMoved& e)
{
    // Update touchPosition, but normalize it so it stays inside the
    // touchArea even when the finger moves out of it
    if (!model.fingerToTouchObject.contains(e.finger)) return;

    auto idx = model.fingerToTouchObject.at(e.finger);
    auto& obj = model.objects[idx];
    auto direction = sf::Vector2f(e.position) - obj.touchArea.getPosition();
    auto length = direction.length();
    obj.touchPosition =
        obj.touchArea.getPosition()
        + dgm::Math::toUnit(direction)
              * std::clamp(length, 0.f, obj.touchArea.getRadius());
}
