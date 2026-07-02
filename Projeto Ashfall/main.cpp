#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "Menu02.h"
#include "MenuAjustes.h"
#include "MenuCreditos.h"
#include "Transicao.h"
#include "Audio.h"
#include "Janela.h"
#include "Resolucao.h"
#include "Elena.h"
#include "TileMap.h"
#include "Projetil.h"
#include "Zumbi.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

static const std::string PATH_TMX = "C:/ProjectAshfall/Data/Tiles/fase1_los_angeles_4.tmx";

static const float ZOOM_CAMERA = 1.5f;

enum class EstadoJogo {
    TelaTitulo,
    MenuPrincipal,
    Ajustes,
    Creditos,
    Jogo
};

static void AplicarConfiguracoes(sf::RenderWindow& window, ModoJanela& modoAtual, sf::VideoMode desktop)
{
    if (gModoTelaIdx == 0)      modoAtual = ModoJanela::JanelaNormal;
    else if (gModoTelaIdx == 1) modoAtual = ModoJanela::TelaCheiaJanela;
    else                        modoAtual = ModoJanela::TelaCheiaReal;
    CriarJanela(window, modoAtual, desktop);
    if (modoAtual == ModoJanela::TelaCheiaJanela) {
        LarguraRef() = std::min((float)desktop.width, 1920.f);
        AlturaRef() = std::min((float)desktop.height, 1080.f);
    }
    else {
        InicializarResolucao(desktop);
    }
    AplicarView(window);
}

static sf::Vector2f DirecaoParaVetor(DirecaoElena dir, float& rotOut)
{
    switch (dir) {
    case DirecaoElena::Tras:     rotOut = 90.f; return { 0.f,    1.f };
    case DirecaoElena::Frente:   rotOut = 270.f; return { 0.f,   -1.f };
    case DirecaoElena::Direita:  rotOut = 0.f; return { 1.f,    0.f };
    case DirecaoElena::Esquerda: rotOut = 180.f; return { -1.f,    0.f };
    case DirecaoElena::NE:       rotOut = 315.f; return { 0.707f,-0.707f };
    case DirecaoElena::NO:       rotOut = 225.f; return { -0.707f,-0.707f };
    case DirecaoElena::SE:       rotOut = 45.f; return { 0.707f, 0.707f };
    case DirecaoElena::SO:       rotOut = 135.f; return { -0.707f, 0.707f };
    default:                     rotOut = 270.f; return { 0.f,   -1.f };
    }
}

int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    CarregarConfig(desktop);

    ModoJanela modoAtual;
    if (gModoTelaIdx == 0)      modoAtual = ModoJanela::JanelaNormal;
    else if (gModoTelaIdx == 1) modoAtual = ModoJanela::TelaCheiaJanela;
    else                        modoAtual = ModoJanela::TelaCheiaReal;

    sf::RenderWindow window;
    CriarJanela(window, modoAtual, desktop);

    if (modoAtual == ModoJanela::TelaCheiaJanela) {
        LarguraRef() = std::min((float)desktop.width, 1920.f);
        AlturaRef() = std::min((float)desktop.height, 1080.f);
    }
    else {
        InicializarResolucao(desktop);
    }
    AplicarView(window);

    EstadoJogo estado = EstadoJogo::TelaTitulo;
    InicializarMenu();
    InicializarTransicao();
    InicializarAudio();
    TocarMusicaTitulo();

    static sf::Clock            clockMenu;
    static sf::Clock            clockJogo;
    static Elena                elena;
    static TileMap              mapa;
    static GerenciadorProjeteis projeteis;
    static GerenciadorZumbis    zumbis;
    static sf::View             cameraJogo;
    static bool                 jogoInicializado = false;
    static bool                 atirandoAnterior = false;

    static sf::RenderTexture    mundoRT;
    static bool                 rtCriado = false;
    static unsigned int         rtW = 0;
    static unsigned int         rtH = 0;

    while (window.isOpen())
    {
        if (estado == EstadoJogo::TelaTitulo)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) window.close();
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                    else if (event.key.code == sf::Keyboard::F11) {
                        if (modoAtual != ModoJanela::TelaCheiaReal)
                            modoAtual = ModoJanela::TelaCheiaReal;
                        else
                            modoAtual = ModoJanela::JanelaNormal;
                        CriarJanela(window, modoAtual, desktop);
                        InicializarResolucao(desktop);
                        AplicarView(window);
                    }
                    else {
                        TocarSomSelecionar();
                        estado = EstadoJogo::MenuPrincipal;
                        InicializarMenuPrincipal();
                        InicializarMusicaMenu();
                        IniciarTransicaoIris();
                        clockMenu.restart();
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    TocarSomSelecionar();
                    estado = EstadoJogo::MenuPrincipal;
                    InicializarMenuPrincipal();
                    InicializarMusicaMenu();
                    IniciarTransicaoIris();
                    clockMenu.restart();
                }
            }
            AtualizarEDesenharMenu(window);
        }
        else if (estado == EstadoJogo::MenuPrincipal)
        {
            float dt = clockMenu.restart().asSeconds();
            if (dt > 0.1f) dt = 0.1f;
            AtualizarMusicaMenu(dt);

            bool ativo = AtualizarEDesenharMenuPrincipal(window);
            if (!ativo && window.isOpen()) {
                int  botao = ObterBotaoSelecionadoMenu02();
                bool temSave = std::ifstream("C:/ProjectAshfall/Data/save.dat").good();
                int idxNovo = temSave ? 1 : 0;
                int idxAjustes = temSave ? 2 : 1;
                int idxCreditos = temSave ? 3 : 2;
                int idxSair = temSave ? 4 : 3;
                if (botao == idxNovo) {
                    estado = EstadoJogo::Jogo;
                    jogoInicializado = false;
                    clockJogo.restart();
                }
                else if (botao == idxAjustes) {
                    estado = EstadoJogo::Ajustes;
                    InicializarAjustes(desktop);
                }
                else if (botao == idxCreditos) {
                    estado = EstadoJogo::Creditos;
                    InicializarCreditos(desktop);
                }
                else if (botao == idxSair) {
                    window.close();
                }
            }
        }
        else if (estado == EstadoJogo::Ajustes)
        {
            float dt = clockMenu.restart().asSeconds();
            if (dt > 0.1f) dt = 0.1f;
            AtualizarMusicaMenu(dt);
            bool ativo = AtualizarEDesenharAjustes(window);
            if (!ativo && window.isOpen()) {
                ModoJanela modoNovo;
                if (gModoTelaIdx == 0)      modoNovo = ModoJanela::JanelaNormal;
                else if (gModoTelaIdx == 1) modoNovo = ModoJanela::TelaCheiaJanela;
                else                        modoNovo = ModoJanela::TelaCheiaReal;
                if (modoNovo != modoAtual)
                    AplicarConfiguracoes(window, modoAtual, desktop);
                estado = EstadoJogo::MenuPrincipal;
                clockMenu.restart();
            }
        }
        else if (estado == EstadoJogo::Creditos)
        {
            float dt = clockMenu.restart().asSeconds();
            if (dt > 0.1f) dt = 0.1f;
            AtualizarMusicaMenu(dt);
            bool ativo = AtualizarEDesenharCreditos(window);
            if (!ativo && window.isOpen()) {
                LimparCreditos();
                estado = EstadoJogo::MenuPrincipal;
                clockMenu.restart();
            }
        }
        else if (estado == EstadoJogo::Jogo)
        {
            if (!jogoInicializado) {
                elena.Carregar();
                mapa.Carregar(PATH_TMX);
                projeteis.Carregar();
                zumbis.Carregar();

                elena.SetTileMap(&mapa);
                projeteis.SetTileMap(&mapa);
                zumbis.SetTileMap(&mapa);
                zumbis.SpawnarAleatorio(10, mapa.GetLarguraPx(), mapa.GetAlturaPx());

                sf::Vector2f spawn = mapa.GetSpawnElena();
                elena.SetPosicao(spawn.x, spawn.y);

                unsigned int novaW = (unsigned int)(LARGURA_REF * ZOOM_CAMERA);
                unsigned int novaH = (unsigned int)(ALTURA_REF * ZOOM_CAMERA);
                if (novaW % 2 != 0) novaW++;
                if (novaH % 2 != 0) novaH++;

                cameraJogo.setSize((float)novaW, (float)novaH);
                cameraJogo.setCenter(spawn.x, spawn.y);

                if (!rtCriado || novaW != rtW || novaH != rtH) {
                    mundoRT.create(novaW, novaH);
                  
                    mundoRT.setSmooth(true);
                    rtW = novaW;
                    rtH = novaH;
                    rtCriado = true;
                }

                jogoInicializado = true;
                atirandoAnterior = false;
                clockJogo.restart();
            }

            float dt = clockJogo.restart().asSeconds();
            if (dt > 0.1f) dt = 0.1f;

       
            {
                static float histDt[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
                static int   idxDt = 0;
                static bool  histCheio = false;
                histDt[idxDt] = dt;
                idxDt = (idxDt + 1) % 16;
                if (idxDt == 0) histCheio = true;
                int n = histCheio ? 16 : idxDt;
                if (n > 0) {
                    float soma = 0.f;
                    for (int i = 0; i < n; i++) soma += histDt[i];
                    dt = soma / (float)n;
                }
            }

            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    estado = EstadoJogo::MenuPrincipal;
                    clockMenu.restart();
                }
                elena.ProcessarEvento(event);
            }

            elena.Atualizar(dt, mapa.GetLarguraPx(), mapa.GetAlturaPx());

            bool atirandoAgora = elena.GetAtirando();
            if (atirandoAgora && !atirandoAnterior) {
                sf::Vector2f pos = elena.GetPosicao();
                float rot = 0.f;
                sf::Vector2f dir = DirecaoParaVetor(elena.GetDirecao(), rot);
                sf::Vector2f offset;
                bool movendo = sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
                if (movendo) {
                    switch (elena.GetDirecao()) {
                    case DirecaoElena::Tras:     offset = { 61.f, 162.f }; break;
                    case DirecaoElena::Frente:   offset = { 85.f,  65.f }; break;
                    case DirecaoElena::Direita:  offset = { 144.f,  93.f }; break;
                    case DirecaoElena::Esquerda: offset = { 1.f,  90.f }; break;
                    case DirecaoElena::NE:       offset = { 139.f,  59.f }; break;
                    case DirecaoElena::NO:       offset = { 19.f,  49.f }; break;
                    case DirecaoElena::SE:       offset = { 132.f, 133.f }; break;
                    case DirecaoElena::SO:       offset = { 16.f, 122.f }; break;
                    default:                     offset = { 72.f,  96.f }; break;
                    }
                }
                else {
                    switch (elena.GetDirecao()) {
                    case DirecaoElena::Tras:     offset = { 61.f, 162.f }; break;
                    case DirecaoElena::Frente:   offset = { 85.f,  65.f }; break;
                    case DirecaoElena::Direita:  offset = { 149.f,  95.f }; break;
                    case DirecaoElena::Esquerda: offset = { 6.f,  94.f }; break;
                    case DirecaoElena::NE:       offset = { 139.f,  59.f }; break;
                    case DirecaoElena::NO:       offset = { 19.f,  49.f }; break;
                    case DirecaoElena::SE:       offset = { 132.f, 133.f }; break;
                    case DirecaoElena::SO:       offset = { 16.f, 122.f }; break;
                    default:                     offset = { 72.f,  96.f }; break;
                    }
                }
                projeteis.Disparar(pos + offset, dir, rot);
            }
            atirandoAnterior = atirandoAgora;

            projeteis.Atualizar(dt, mapa.GetLarguraPx(), mapa.GetAlturaPx());

            sf::Vector2f centroElena = elena.GetPosicao() + sf::Vector2f(24.f, 96.f);
            auto hitboxProjeteis = projeteis.GetHitboxes();
            zumbis.Atualizar(dt, centroElena, hitboxProjeteis);

            sf::Vector2f pos = elena.GetPosicao();
            float cx = pos.x + 24.f;
            float cy = pos.y + 48.f;
            float halfW = cameraJogo.getSize().x / 2.f;
            float halfH = cameraJogo.getSize().y / 2.f;
            float mapaW = (float)mapa.GetLarguraPx();
            float mapaH = (float)mapa.GetAlturaPx();
            if (mapaW > 2.f * halfW) {
                float maxX = mapaW - halfW;
                cx = std::max(halfW, std::min(cx, maxX));
            }
            else {
                cx = mapaW / 2.f;
            }
            if (mapaH > 2.f * halfH) {
                float maxY = mapaH - halfH;
                cy = std::max(halfH, std::min(cy, maxY));
            }
            else {
                cy = mapaH / 2.f;
            }
            
            static float camX = cx;
            static float camY = cy;
            static bool  camInit = false;
            if (!camInit) { camX = cx; camY = cy; camInit = true; }

            float suav = 1.f - std::pow(0.02f, dt);
            camX += (cx - camX) * suav;
            camY += (cy - camY) * suav;

            cameraJogo.setCenter(std::floor(camX), std::floor(camY));

            mundoRT.clear(sf::Color(20, 20, 20));
            mundoRT.setView(cameraJogo);
            mapa.Desenhar(mundoRT, cameraJogo);

            sf::Vector2f posElenaAtual = elena.GetPosicao();
            int qtdZumbis = zumbis.GetQuantidade();
            std::vector<std::pair<float, int>> ordem;
            for (int i = 0; i < qtdZumbis; i++)
                ordem.push_back({ zumbis.GetPosicaoZumbi(i).y, i });
            std::sort(ordem.begin(), ordem.end());

            bool elenaDesenhada = false;
            for (int k = 0; k < (int)ordem.size(); k++) {
                float y = ordem[k].first;
                int   idx = ordem[k].second;
                if (!elenaDesenhada && posElenaAtual.y <= y) {
                    elena.Desenhar(mundoRT);
                    elenaDesenhada = true;
                }
                zumbis.DesenharZumbi(mundoRT, idx);
            }
            if (!elenaDesenhada)
                elena.Desenhar(mundoRT);

            projeteis.Desenhar(mundoRT);

            mundoRT.display();

            window.clear(sf::Color(20, 20, 20));
            window.setView(window.getDefaultView());
            sf::Sprite spriteMundo(mundoRT.getTexture());
            sf::Vector2u tamJanela = window.getSize();
            float escX = (float)tamJanela.x / (float)rtW;
            float escY = (float)tamJanela.y / (float)rtH;
            float esc = std::min(escX, escY);

            if (tamJanela.x == rtW && tamJanela.y == rtH) esc = 1.f;
            spriteMundo.setScale(esc, esc);
            float offX = std::floor((tamJanela.x - rtW * esc) / 2.f);
            float offY = std::floor((tamJanela.y - rtH * esc) / 2.f);
            spriteMundo.setPosition(offX, offY);
            window.draw(spriteMundo);

            window.display();
        }
    }

    LimparRecursosMenu();
    LimparMenuPrincipal();
    LimparAjustes();
    LimparCreditos();
    LimparTransicao();
    LimparAudio();
    return 0;
}