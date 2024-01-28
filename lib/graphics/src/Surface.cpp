//
// Created by Charlito33 on 26/12/2023.
// Copyright (c) 2023 Charlito33. All rights reserved.
//

#include "Surface.hpp"

#include <graphics.hpp>

#include "color.hpp"

namespace graphics
{
    Surface::Surface(const uint16_t width, const uint16_t height)
    {
        m_sprite.setColorDepth(16);

        m_sprite.createSprite(width / graphics::getRenderScale(), height / graphics::getRenderScale());
    }

    Surface::~Surface()
    {
        m_sprite.deleteSprite();
    }

    uint16_t Surface::getWidth() const
    {
        return m_sprite.width();
    }

    uint16_t Surface::getHeight() const
    {
        return m_sprite.height();
    }

    uint16_t Surface::getPixel(const uint16_t x, const uint16_t y)
    {
        return m_sprite.readPixel(x, y);
    }

    void Surface::setPixel(const uint16_t x, const uint16_t y, const uint16_t value)
    {
        m_sprite.writePixel(x, y, value);
    }

    void Surface::pushSurface(Surface *surface, const int16_t x, const int16_t y)
    {
        surface->m_sprite.pushSprite(
            &m_sprite,
            x / graphics::getRenderScale(),
            y / graphics::getRenderScale()
        );
    }

    void Surface::clear(const uint8_t r, const uint8_t g, const uint8_t b)
    {
        m_sprite.clear(lgfx::color565(r, g, b));
    }

    void Surface::clear()
    {
        m_sprite.clear();
    }

    void Surface::setColor(const uint8_t r, const uint8_t g, const uint8_t b)
    {
        m_r = r;
        m_g = g;
        m_b = b;

        m_sprite.setColor(r, g, b);
    }

    void Surface::fillRect(const int16_t x, const int16_t y, const uint16_t w, const uint16_t h)
    {
        m_sprite.fillRect(
            x / graphics::getRenderScale(),
            y / graphics::getRenderScale(),
            w / graphics::getRenderScale(),
            h / graphics::getRenderScale()
        );
    }

    void Surface::drawRect(const int16_t x, const int16_t y, const uint16_t w, const uint16_t h)
    {
        m_sprite.drawRect(
            x / graphics::getRenderScale(),
            y / graphics::getRenderScale(),
            w / graphics::getRenderScale(),
            h / graphics::getRenderScale()
        );
    }

    void Surface::fillRoundRect(const int16_t x, const int16_t y, const uint16_t w, const uint16_t h, const uint16_t r)
    {
        m_sprite.fillSmoothRoundRect(
            x / graphics::getRenderScale(),
            y / graphics::getRenderScale(),
            w / graphics::getRenderScale(),
            h / graphics::getRenderScale(),
            r
        );
    }

    void Surface::drawRoundRect(const int16_t x, const int16_t y, const uint16_t w, const uint16_t h, const uint16_t r)
    {
        m_sprite.drawRoundRect(
            x / graphics::getRenderScale(),
            y / graphics::getRenderScale(),
            w / graphics::getRenderScale(),
            h / graphics::getRenderScale(),
            r
        );
    }

    void Surface::drawLine(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2)
    {
        m_sprite.drawLine(
            x1 / graphics::getRenderScale(),
            y1 / graphics::getRenderScale(),
            x2 / graphics::getRenderScale(),
            y2 / graphics::getRenderScale()
        );
    }

    void Surface::drawImage(const Image& image, const int16_t x, const int16_t y)
    {
        switch (image.getType())
        {
        case BMP:
            m_sprite.drawBmp(image.getData(), image.getSize(), x, y, static_cast<int32_t>(image.getWidth()), static_cast<int32_t>(image.getHeight()));
            break;
        case PNG:
            m_sprite.drawPng(image.getData(), image.getSize(), x, y, static_cast<int32_t>(image.getWidth()), static_cast<int32_t>(image.getHeight()));
            break;
        case JPG:
            m_sprite.drawJpg(image.getData(), image.getSize(), x, y, static_cast<int32_t>(image.getWidth()), static_cast<int32_t>(image.getHeight()));
            break;
        }
    }

    void Surface::setTextScale(const uint8_t scale)
    {
        // Even if LovyanGFX supports "floating point scaling"
        // we don't use it, because half-pixels are not a thing yet
        m_sprite.setTextSize(scale);
    }

    void Surface::drawText(const std::string& text, const int16_t x, const int16_t y)
    {
        m_sprite.setTextColor(packRGB565(m_r, m_g, m_b)); // Provide only the foreground color, background is transparent
        m_sprite.drawString(text.c_str(), x, y);
    }

    void Surface::drawTextCentered(const std::string& text, const int16_t x, const int16_t y, const uint16_t w)
    {
        const uint16_t textWidth = m_sprite.textWidth(text.c_str());

        if (w == -1)
        {
            drawText(text, static_cast<int16_t>(x + getScreenWidth() * 0.5 - textWidth * 0.5), y);
        }
        else
        {
            drawText(text, static_cast<int16_t>(x + w * 0.5 - textWidth * 0.5), y);
        }
    }

    void Surface::blur(uint8_t radius)
    {
        // Copy
        auto copy = lgfx::LGFX_Sprite();
        copy.createSprite(getWidth(), getHeight());

        // Apply blur effect
        for (uint16_t x = radius; x < getWidth() - radius; x++)
        {
            for (uint16_t y = radius; y < getHeight() - radius; y++)
            {
                uint64_t sumR = 0;
                uint64_t sumG = 0;
                uint64_t sumB = 0;

                const uint8_t blurSize = pow((radius * 2 + 1), 2);

                // Read nearby pixel values
                for (uint8_t i = 0; i <= blurSize; i++)
                {
                    const uint8_t dx = i % (radius * 2 + 1);
                    const uint8_t dy = i / (radius * 2 + 1);

                    const uint16_t color = m_sprite.readPixel(x + dx, y + dy);

                    uint8_t r, g, b;
                    unpackRGB565(color, &r, &g, &b);

                    sumR += r;
                    sumG += g;
                    sumB += b;
                }

                sumR /= blurSize;
                sumG /= blurSize;
                sumB /= blurSize;

                uint16_t color = packRGB565(sumR, sumG, sumB);

                copy.writePixel(x, y, color);
            }
        }

        // Update the Surface
        copy.pushSprite(&m_sprite, 0, 0);
    }
} // graphics