#pragma once
#include <SFML/Graphics.hpp>

void InicializarTransicao();

void IniciarTransicaoIris();

bool TransicaoEstaAtiva();

bool TransicaoEstaNoMeio();

void AtualizarTransicao(float dt);

void DesenharTransicao(sf::RenderWindow& window);

void LimparTransicao();