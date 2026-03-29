#include "misc/ShadeableRenderingPipeline2D.hpp"

ShadeableRenderingPipeline2D::ShadeableRenderingPipeline2D(
    const sf::Texture& texture,
    const sf::Shader& shader,
    const std::optional<sf::BlendMode> mode)
    : vertices(sf::PrimitiveType::Triangles, 512)
    , renderStates(sf::RenderStates::Default)
{
    renderStates.texture = &texture;
    renderStates.shader = &shader;

    if (mode) renderStates.blendMode = *mode;
}

ShadeableRenderingPipeline2D::ShadeableRenderingPipeline2D(
    const sf::Texture& texture, const std::optional<sf::BlendMode> mode)
    : vertices(sf::PrimitiveType::Triangles, 512)
    , renderStates(sf::RenderStates::Default)
{
    renderStates.texture = &texture;
    if (mode) renderStates.blendMode = *mode;
}

void ShadeableRenderingPipeline2D::addFace(
    const sf::Vector2f& origin,
    const sf::FloatRect& textureRect,
    const sf::Angle& rotation,
    const sf::Vector2f& scale,
    const sf::Color color)
{
    resizeVertexArrayIfNeeded();
    auto texSizeHalf = textureRect.size / 2.f;

    // Face is - first triangle
    // 0---1
    //  \  |
    //   \ |
    //    \|
    //     2

    // second triangle
    // 3
    // |\
    // | \
    // |  \
    // 5---4

    // both triangles are going clockwise

    vertices[currentVertexIdx + 0].position =
        origin - texSizeHalf.componentWiseMul(scale).rotatedBy(rotation);
    vertices[currentVertexIdx + 0].texCoords = textureRect.position;
    vertices[currentVertexIdx + 0].color = color;

    vertices[currentVertexIdx + 1].position =
        origin
        + sf::Vector2f(texSizeHalf.x, -texSizeHalf.y)
              .componentWiseMul(scale)
              .rotatedBy(rotation);
    vertices[currentVertexIdx + 1].texCoords =
        textureRect.position + sf::Vector2f(textureRect.size.x, 0.f);
    vertices[currentVertexIdx + 1].color = color;

    vertices[currentVertexIdx + 2].position =
        origin + texSizeHalf.componentWiseMul(scale).rotatedBy(rotation);
    vertices[currentVertexIdx + 2].texCoords =
        textureRect.position + textureRect.size;
    vertices[currentVertexIdx + 2].color = color;

    // same as 0
    vertices[currentVertexIdx + 3].position =
        vertices[currentVertexIdx + 0].position;
    vertices[currentVertexIdx + 3].texCoords =
        vertices[currentVertexIdx + 0].texCoords;
    vertices[currentVertexIdx + 3].color = color;

    // same as 2
    vertices[currentVertexIdx + 4].position =
        vertices[currentVertexIdx + 2].position;
    vertices[currentVertexIdx + 4].texCoords =
        vertices[currentVertexIdx + 2].texCoords;
    vertices[currentVertexIdx + 4].color = color;

    vertices[currentVertexIdx + 5].position =
        origin
        + sf::Vector2f(-texSizeHalf.x, texSizeHalf.y)
              .componentWiseMul(scale)
              .rotatedBy(rotation);
    vertices[currentVertexIdx + 5].texCoords =
        textureRect.position + sf::Vector2f(0.f, textureRect.size.y);
    vertices[currentVertexIdx + 5].color = color;

    currentVertexIdx += 6;
}

void ShadeableRenderingPipeline2D::resizeVertexArrayIfNeeded()
{
    if (vertices.getVertexCount() < currentVertexIdx + 6)
    {
        // reserve space for extra 512 faces
        vertices.resize(vertices.getVertexCount() + 512 * 6);
    }
}
