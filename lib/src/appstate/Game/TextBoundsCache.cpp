#include "appstate/Game/TextBoundsCache.hpp"

sf::Vector2f TextBoundsCache::getGlobalBounds(const sf::Text& text)
{
    auto&& itr = cache.find(text.getString());
    if (itr != cache.end()) return itr->second;

    auto&& bounds = text.getGlobalBounds().size;
    cache[text.getString()] = bounds;
    return bounds;
}
