#pragma once
#include <SFML/Graphics.hpp>

extern float POS_NOW_PLAYING_Y;

void InicializarAudio();

void TocarMusicaTitulo();
void PararMusicaTitulo();

void InicializarMusicaMenu();
void AtualizarMusicaMenu(float dt);
void PararMusicaMenu();

void TocarSomNavegar();
void TocarSomSelecionar();
void AtualizarVolumeMusica(int volume);
void AtualizarVolumeInterface(int volume);
void ProximaMusica();
void MusicaAnterior();

void DesenharNowPlaying(sf::RenderWindow& window, float dt);

void LimparAudio();