#pragma once
#include <SFML/Graphics.hpp>

void InicializarAjustes(sf::VideoMode desktop);
bool AtualizarEDesenharAjustes(sf::RenderWindow& window);
void LimparAjustes();

void CarregarConfig(sf::VideoMode desktop);
void SalvarConfig();

extern int gResolucaoIdx;
extern int gModoTelaIdx;
extern int gVolMusica;
extern int gVolEfeitos;