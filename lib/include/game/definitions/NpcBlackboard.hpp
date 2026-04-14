#pragma once

#include "game/enums/NpcKind.hpp"
#include "game/input/NpcInput.hpp"
#include <SFML/System/Vector2.hpp>
#include <fsm/Types.hpp>

struct [[nodiscard]] NpcBlackboard final : public fsm::BlackboardBase
{
    entt::entity ownerEntity = entt::null;
    NpcInput& input;
    NpcKind kind = NpcKind::Melee;
    entt::entity targetEntity = entt::null;
    sf::Vector2f waypoint;
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
            "[ waypoint: {} | kind: {} ]",
            dgm::Utility::to_string(bb.waypoint),
            std::to_underlying(bb.kind));
    }
};

#endif
