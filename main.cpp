#include "Mandelbrot.hpp"
#include "SierpinskiCarpet.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

int main() {
    constexpr sf::Vector2u windowSize{1900, 1080};
    sf::RenderWindow window(sf::VideoMode(windowSize), "Fractal Explorer - SFML 3.1.0");
    window.setFramerateLimit(60);

    std::vector<std::unique_ptr<Fractal>> fractals;
    fractals.push_back(std::make_unique<Mandelbrot>(windowSize));
    fractals.push_back(std::make_unique<SierpinskiCarpet>(windowSize));

    std::size_t activeIdx = 0;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Num1) {
                    activeIdx = 0;
                } else if (key->code == sf::Keyboard::Key::Num2) {
                    activeIdx = 1;
                } else if (key->code == sf::Keyboard::Key::R) {
                    fractals[activeIdx]->reset();
                }
            }

            fractals[activeIdx]->handleEvent(*event);
        }

        fractals[activeIdx]->generate();
        window.setTitle(fractals[activeIdx]->getName() + " | [1-2] Switch | [R] Reset | [Up/Down] Detail");

        window.clear(sf::Color(15, 15, 22));
        window.draw(*fractals[activeIdx]);
        window.display();
    }

    return 0;
}
