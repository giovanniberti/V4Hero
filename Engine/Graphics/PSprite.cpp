#include "PSprite.h"
#include <fstream>
#include <iostream>
#include <string>

PSprite::PSprite(const PSprite& other)
    : t(other.t),
      s(other.s),
      texname(other.texname),
      qualitySetting(other.qualitySetting),
      resSetting(other.resSetting),
      ratioX(other.ratioX),
      ratioY(other.ratioY),
      resRatioX(other.resRatioX),
      resRatioY(other.resRatioY),
      scaleX(other.scaleX),
      scaleY(other.scaleY),
      orX(other.orX),
      orY(other.orY),
      baseX(other.baseX),
      baseY(other.baseY),
      lx(other.lx),
      ly(other.ly),
      angle(other.angle),
      exported(other.exported)
{
    s.setTexture(t, true);
}

PSprite& PSprite::operator=(PSprite other)
{
    swap(*this, other);
    return *this;
}

void swap(PSprite& first, PSprite& second)
{
    using std::swap;
    swap(first.t, second.t);
    swap(first.s, second.s);
    swap(first.texname, second.texname);
    swap(first.qualitySetting, second.qualitySetting);
    swap(first.resSetting, second.resSetting);
    swap(first.ratioX, second.ratioX);
    swap(first.ratioY, second.ratioY);
    swap(first.resRatioX, second.resRatioX);
    swap(first.resRatioY, second.resRatioY);
    swap(first.scaleX, second.scaleX);
    swap(first.scaleY, second.scaleY);
    swap(first.orX, second.orX);
    swap(first.orY, second.orY);
    swap(first.baseX, second.baseX);
    swap(first.baseY, second.baseY);
    swap(first.lx, second.lx);
    swap(first.ly, second.ly);
    swap(first.angle, second.angle);
    swap(first.exported, second.exported);
}

void PSprite::loadFromFile(std::string file, int q)
{
    std::cout << file << std::endl;
    std::string a = file.substr(0, file.find_last_of("."));
    std::string b = file.substr(file.find_last_of("."));
    std::string sq = "";

    switch (q)
    {
        case 0:
            sq = "";
            break;

        case 1:
            sq = "M";
            break;

        case 2:
            sq = "H";
            break;

        case 3:
            sq = "U";
            break;
    }

    std::string c = a + "_" + sq + b;

    qualitySetting = q;
    resSetting = 1;

    std::cout << "[PSPRITE] Loading " << c << std::endl;
    texname = c;

    if (!t.loadFromFile(c))
    {
        std::ofstream dbg("V4Hero-errors.log", std::ios::app);
        dbg << "Failed to load " << c;
        dbg << "\r\n";
        dbg.close();
    }

    t.setSmooth(true);
    s.setTexture(t, true);
}

void PSprite::loadFromFile(std::string file, int q, int r = 1)
{
    std::cout << file << std::endl;
    std::string a = file.substr(0, file.find_last_of("."));
    std::string b = file.substr(file.find_last_of("."));
    std::string sq = "";

    switch (q)
    {
        case 0:
            sq = "";
            break;

        case 1:
            sq = "M";
            break;

        case 2:
            sq = "H";
            break;

        case 3:
            sq = "U";
            break;
    }

    std::string c = a + "_" + sq + b;

    qualitySetting = q;
    resSetting = r;

    std::cout << "[PSPRITE] Loading " << c << std::endl;
    texname = c;

    if (!t.loadFromFile(c))
    {
        std::ofstream dbg("V4Hero-errors.log", std::ios::app);
        dbg << "Failed to load " << c;
        dbg << "\r\n";
        dbg.close();
    }

    t.setSmooth(true);
    s.setTexture(t, true);
}

void PSprite::setRepeated(bool r)
{
    t.setRepeated(r);
    s.setTexture(t);
}

void PSprite::setTextureRect(sf::IntRect rect)
{
    s.setTextureRect(rect);
}

void PSprite::setOrigin(float x, float y)
{
    orX = x;
    orY = y;
    s.setOrigin(orX, orY);
}

void PSprite::setScale(float x, float y)
{
    scaleX = x;
    scaleY = y;
    s.setScale(ratioX * scaleX, ratioY * scaleY);
}

void PSprite::setRotation(float a)
{
    angle = a;
}

void PSprite::setColor(sf::Color color)
{
    s.setColor(color);
}

sf::Color PSprite::getColor()
{
    return s.getColor();
}

void PSprite::setTexture(sf::Texture& texture)
{
    t = texture;
    s.setTexture(t, true);

    exported = false;
}

void PSprite::applyTexture()
{
    s.setTexture(t, true);
    exported = false;
}

void PSprite::setSprite(sf::Sprite& sprite)
{
    s = sprite;
}

void PSprite::setPosition(float x, float y)
{
    //s.setPosition(x*ratioX,y*ratioY);
    if (baseX == -999)
        baseX = x;

    if (baseY == -999)
        baseY = y;

    lx = x;
    ly = y;

    //std::cout << x << " " << y << "  " << lx << " " << ly << std::endl;
}

sf::Vector2f PSprite::getPosition()
{
    return sf::Vector2f(lx, ly);
}

void PSprite::setScale(float ss)
{
    scaleX = ss;
    scaleY = ss;
    s.setScale(ratioX * scaleX, ratioY * scaleY);
}

sf::FloatRect PSprite::getLocalBounds()
{
    return s.getLocalBounds();
}

sf::FloatRect PSprite::getGlobalBounds()
{
    return s.getGlobalBounds();
}

sf::FloatRect PSprite::getGlobalBoundsScaled()
{
    float nw = 1;
    float nh = 1;

    if (s.getGlobalBounds().width > 0)
        nw = s.getGlobalBounds().width / resRatioX;

    if (s.getGlobalBounds().height > 0)
        nh = s.getGlobalBounds().height / resRatioY;

    return sf::FloatRect(s.getGlobalBounds().left, s.getGlobalBounds().top, nw, nh);
    //return s.getGlobalBounds();
}

void PSprite::setSmooth(bool smooth)
{
    t.setSmooth(smooth);
    s.setTexture(t);
}

void PSprite::draw(sf::RenderWindow& window)
{
    switch (qualitySetting)
    {
        case 0: ///low
        {
            ratioX = window.getSize().x / float(640);
            ratioY = window.getSize().y / float(360);
            break;
        }

        case 1: ///med
        {
            ratioX = window.getSize().x / float(1280);
            ratioY = window.getSize().y / float(720);
            break;
        }

        case 2: ///high
        {
            ratioX = window.getSize().x / float(1920);
            ratioY = window.getSize().y / float(1080);
            break;
        }

        case 3: ///ultra
        {
            ratioX = window.getSize().x / float(3840);
            ratioY = window.getSize().y / float(2160);
            break;
        }
    }

    switch (resSetting)
    {
        case 0: ///low
        {
            resRatioX = window.getSize().x / float(640);
            resRatioY = window.getSize().y / float(360);
            break;
        }

        case 1: ///med
        {
            resRatioX = window.getSize().x / float(1280);
            resRatioY = window.getSize().y / float(720);
            break;
        }

        case 2: ///high
        {
            resRatioX = window.getSize().x / float(1920);
            resRatioY = window.getSize().y / float(1080);
            break;
        }

        case 3: ///ultra
        {
            resRatioX = window.getSize().x / float(3840);
            resRatioY = window.getSize().y / float(2160);
            break;
        }
    }

    s.setTexture(t);
    s.setScale(ratioX * scaleX, ratioY * scaleY);
    s.setOrigin(orX, orY);
    s.setPosition(lx * resRatioX, ly * resRatioY);
    s.setRotation(angle * (180 / 3.14159265358));
    window.draw(s);

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::F9)))
    {
        if (!exported)
        {
            if (texname.find("locationbg") != std::string::npos)
            {
                sf::Image img;
                img = t.copyToImage();
                int rrr = rand() % 100000000;
                img.saveToFile("texDump/" + std::to_string(rrr) + ".png");

                exported = true;
            }
        }
    }
}

void PSprite::update(sf::RenderWindow& window)
{
    ///Update updates the sprite without drawing it on screen. Useful for RenderTexture

    switch (qualitySetting)
    {
        case 0: ///low
        {
            ratioX = window.getSize().x / float(640);
            ratioY = window.getSize().y / float(360);
            break;
        }

        case 1: ///med
        {
            ratioX = window.getSize().x / float(1280);
            ratioY = window.getSize().y / float(720);
            break;
        }

        case 2: ///high
        {
            ratioX = window.getSize().x / float(1920);
            ratioY = window.getSize().y / float(1080);
            break;
        }

        case 3: ///ultra
        {
            ratioX = window.getSize().x / float(3840);
            ratioY = window.getSize().y / float(2160);
            break;
        }
    }

    switch (resSetting)
    {
        case 0: ///low
        {
            resRatioX = window.getSize().x / float(640);
            resRatioY = window.getSize().y / float(360);
            break;
        }

        case 1: ///med
        {
            resRatioX = window.getSize().x / float(1280);
            resRatioY = window.getSize().y / float(720);
            break;
        }

        case 2: ///high
        {
            resRatioX = window.getSize().x / float(1920);
            resRatioY = window.getSize().y / float(1080);
            break;
        }

        case 3: ///ultra
        {
            resRatioX = window.getSize().x / float(3840);
            resRatioY = window.getSize().y / float(2160);
            break;
        }
    }

    s.setTexture(t);
    s.setScale(ratioX * scaleX, ratioY * scaleY);
    s.setOrigin(orX, orY);
    s.setPosition(lx * resRatioX, ly * resRatioY);
    s.setRotation(angle * (180 / 3.14159265358));

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::F9)))
    {
        if (!exported)
        {
            sf::Image img;
            img = t.copyToImage();
            int rrr = rand() % 100000000;
            img.saveToFile("texDump/" + std::to_string(rrr) + ".png");

            exported = true;
        }
    }
}

void saveToFile(std::string file)
{
}
