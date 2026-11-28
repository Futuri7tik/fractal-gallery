#pragma once
#include "PixelFractal.hpp"
#include <cmath>

class Mandelbrot : public PixelFractal {
public:
    explicit Mandelbrot(sf::Vector2u size) : PixelFractal(size) {
        Mandelbrot::reset();
    }

    void reset() override {
        m_centerX = -0.5;
        m_centerY = 0.0;
        m_scale = 3.0 / m_size.x;
        m_maxIter = 150;
        m_needsUpdate = true;
    }

    std::string getName() const override {
        return "Mandelbrot Set (Iter: " + std::to_string(m_maxIter) + ")";
    }

protected:
    sf::Color computePixel(double c_re, double c_im) const override {
        double z_re = 0.0;
        double z_im = 0.0;
        int iter = 0;

        while (z_re * z_re + z_im * z_im <= 4.0 && iter < m_maxIter) {
            double next_re = z_re * z_re - z_im * z_im + c_re;
            z_im = 2.0 * z_re * z_im + c_im;
            z_re = next_re;
            ++iter;
        }

        if (iter == m_maxIter) {
            return sf::Color::Black;
        }

        double log_zn = std::log(z_re * z_re + z_im * z_im) / 2.0;
        double nu = std::log(log_zn / std::log(2.0)) / std::log(2.0);
        double smoothIter = iter + 1.0 - nu;

        double t = std::fmod(smoothIter * 0.02, 1.0);
        if (t < 0.0) t += 1.0;

        auto r = static_cast<std::uint8_t>(9 * (1 - t) * t * t * t * 255);
        auto g = static_cast<std::uint8_t>(15 * (1 - t) * (1 - t) * t * t * 255);
        auto b = static_cast<std::uint8_t>(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);

        return sf::Color(r, g, b);
    }
};
