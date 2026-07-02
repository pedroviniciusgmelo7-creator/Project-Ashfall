#pragma once
#include <SFML/Graphics.hpp>

enum class ModoJanela {
    TelaCheiaReal,
    TelaCheiaJanela,
    JanelaNormal
};

void CriarJanela(sf::RenderWindow& window, ModoJanela modo, sf::VideoMode desktop);
void TratarTeclaJanela(sf::Keyboard::Key tecla, sf::RenderWindow& window, ModoJanela& modoAtual, sf::VideoMode desktop);