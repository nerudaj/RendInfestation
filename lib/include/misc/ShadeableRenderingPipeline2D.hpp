#include <DGM/classes/Window.hpp>
#include <SFML/Graphics.hpp>

class [[nodiscard]] ShadeableRenderingPipeline2D final
{
public:
    ShadeableRenderingPipeline2D(
        const sf::Texture& texture, const sf::Shader& shader);

    explicit ShadeableRenderingPipeline2D(const sf::Texture& texture);

public:
    void clear()
    {
        vertices.clear();
        currentVertexIdx = 0;
    }

    void addFace(
        const sf::Vector2f& origin,
        const sf::FloatRect& textureRect,
        const sf::Angle& rotation = sf::degrees(0),
        const sf::Vector2f& scale = sf::Vector2f { 1.f, 1.f });

    void renderTo(dgm::Window& window)
    {
        window.getSfmlWindowContext().draw(vertices, renderStates);
    }

private:
    void resizeVertexArrayIfNeeded();

private:
    sf::VertexArray vertices;
    size_t currentVertexIdx = 0;
    sf::RenderStates renderStates;
};
