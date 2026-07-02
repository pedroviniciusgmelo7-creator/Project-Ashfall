#include "Janela.h"

void CriarJanela(sf::RenderWindow& window, ModoJanela modo, sf::VideoMode desktop)
{
    unsigned int janelaW = (unsigned int)(desktop.width * 0.8f);
    unsigned int janelaH = (unsigned int)(desktop.height * 0.8f);

    window.close();

    switch (modo)
    {
    case ModoJanela::TelaCheiaReal:
        window.create(desktop, "Project Ashfall", sf::Style::Fullscreen);
        break;

    case ModoJanela::TelaCheiaJanela:
        window.create(
            sf::VideoMode(desktop.width, desktop.height - 40),
            "Project Ashfall",
            sf::Style::Default);
        window.setPosition(sf::Vector2i(0, 0));
        break;

    case ModoJanela::JanelaNormal:
        window.create(
            sf::VideoMode(janelaW, janelaH),
            "Project Ashfall",
            sf::Style::Default);
        window.setPosition(sf::Vector2i(
            (desktop.width - janelaW) / 2,
            (desktop.height - janelaH) / 2));
        break;
    }

    window.setFramerateLimit(60);

    sf::Vector2u novoTamanho = window.getSize();
    sf::View novaView(sf::FloatRect(0, 0,
        (float)novoTamanho.x,
        (float)novoTamanho.y));
    window.setView(novaView);
}

void TratarTeclaJanela(sf::Keyboard::Key tecla, sf::RenderWindow& window, ModoJanela& modoAtual, sf::VideoMode desktop)
{
    if (tecla == sf::Keyboard::F11) {
        if (modoAtual != ModoJanela::TelaCheiaReal) {
            modoAtual = ModoJanela::TelaCheiaReal;
            CriarJanela(window, modoAtual, desktop);
        }
        else {
            modoAtual = ModoJanela::JanelaNormal;
            CriarJanela(window, modoAtual, desktop);
        }
    }
}
