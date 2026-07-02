#pragma once

#include <SFML/Graphics.hpp>
#include <string>

void InicializarMenu();
void AtualizarEDesenharMenu(sf::RenderWindow& window);
void LimparRecursosMenu();
bool ObterResolucaoVideo(const std::string& caminho, int& w, int& h);
