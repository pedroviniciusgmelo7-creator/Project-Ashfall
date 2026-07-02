#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Fonte.h"
#include "Resolucao.h"

inline unsigned int TamBotao() { return (unsigned int)(LARGURA_REF * 0.052f); }
inline unsigned int TamTitulo() { return (unsigned int)(LARGURA_REF * 0.072f); }
inline unsigned int TamDica() { return (unsigned int)(LARGURA_REF * 0.028f); }
inline unsigned int TamLabel() { return (unsigned int)(LARGURA_REF * 0.040f); }

inline sf::Color CorNormal() { return sf::Color(120, 85, 25, 255); }
inline sf::Color CorSelecionado() { return sf::Color(180, 140, 45, 255); }
inline sf::Color CorTitulo() { return sf::Color(160, 120, 35); }
inline sf::Color CorDica() { return sf::Color(70, 55, 25); }
inline sf::Color CorBrilho() { return sf::Color(200, 160, 50, 80); }
inline sf::Color CorSeta() { return sf::Color(160, 120, 30); }
inline sf::Color CorPainel() { return sf::Color(8, 6, 3, 220); }
inline sf::Color CorBordaPainel() { return sf::Color(100, 70, 20, 220); }

inline sf::Text CriarTexto(const std::string& str, unsigned int tam, sf::Color cor)
{
    sf::Text t;
    if (GetFonte()) t.setFont(*GetFonte());
    t.setString(str);
    t.setCharacterSize(tam);
    t.setFillColor(cor);
    return t;
}

inline sf::Text CriarBotao(const std::string& str)
{
    return CriarTexto(str, TamBotao(), CorNormal());
}

inline sf::Text CriarTitulo(const std::string& str)
{
    return CriarTexto(str, TamTitulo(), CorTitulo());
}

inline sf::Text CriarDica(const std::string& str)
{
    return CriarTexto(str, TamDica(), CorDica());
}

inline void DesenharBotao(sf::RenderWindow& window, sf::Text& texto, bool selecionado)
{
    sf::Vector2f pos = texto.getPosition();

    if (selecionado) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                sf::Text borda = texto;
                borda.setFillColor(sf::Color(30, 20, 5, 180));
                borda.setPosition(pos.x + dx, pos.y + dy);
                window.draw(borda);
            }
        }
        texto.setFillColor(CorSelecionado());
        window.draw(texto);
    }
    else {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                sf::Text borda = texto;
                borda.setFillColor(sf::Color(15, 10, 3, 120));
                borda.setPosition(pos.x + dx, pos.y + dy);
                window.draw(borda);
            }
        }
        texto.setFillColor(CorNormal());
        window.draw(texto);
    }
}

inline void DesenharSeta(sf::RenderWindow& window, float x, float y)
{
    sf::Text seta = CriarTexto(">", TamBotao(), CorSeta());
    sf::FloatRect b = seta.getLocalBounds();
    seta.setOrigin(0.f, b.height / 2.f);
    seta.setPosition(x, y);
    window.draw(seta);
}