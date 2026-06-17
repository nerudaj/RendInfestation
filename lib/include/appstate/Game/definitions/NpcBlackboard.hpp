#pragma once

#include "appstate/Game/enums/NpcKind.hpp"
#include "appstate/Game/input/NpcInput.hpp"
#include <SFML/System/Vector2.hpp>
#include <fsm/Types.hpp>
#include <optional>

struct [[nodiscard]] NpcBlackboard final : public fsm::BlackboardBase
{
    entt::entity ownerEntity = entt::null;
    NpcInput& input;
    NpcKind kind = {};
    entt::entity targetEntity = entt::null;
    std::optional<sf::Vector2f> waypoint = std::nullopt;
};

#ifndef ANDROID

#include <format>

template<class CharT>
struct std::formatter<NpcBlackboard, CharT>
{
    template<class ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<class FormatContext>
    constexpr auto format(const NpcBlackboard& bb, FormatContext& ctx) const
    {
        return std::format_to(
            ctx.out(),
            "\"[ waypoint: {} | kind: {} ]\"",
            bb.waypoint ? dgm::Utility::to_string(*bb.waypoint)
                        : std::string("NONE"),
            std::to_underlying(bb.kind));
    }
};

#endif
