#pragma once
#include <SFML/Graphics.hpp>
#include <string>

inline sf::Font*& GetFontePtr() {
    static sf::Font* f = nullptr;
    return f;
}

inline bool InicializarFonte()
{
    if (!GetFontePtr()) GetFontePtr() = new sf::Font();
    return GetFontePtr()->loadFromFile("C:/ProjectAshfall/Data/fontes/PixelatedLoveOld.ttf");
}

inline sf::Font* GetFonte()
{
    return GetFontePtr();
}

inline void LimparFonte()
{
    if (GetFontePtr()) {
        delete GetFontePtr();
        GetFontePtr() = nullptr;
    }
}