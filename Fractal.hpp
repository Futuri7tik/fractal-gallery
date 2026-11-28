#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Fractal : public sf::Drawable {
public:
    explicit Fractal(sf::Vector2u windowSize) : m_size(windowSize) {}
    virtual ~Fractal() = default;

    virtual void generate() = 0;
    virtual void handleEvent(const sf::Event& event) {}
    virtual void reset() = 0;
    virtual std::string getName() const = 0;

protected:
    sf::Vector2u m_size;
    bool m_needsUpdate = true;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;
};
