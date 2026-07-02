#include "Menu02.h"
#include "Resolucao.h"
#include "EstiloUI.h"
#include "Transicao.h"
#include "Audio.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

static sf::Texture* gFundoTextura = nullptr;
static sf::Sprite* gFundoSprite = nullptr;
static sf::Shader* gShader = nullptr;
static bool         gShaderOk = false;
static bool         gTemSave = false;
static int          gSelecionado = 0;
static int          gTotalBotoes = 0;
static sf::Clock* gAnimClock = nullptr;

static const std::string SAVE_PATH = "C:/ProjectAshfall/Data/save.dat";
static const std::string SHADER_PATH = "C:/ProjectAshfall/Data/shaders/lanterna.frag";
static const std::string FUNDO_PATH = "C:/ProjectAshfall/Data/imagens/menu_principal/fundotela.png";

static std::vector<std::string> gLabels;
static std::vector<sf::Text>    gTextos;

static bool TemSave()
{
    std::ifstream f(SAVE_PATH);
    return f.good();
}

static sf::Texture* CarregarTextura(const std::string& path)
{
    sf::Texture* t = new sf::Texture();
    if (!t->loadFromFile(path)) { delete t; return nullptr; }
    t->setSmooth(true);
    return t;
}

static void MontarBotoes()
{
    gLabels.clear();
    gTextos.clear();

    if (gTemSave) gLabels.push_back("CONTINUAR");
    gLabels.push_back("NOVO JOGO");
    gLabels.push_back("AJUSTES");
    gLabels.push_back("CREDITOS");
    gLabels.push_back("SAIR");

    gTotalBotoes = (int)gLabels.size();

    for (auto& lbl : gLabels)
        gTextos.push_back(CriarBotao(lbl));
}

static void PosicionarBotoes()
{
    float startY = ALTURA_REF * 0.38f;
    float spacing = ALTURA_REF * 0.11f;
    float leftX = LARGURA_REF * 0.08f;

    for (int i = 0; i < (int)gTextos.size(); i++) {
        sf::FloatRect b = gTextos[i].getLocalBounds();
        gTextos[i].setOrigin(0.f, b.height / 2.f);
        gTextos[i].setPosition(leftX, startY + i * spacing);
    }
}

void InicializarMenuPrincipal()
{
    gAnimClock = new sf::Clock();
    gTemSave = TemSave();
    gSelecionado = 0;

    InicializarFonte();

    gFundoTextura = CarregarTextura(FUNDO_PATH);
    if (gFundoTextura) gFundoSprite = new sf::Sprite(*gFundoTextura);

    if (sf::Shader::isAvailable()) {
        gShader = new sf::Shader();
        if (gShader->loadFromFile(SHADER_PATH, sf::Shader::Fragment)) {
            gShader->setUniform("texture", sf::Shader::CurrentTexture);
            gShaderOk = true;
        }
        else {
            delete gShader;
            gShader = nullptr;
            gShaderOk = false;
        }
    }

    MontarBotoes();
}

bool AtualizarEDesenharMenuPrincipal(sf::RenderWindow& window)
{
    AplicarView(window);

    static sf::Clock deltaClock;
    float dt = deltaClock.restart().asSeconds();
    if (dt > 0.1f) dt = 0.1f;

    if (TransicaoEstaAtiva())
        AtualizarTransicao(dt);

    float t = gAnimClock ? gAnimClock->getElapsedTime().asSeconds() : 0.f;

    PosicionarBotoes();

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) { window.close(); return false; }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) { window.close(); return false; }
            if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                gSelecionado = (gSelecionado - 1 + gTotalBotoes) % gTotalBotoes;
                TocarSomNavegar();
            }
            if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                gSelecionado = (gSelecionado + 1) % gTotalBotoes;
                TocarSomNavegar();
            }
            if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
                TocarSomSelecionar();
                return false;
            }
            if (event.key.code == sf::Keyboard::N) ProximaMusica();
            if (event.key.code == sf::Keyboard::B) MusicaAnterior();
        }
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mp = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            for (int i = 0; i < (int)gTextos.size(); i++)
                if (gTextos[i].getGlobalBounds().contains(mp)) {
                    if (i != gSelecionado) TocarSomNavegar();
                    gSelecionado = i;
                }
        }
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mp = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            for (int i = 0; i < (int)gTextos.size(); i++)
                if (gTextos[i].getGlobalBounds().contains(mp)) {
                    gSelecionado = i;
                    TocarSomSelecionar();
                    return false;
                }
        }
    }

    float lanternaX = LARGURA_REF * 0.830f;
    float lanternaY = ALTURA_REF * 0.431f;
    float intensidade = 0.7f + 0.3f * std::sin(t * 2.5f + std::sin(t * 4.8f));
    if (intensidade < 0.1f) intensidade = 0.1f;
    if (intensidade > 1.0f) intensidade = 1.0f;

    window.clear(sf::Color::Black);

    if (gFundoSprite && gFundoTextura) {
        sf::Vector2u texSize = gFundoTextura->getSize();
        float scale = std::max(LARGURA_REF / texSize.x, ALTURA_REF / texSize.y);
        gFundoSprite->setScale(scale, scale);
        gFundoSprite->setPosition(
            (LARGURA_REF - texSize.x * scale) / 2.f,
            (ALTURA_REF - texSize.y * scale) / 2.f);

        if (gShaderOk && gShader) {
            gShader->setUniform("lanternaPos", sf::Glsl::Vec2(lanternaX, lanternaY));
            gShader->setUniform("intensidade", intensidade);
            gShader->setUniform("resolucao", sf::Glsl::Vec2(LARGURA_REF, ALTURA_REF));
            gShader->setUniform("tempo", t);
            window.draw(*gFundoSprite, gShader);
        }
        else {
            window.draw(*gFundoSprite);
            sf::RectangleShape filtro(sf::Vector2f(LARGURA_REF, ALTURA_REF));
            filtro.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(filtro);
        }
    }

    float startY = ALTURA_REF * 0.38f;
    float spacing = ALTURA_REF * 0.11f;
    float leftX = LARGURA_REF * 0.08f;

    for (int i = 0; i < (int)gTextos.size(); i++) {
        DesenharBotao(window, gTextos[i], i == gSelecionado);
        if (i == gSelecionado)
            DesenharSeta(window, leftX - TamBotao() * 0.6f, startY + i * spacing);
    }

    DesenharNowPlaying(window, dt);
    DesenharTransicao(window);

    window.display();
    return true;
}

int ObterBotaoSelecionadoMenu02()
{
    return gSelecionado;
}

void LimparMenuPrincipal()
{
    if (gShader) { delete gShader;      gShader = nullptr; }
    if (gFundoSprite) { delete gFundoSprite; gFundoSprite = nullptr; }
    if (gFundoTextura) { delete gFundoTextura; gFundoTextura = nullptr; }
    if (gAnimClock) { delete gAnimClock;   gAnimClock = nullptr; }
    gTextos.clear();
    gLabels.clear();
    LimparFonte();
}