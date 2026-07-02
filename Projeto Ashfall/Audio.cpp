#include "Audio.h"
#include "Resolucao.h"
#include "EstiloUI.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cstdlib>

static const std::string PATH_TITULO = "C:/ProjectAshfall/Data/musics/telainicial/silent hill 2 OST - promise (reprise) ( slowed  reverb  rain ).mp3";

static const std::string PATHS_MENU[] = {
    "C:/ProjectAshfall/Data/musics/telainicial/Daisy.mp3",
    "C:/ProjectAshfall/Data/musics/telainicial/Fallout - I Don't Want To Set The World On Fire (Tribute).mp3",
    "C:/ProjectAshfall/Data/musics/telainicial/princess chelsea - cigarette duet (slowed down).mp3",
    "C:/ProjectAshfall/Data/musics/telainicial/tonight you belong to me ( slowed  reverb ).mp3"
};

static const std::string NOMES_MENU[] = {
    "Daisy Bell",
    "I Don't Want To Set The World On Fire",
    "Cigarette Duet",
    "Tonight You Belong To Me"
};

static const int TOTAL_MUSICAS_MENU = 4;

static const std::string PATH_SOM_NAVEGAR = "C:/ProjectAshfall/Data/musics/selecionar/Retro1.mp3";
static const std::string PATH_SOM_SELECIONAR = "C:/ProjectAshfall/Data/musics/selecionar/Retro8.mp3";
static const std::string PATH_NOW_PLAYING = "C:/ProjectAshfall/Data/imagens/menu_principal/player.png";

static sf::Music* gMusicaTitulo = nullptr;
static sf::Music* gMusicaMenu = nullptr;
static sf::SoundBuffer* gBufferNavegar = nullptr;
static sf::SoundBuffer* gBufferSelecion = nullptr;
static sf::Sound* gSomNavegar = nullptr;
static sf::Sound* gSomSelecionar = nullptr;

static sf::Texture* gNowPlayingTextura = nullptr;
static sf::Sprite* gNowPlayingSprite = nullptr;
static bool         gNowPlayingOk = false;

static int   gMusicaAtualIdx = -1;
static float gTimerDelay = 0.f;
static bool  gAguardandoInicio = true;
static const float DELAY_INICIAL = 3.0f;

static int gOrdem[TOTAL_MUSICAS_MENU] = { 0, 1, 2, 3 };
static int gOrdemIdx = 0;

static float gScrollX = 0.f;
static float gScrollVel = 80.f;

float POS_NOW_PLAYING_Y = 5.f;

static void EmbaralharOrdem()
{
    for (int i = TOTAL_MUSICAS_MENU - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = gOrdem[i];
        gOrdem[i] = gOrdem[j];
        gOrdem[j] = tmp;
    }
    gOrdemIdx = 0;
}

static void TocarProximaMusica()
{
    if (gOrdemIdx >= TOTAL_MUSICAS_MENU)
        EmbaralharOrdem();

    gMusicaAtualIdx = gOrdem[gOrdemIdx];
    gOrdemIdx++;

    if (gMusicaMenu) {
        gMusicaMenu->stop();
        gMusicaMenu->openFromFile(PATHS_MENU[gMusicaAtualIdx]);
        gMusicaMenu->setLoop(false);
        gMusicaMenu->setVolume(70.f);
        gMusicaMenu->play();
    }

    gScrollX = 302.f;
}

void InicializarAudio()
{
    gMusicaTitulo = new sf::Music();
    gMusicaMenu = new sf::Music();

    gBufferNavegar = new sf::SoundBuffer();
    gBufferSelecion = new sf::SoundBuffer();

    if (gBufferNavegar->loadFromFile(PATH_SOM_NAVEGAR)) {
        gSomNavegar = new sf::Sound();
        gSomNavegar->setBuffer(*gBufferNavegar);
    }

    if (gBufferSelecion->loadFromFile(PATH_SOM_SELECIONAR)) {
        gSomSelecionar = new sf::Sound();
        gSomSelecionar->setBuffer(*gBufferSelecion);
    }

    if (!gNowPlayingTextura) {
        gNowPlayingTextura = new sf::Texture();
        if (gNowPlayingTextura->loadFromFile(PATH_NOW_PLAYING)) {
            gNowPlayingTextura->setSmooth(true);
            gNowPlayingSprite = new sf::Sprite(*gNowPlayingTextura);
            gNowPlayingOk = true;
        }
        else {
            delete gNowPlayingTextura;
            gNowPlayingTextura = nullptr;
        }
    }

    srand((unsigned int)time(nullptr));
    EmbaralharOrdem();
}

void TocarMusicaTitulo()
{
    if (!gMusicaTitulo) return;
    if (gMusicaMenu) gMusicaMenu->stop();
    gMusicaTitulo->openFromFile(PATH_TITULO);
    gMusicaTitulo->setLoop(true);
    gMusicaTitulo->setVolume(60.f);
    gMusicaTitulo->play();
}

void PararMusicaTitulo()
{
    if (gMusicaTitulo) gMusicaTitulo->stop();
}

void InicializarMusicaMenu()
{
    if (gMusicaTitulo) gMusicaTitulo->stop();
    if (gMusicaMenu)   gMusicaMenu->stop();
    gTimerDelay = 0.f;
    gAguardandoInicio = true;
    gMusicaAtualIdx = -1;
    EmbaralharOrdem();
}

static bool gTrocandoMusica = false;

void AtualizarMusicaMenu(float dt)
{
    if (gAguardandoInicio) {
        gTimerDelay += dt;
        if (gTimerDelay >= DELAY_INICIAL) {
            gAguardandoInicio = false;
            gTrocandoMusica = false;
            TocarProximaMusica();
        }
        return;
    }

    if (!gTrocandoMusica && gMusicaMenu) {
        bool deveTracar = false;

        if (gMusicaMenu->getStatus() == sf::Music::Stopped)
            deveTracar = true;

        if (gMusicaMenu->getStatus() == sf::Music::Playing) {
            sf::Time duracao = gMusicaMenu->getDuration();
            sf::Time posicao = gMusicaMenu->getPlayingOffset();
            if (duracao > sf::Time::Zero && posicao >= duracao - sf::seconds(0.5f))
                deveTracar = true;
        }

        if (deveTracar) {
            gTrocandoMusica = true;
            TocarProximaMusica();
        }
    }

    if (gTrocandoMusica && gMusicaMenu && gMusicaMenu->getStatus() == sf::Music::Playing) {
        sf::Time posicao = gMusicaMenu->getPlayingOffset();
        if (posicao > sf::seconds(1.0f))
            gTrocandoMusica = false;
    }
}

void PararMusicaMenu()
{
    if (gMusicaMenu) gMusicaMenu->stop();
    gAguardandoInicio = true;
    gTimerDelay = 0.f;
    gMusicaAtualIdx = -1;
}

void AtualizarVolumeMusica(int volume)
{
    float vol = (float)std::max(0, std::min(100, volume));
    if (gMusicaMenu)   gMusicaMenu->setVolume(vol);
    if (gMusicaTitulo) gMusicaTitulo->setVolume(vol);
}

void AtualizarVolumeInterface(int volume)
{
    float vol = (float)std::max(0, std::min(100, volume));
    if (gSomNavegar)    gSomNavegar->setVolume(vol);
    if (gSomSelecionar) gSomSelecionar->setVolume(vol);
}

void ProximaMusica()
{
    if (gAguardandoInicio) return;
    gTrocandoMusica = true;
    TocarProximaMusica();
}

void MusicaAnterior()
{
    if (gAguardandoInicio) return;
    gOrdemIdx -= 2;
    if (gOrdemIdx < 0) gOrdemIdx = TOTAL_MUSICAS_MENU - 1;
    gTrocandoMusica = true;
    TocarProximaMusica();
}

void TocarSomNavegar()
{
    if (gSomNavegar) gSomNavegar->play();
}

void TocarSomSelecionar()
{
    if (gSomSelecionar) gSomSelecionar->play();
}

void DesenharNowPlaying(sf::RenderWindow& window, float dt)
{
    if (gMusicaAtualIdx < 0 || gAguardandoInicio) return;

    float larguraPainel = LARGURA_REF * 0.36f;
    float painelX = LARGURA_REF - larguraPainel - 20.f;
    float painelY = POS_NOW_PLAYING_Y;
    float alturaPainel = larguraPainel * 0.15f;

    if (gNowPlayingOk && gNowPlayingSprite && gNowPlayingTextura) {
        sf::Vector2u ts = gNowPlayingTextura->getSize();
        float proporcao = (float)ts.y / (float)ts.x;
        alturaPainel = larguraPainel * proporcao;
        float scale = larguraPainel / (float)ts.x;
        gNowPlayingSprite->setScale(scale, scale);
        gNowPlayingSprite->setPosition(painelX, painelY);
        gNowPlayingSprite->setColor(sf::Color(255, 255, 255, 100));
        window.draw(*gNowPlayingSprite);
    }

    float textAreaX = 993.f;
    float textAreaW = 1295.f - 993.f;

    unsigned int tamTexto = (unsigned int)(LARGURA_REF * 0.018f);
    sf::Text txNome = CriarTexto(NOMES_MENU[gMusicaAtualIdx], tamTexto, sf::Color(210, 185, 130, 255));
    float larguraTexto = txNome.getLocalBounds().width;

    gScrollX -= gScrollVel * dt;
    if (gScrollX < -larguraTexto - 40.f)
        gScrollX = textAreaW + 40.f;

    sf::View viewAntiga = window.getView();

    sf::View viewClip;
    viewClip.setViewport(sf::FloatRect(
        textAreaX / LARGURA_REF,
        painelY / ALTURA_REF,
        textAreaW / LARGURA_REF,
        alturaPainel / ALTURA_REF
    ));
    viewClip.setSize(textAreaW, alturaPainel);
    viewClip.setCenter(textAreaW / 2.f, alturaPainel / 2.f);
    window.setView(viewClip);

    float posTextoX = gScrollX - textAreaW / 2.f + larguraTexto / 2.f;
    txNome.setOrigin(larguraTexto / 2.f, 0.f);
    txNome.setPosition(posTextoX, (alturaPainel - txNome.getLocalBounds().height) / 2.f - 4.f);
    window.draw(txNome);

    window.setView(viewAntiga);
}

void LimparAudio()
{
    if (gMusicaTitulo) { delete gMusicaTitulo;   gMusicaTitulo = nullptr; }
    if (gMusicaMenu) { delete gMusicaMenu;      gMusicaMenu = nullptr; }
    if (gSomNavegar) { delete gSomNavegar;      gSomNavegar = nullptr; }
    if (gSomSelecionar) { delete gSomSelecionar;   gSomSelecionar = nullptr; }
    if (gBufferNavegar) { delete gBufferNavegar;   gBufferNavegar = nullptr; }
    if (gBufferSelecion) { delete gBufferSelecion;  gBufferSelecion = nullptr; }
    if (gNowPlayingSprite) { delete gNowPlayingSprite;  gNowPlayingSprite = nullptr; }
    if (gNowPlayingTextura) { delete gNowPlayingTextura; gNowPlayingTextura = nullptr; }
    gNowPlayingOk = false;
    gMusicaAtualIdx = -1;
}