#include "MenuCreditos.h"
#include "Resolucao.h"
#include "Janela.h"
#include "EstiloUI.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

static const std::string FUNDO_PATH = "C:/ProjectAshfall/Data/imagens/menu_principal/fundotela.png";

static sf::Texture* gFundoTextura = nullptr;
static sf::Sprite* gFundoSprite = nullptr;

static sf::Clock gClockCreditos;
static float gScrollY = 0.f;
static float gVelocidadeScroll = 60.f;
static bool gPausado = false;

struct LinhaCredito {
    std::string texto;
    bool titulo;
    bool categoria;
};

static std::vector<LinhaCredito> gLinhas;
static float gAlturaTotal = 0.f;

static void MontarLinhas()
{
    gLinhas.clear();
    gLinhas.push_back({ "PROJECT ASHFALL", true, false });
    gLinhas.push_back({ "", false, false });
    gLinhas.push_back({ "", false, false });

    gLinhas.push_back({ "DESENVOLVIMENTO", false, true });
    gLinhas.push_back({ "PEDRO GOMES", false, false });
    gLinhas.push_back({ "ALAN VICTOR", false, false });
    gLinhas.push_back({ "", false, false });

    gLinhas.push_back({ "PROGRAMACAO", false, true });
    gLinhas.push_back({ "PEDRO GOMES", false, false });
    gLinhas.push_back({ "ALAN VICTOR", false, false });
    gLinhas.push_back({ "", false, false });

    gLinhas.push_back({ "DESIGN E NARRATIVA", false, true });
    gLinhas.push_back({ "PEDRO GOMES", false, false });
    gLinhas.push_back({ "ALAN VICTOR", false, false });
    gLinhas.push_back({ "", false, false });

    gLinhas.push_back({ "ARTE E PIXEL ART", false, true });
    gLinhas.push_back({ "PEDRO GOMES", false, false });
    gLinhas.push_back({ "ALAN VICTOR", false, false });
    gLinhas.push_back({ "", false, false });
    gLinhas.push_back({ "", false, false });

    gLinhas.push_back({ "AGRADECIMENTOS ESPECIAIS", false, true });
    gLinhas.push_back({ "UNIVERSIDADE DO ESTADO DO RIO GRANDE DO NORTE", false, false });
    gLinhas.push_back({ "UERN", false, false });
    gLinhas.push_back({ "", false, false });
    gLinhas.push_back({ "", false, false });

    gLinhas.push_back({ "OBRIGADO POR JOGAR", true, false });
    gLinhas.push_back({ "", false, false });
    gLinhas.push_back({ "PROJECT ASHFALL", false, false });
}

void InicializarCreditos(sf::VideoMode desktop)
{
    InicializarFonte();

    if (!gFundoTextura) {
        gFundoTextura = new sf::Texture();
        if (gFundoTextura->loadFromFile(FUNDO_PATH)) {
            gFundoTextura->setSmooth(true);
            gFundoSprite = new sf::Sprite(*gFundoTextura);
        }
        else {
            delete gFundoTextura;
            gFundoTextura = nullptr;
        }
    }

    MontarLinhas();

    gScrollY = 0.f;
    gPausado = false;
    gClockCreditos.restart();

    float espacamento = ALTURA_REF * 0.07f;
    gAlturaTotal = (float)gLinhas.size() * espacamento;
}

bool AtualizarEDesenharCreditos(sf::RenderWindow& window)
{
    AplicarView(window);

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) { window.close(); return false; }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) return false;
            if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Enter)
                gPausado = !gPausado;
        }
    }

    float dt = gClockCreditos.restart().asSeconds();
    if (dt > 0.1f) dt = 0.1f;

    if (!gPausado)
        gScrollY += gVelocidadeScroll * dt;

    float espacamento = ALTURA_REF * 0.07f;
    if (gScrollY > gAlturaTotal + ALTURA_REF * 0.3f)
        return false;

    window.clear(sf::Color(8, 8, 10));

    if (gFundoSprite && gFundoTextura) {
        sf::Vector2u texSize = gFundoTextura->getSize();
        float scale = std::max(LARGURA_REF / texSize.x, ALTURA_REF / texSize.y);
        gFundoSprite->setScale(scale, scale);
        gFundoSprite->setPosition(
            (LARGURA_REF - texSize.x * scale) / 2.f,
            (ALTURA_REF - texSize.y * scale) / 2.f);
        gFundoSprite->setColor(sf::Color(255, 255, 255, 100));
        window.draw(*gFundoSprite);
    }

    sf::RectangleShape overlay(sf::Vector2f(LARGURA_REF, ALTURA_REF));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));
    window.draw(overlay);

    float startY = ALTURA_REF - gScrollY;

    for (size_t i = 0; i < gLinhas.size(); i++) {
        float posY = startY + (float)i * espacamento;

        if (posY < -espacamento || posY > ALTURA_REF + espacamento)
            continue;

        if (gLinhas[i].texto.empty()) continue;

        unsigned int tam;
        sf::Color cor;

        if (gLinhas[i].titulo) {
            tam = (unsigned int)(LARGURA_REF * 0.045f);
            cor = CorTitulo();
        }
        else if (gLinhas[i].categoria) {
            tam = (unsigned int)(LARGURA_REF * 0.030f);
            cor = sf::Color(210, 185, 130, 255);
        }
        else {
            tam = (unsigned int)(LARGURA_REF * 0.024f);
            cor = sf::Color(180, 180, 180, 230);
        }

        sf::Text txt = CriarTexto(gLinhas[i].texto, tam, cor);
        sf::FloatRect tr = txt.getLocalBounds();
        txt.setOrigin(tr.width / 2.f, 0.f);
        txt.setPosition(LARGURA_REF / 2.f, posY);
        window.draw(txt);
    }

    window.display();
    return true;
}

void LimparCreditos()
{
    if (gFundoSprite) { delete gFundoSprite; gFundoSprite = nullptr; }
    if (gFundoTextura) { delete gFundoTextura; gFundoTextura = nullptr; }
    gLinhas.clear();
}