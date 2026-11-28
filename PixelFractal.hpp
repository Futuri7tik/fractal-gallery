#pragma once
#include "Fractal.hpp"
#include <algorithm>
#include <cstdint>
#include <thread>
#include <vector>

class PixelFractal : public Fractal {
public:
    explicit PixelFractal(sf::Vector2u size)
        : Fractal(size),
          m_pixels(size.x * size.y * 4, 255),
          m_texture(size),
          m_sprite(m_texture) {}

    void generate() override {
        if (!m_needsUpdate) return;

        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0)
            numThreads = 4;

        std::vector<std::thread> workers;
        workers.reserve(numThreads);

        unsigned int rowsPerThread = m_size.y / numThreads;

        for (unsigned int t = 0; t < numThreads; ++t) {
            unsigned int startY = t * rowsPerThread;
            unsigned int endY = (t == numThreads - 1) ? m_size.y : startY + rowsPerThread;

            workers.emplace_back([this, startY, endY]() {
                for (unsigned int y = startY; y < endY; ++y) {
                    double im = m_centerY + (static_cast<double>(y) - m_size.y / 2.0) * m_scale;
                    for (unsigned int x = 0; x < m_size.x; ++x) {
                        double re = m_centerX + (static_cast<double>(x) - m_size.x / 2.0) * m_scale;

                        sf::Color color = computePixel(re, im);

                        std::size_t idx = (static_cast<std::size_t>(y) * m_size.x + x) * 4;
                        m_pixels[idx]     = color.r;
                        m_pixels[idx + 1] = color.g;
                        m_pixels[idx + 2] = color.b;
                        m_pixels[idx + 3] = 255;
                    }
                }
            });
        }

        for (auto& worker : workers) {
            worker.join();
        }

        m_texture.update(m_pixels.data());
        m_needsUpdate = false;
    }

    void handleEvent(const sf::Event& event) override {
        if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
            if (wheel->wheel == sf::Mouse::Wheel::Vertical) {
                double mouseReBefore = m_centerX + (wheel->position.x - m_size.x / 2.0) * m_scale;
                double mouseImBefore = m_centerY + (wheel->position.y - m_size.y / 2.0) * m_scale;

                double zoomFactor = (wheel->delta > 0) ? 0.8 : 1.25;
                m_scale *= zoomFactor;

                // Сдвигаем центр так, чтобы точка под курсором осталась на месте
                m_centerX = mouseReBefore - (wheel->position.x - m_size.x / 2.0) * m_scale;
                m_centerY = mouseImBefore - (wheel->position.y - m_size.y / 2.0) * m_scale;

                m_needsUpdate = true;
            }
        }
        // 2. Начало перетаскивания (ЛКМ)
        else if (const auto* btn = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (btn->button == sf::Mouse::Button::Left) {
                m_isDragging = true;
                m_lastMousePos = btn->position;
            }
        }
        // 3. Окончание перетаскивания
        else if (const auto* btn = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (btn->button == sf::Mouse::Button::Left) {
                m_isDragging = false;
            }
        }
        // 4. Сдвиг камеры при движении мыши
        else if (const auto* move = event.getIf<sf::Event::MouseMoved>()) {
            if (m_isDragging) {
                sf::Vector2i delta = move->position - m_lastMousePos;
                m_centerX -= delta.x * m_scale;
                m_centerY -= delta.y * m_scale;
                m_lastMousePos = move->position;
                m_needsUpdate = true;
            }
        }
        // 5. Изменение числа итераций стрелками Вверх/Вниз
        else if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Up) {
                m_maxIter += 50;
                m_needsUpdate = true;
            } else if (key->code == sf::Keyboard::Key::Down && m_maxIter > 50) {
                m_maxIter -= 50;
                m_needsUpdate = true;
            }
        }
    }

protected:
    virtual sf::Color computePixel(double re, double im) const = 0;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_sprite, states);
    }

    double m_centerX = 0.0;
    double m_centerY = 0.0;
    double m_scale = 0.004;
    int m_maxIter = 150;

private:
    std::vector<std::uint8_t> m_pixels;
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    bool m_isDragging = false;
    sf::Vector2i m_lastMousePos{0, 0};
};
