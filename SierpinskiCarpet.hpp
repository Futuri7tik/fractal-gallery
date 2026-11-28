#pragma once
#include "Fractal.hpp"
#include <algorithm>

class SierpinskiCarpet : public Fractal {
public:
    explicit SierpinskiCarpet(sf::Vector2u size) : Fractal(size) {
        SierpinskiCarpet::reset();
    }

    void reset() override {
        m_depth = 5;
        m_needsUpdate = true;
    }

    std::string getName() const override {
        return "Sierpinski Carpet (Depth: " + std::to_string(m_depth) + ")";
    }

    void handleEvent(const sf::Event& event) override {
        if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Up && m_depth < 7) {
                ++m_depth;
                m_needsUpdate = true;
            } else if (key->code == sf::Keyboard::Key::Down && m_depth > 1) {
                --m_depth;
                m_needsUpdate = true;
            }
        }
    }

    void generate() override {
        if (!m_needsUpdate) return;
        m_vertices.clear();

        float side = static_cast<float>(std::min(m_size.x, m_size.y)) * 0.85f;
        float startX = (m_size.x - side) / 2.f;
        float startY = (m_size.y - side) / 2.f;

        // Рисуем сплошной базовый квадрат, поверх которого будем "вырезать" центральные
        addQuad(startX, startY, side, sf::Color(40, 180, 220));
        subdivide(startX, startY, side, m_depth);

        m_needsUpdate = false;
    }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_vertices, states);
    }

private:
    void subdivide(float x, float y, float size, int depth) {
        if (depth == 0) return;

        float subSize = size / 3.f;

        // Вырезаем центральный квадрат (красим в цвет фона)
        addQuad(x + subSize, y + subSize, subSize, sf::Color(15, 15, 22));

        // Рекурсивно обходим 8 окружающих квадратов
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (row == 1 && col == 1) continue;
                subdivide(x + col * subSize, y + row * subSize, subSize, depth - 1);
            }
        }
    }

    void addQuad(float x, float y, float size, sf::Color color) {
        sf::Vertex v0{{x, y}, color};
        sf::Vertex v1{{x + size, y}, color};
        sf::Vertex v2{{x + size, y + size}, color};
        sf::Vertex v3{{x, y + size}, color};

        // Два треугольника формируют квадрат
        m_vertices.append(v0);
        m_vertices.append(v1);
        m_vertices.append(v2);

        m_vertices.append(v0);
        m_vertices.append(v2);
        m_vertices.append(v3);
    }

    sf::VertexArray m_vertices{sf::PrimitiveType::Triangles};
    int m_depth = 5;
};
