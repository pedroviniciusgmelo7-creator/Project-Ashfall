#pragma once
#include <SFML/Graphics.hpp>

inline float& LarguraRef() {
    static float v = 1920.f;
    return v;
}

inline float& AlturaRef() {
    static float v = 1080.f;
    return v;
}

#define LARGURA_REF LarguraRef()
#define ALTURA_REF  AlturaRef()

inline void InicializarResolucao(sf::VideoMode desktop)
{
    LarguraRef() = (float)desktop.width;
    AlturaRef() = (float)desktop.height;
}

inline void AplicarView(sf::RenderWindow& window)
{
    sf::View view(sf::FloatRect(0, 0, LARGURA_REF, ALTURA_REF));
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    window.setView(view);
}