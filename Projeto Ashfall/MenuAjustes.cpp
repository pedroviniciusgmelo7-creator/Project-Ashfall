#include "MenuAjustes.h"
#include "Resolucao.h"
#include "Janela.h"
#include "EstiloUI.h"
#include "Audio.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

static const std::string CONFIG_PATH = "C:/ProjectAshfall/Data/config.cfg";
static const std::string FUNDO_PATH = "C:/ProjectAshfall/Data/imagens/menu_principal/fundotela.png";
static const std::string LIVRO_PATH = "C:/ProjectAshfall/Data/imagens/menu_principal/livro.png";

static sf::Texture* gFundoTextura = nullptr;
static sf::Sprite* gFundoSprite = nullptr;
static sf::Texture* gLivroTextura = nullptr;
static sf::Sprite* gLivroSprite = nullptr;

static float gLivroW = 0.f;
static float gLivroH = 0.f;
static float gLivroX = 0.f;
static float gLivroY = 0.f;

static int gAbaAtual = 0;
static const int TOTAL_ABAS = 2;
static const char* NOMES_ABAS[] = { "VIDEO", "AUDIO" };

int gResolucaoIdx = 1;
int gModoTelaIdx = 0;
int gVolMusica = 80;
int gVolEfeitos = 60;
int gVolInterface = 80;

static int           gOpcaoSel = 0;
static sf::VideoMode gDesktop;

static const std::vector<std::string> RESOLUCOES = { "1280 x 720", "1920 x 1080", "Nativa" };
static const std::vector<std::string> MODOS_TELA = { "Janela", "Janela Cheia", "Tela Cheia Real" };

float POS_LIVRO_ESCALA = 1.0f;

float POS_ABA_X = 682.f;
float POS_ABA_Y = 28.f;
unsigned int TAM_ABA = 50;

float POS_DICA_ABA_X = 960.f;
float POS_DICA_ABA_Y = 130.f;
unsigned int TAM_DICA_ABA = 16;

float POS_OPCOES_ESQ_X = 0.f;
float POS_OPCOES_DIR_X = 0.f;
float POS_OPCOES_TOP_Y = 0.f;
unsigned int TAM_OPCAO = 42;

float POS_DICA_RODAPE_X = 676.f;
float POS_DICA_RODAPE_Y = 690.f;
unsigned int TAM_DICA_RODAPE = 24;

struct Opcao { std::string label; std::string valor; };

static std::vector<Opcao> GetOpcoes()
{
    if (gAbaAtual == 0) {
        return {
            { "RESOLUCAO",    RESOLUCOES[gResolucaoIdx] },
            { "MODO DE TELA", MODOS_TELA[gModoTelaIdx]  },
            { "VOLTAR",       ""                         }
        };
    }
    else {
        return {
            { "VOL. MUSICA",    std::to_string(gVolMusica) + "%" },
            { "VOL. EFEITOS",   std::to_string(gVolEfeitos) + "%" },
            { "VOL. INTERFACE", std::to_string(gVolInterface) + "%" },
            { "VOLTAR",         ""                                   }
        };
    }
}

static void SairDosAjustes(sf::RenderWindow& window)
{
    (void)window;
    SalvarConfig();
}

static void AvancarValor(int opcao)
{
    if (gAbaAtual == 0) {
        if (opcao == 0) gResolucaoIdx = (gResolucaoIdx + 1) % (int)RESOLUCOES.size();
        if (opcao == 1) gModoTelaIdx = (gModoTelaIdx + 1) % (int)MODOS_TELA.size();
    }
    else if (gAbaAtual == 1) {
        if (opcao == 0) { gVolMusica = std::min(100, gVolMusica + 10); AtualizarVolumeMusica(gVolMusica); }
        if (opcao == 1)   gVolEfeitos = std::min(100, gVolEfeitos + 10);
        if (opcao == 2) { gVolInterface = std::min(100, gVolInterface + 10); AtualizarVolumeInterface(gVolInterface); }
    }
}

static void VoltarValor(int opcao)
{
    if (gAbaAtual == 0) {
        if (opcao == 0) gResolucaoIdx = (gResolucaoIdx - 1 + (int)RESOLUCOES.size()) % (int)RESOLUCOES.size();
        if (opcao == 1) gModoTelaIdx = (gModoTelaIdx - 1 + (int)MODOS_TELA.size()) % (int)MODOS_TELA.size();
    }
    else if (gAbaAtual == 1) {
        if (opcao == 0) { gVolMusica = std::max(0, gVolMusica - 10); AtualizarVolumeMusica(gVolMusica); }
        if (opcao == 1)   gVolEfeitos = std::max(0, gVolEfeitos - 10);
        if (opcao == 2) { gVolInterface = std::max(0, gVolInterface - 10); AtualizarVolumeInterface(gVolInterface); }
    }
}

void SalvarConfig()
{
    std::ofstream f(CONFIG_PATH);
    if (!f.is_open()) return;
    f << "resolucao=" << gResolucaoIdx << "\n";
    f << "modo_tela=" << gModoTelaIdx << "\n";
    f << "volume_musica=" << gVolMusica << "\n";
    f << "volume_efeitos=" << gVolEfeitos << "\n";
    f << "volume_interface=" << gVolInterface << "\n";
}

void CarregarConfig(sf::VideoMode desktop)
{
    gDesktop = desktop;
    std::ifstream f(CONFIG_PATH);
    if (!f.is_open()) {
        LarguraRef() = (float)desktop.width;
        AlturaRef() = (float)desktop.height;
        return;
    }
    std::string linha;
    while (std::getline(f, linha)) {
        auto sep = linha.find('=');
        if (sep == std::string::npos) continue;
        std::string chave = linha.substr(0, sep);
        std::string valor = linha.substr(sep + 1);
        if (chave == "resolucao")             gResolucaoIdx = std::stoi(valor);
        else if (chave == "modo_tela")        gModoTelaIdx = std::stoi(valor);
        else if (chave == "volume_musica")    gVolMusica = std::stoi(valor);
        else if (chave == "volume_efeitos")   gVolEfeitos = std::stoi(valor);
        else if (chave == "volume_interface") gVolInterface = std::stoi(valor);
    }
    if (gResolucaoIdx == 0) { LarguraRef() = 1280.f; AlturaRef() = 720.f; }
    else if (gResolucaoIdx == 1) { LarguraRef() = 1920.f; AlturaRef() = 1080.f; }
    else { LarguraRef() = (float)desktop.width; AlturaRef() = (float)desktop.height; }
}

void InicializarAjustes(sf::VideoMode desktop)
{
    gDesktop = desktop;
    gOpcaoSel = 0;
    gAbaAtual = 0;

    InicializarFonte();

    if (!gFundoTextura) {
        gFundoTextura = new sf::Texture();
        if (gFundoTextura->loadFromFile(FUNDO_PATH)) {
            gFundoTextura->setSmooth(true);
            gFundoSprite = new sf::Sprite(*gFundoTextura);
        }
        else { delete gFundoTextura; gFundoTextura = nullptr; }
    }

    if (!gLivroTextura) {
        gLivroTextura = new sf::Texture();
        if (gLivroTextura->loadFromFile(LIVRO_PATH)) {
            gLivroTextura->setSmooth(false);
            gLivroSprite = new sf::Sprite(*gLivroTextura);
            sf::Vector2u ts = gLivroTextura->getSize();
            float refW = LARGURA_REF * POS_LIVRO_ESCALA;
            float refH = ALTURA_REF * POS_LIVRO_ESCALA;
            float sx = refW / ts.x;
            float sy = refH / ts.y;
            float scale = std::min(sx, sy);
            gLivroW = ts.x * scale;
            gLivroH = ts.y * scale;
            gLivroX = (LARGURA_REF - gLivroW) / 2.f;
            gLivroY = (ALTURA_REF - gLivroH) / 2.f;
        }
        else { delete gLivroTextura; gLivroTextura = nullptr; }
    }

    POS_OPCOES_ESQ_X = 364.f;
    POS_OPCOES_DIR_X = 720.f;
    POS_OPCOES_TOP_Y = 130.f;
}

bool AtualizarEDesenharAjustes(sf::RenderWindow& window)
{
    AplicarView(window);

    auto opcoes = GetOpcoes();
    int  totalOpcoes = (int)opcoes.size();
    float paginaH = (gLivroH > 0.f) ? gLivroH * 0.62f : ALTURA_REF * 0.62f;
    float startY = (POS_OPCOES_TOP_Y > 0.f) ? POS_OPCOES_TOP_Y + 10.f : ALTURA_REF * 0.20f + 10.f;
    float spacing = paginaH / (float)totalOpcoes;

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) { window.close(); return false; }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) { SairDosAjustes(window); return false; }

            if (event.key.code == sf::Keyboard::Q) { gAbaAtual = (gAbaAtual - 1 + TOTAL_ABAS) % TOTAL_ABAS; gOpcaoSel = 0; TocarSomNavegar(); opcoes = GetOpcoes(); totalOpcoes = (int)opcoes.size(); spacing = paginaH / (float)totalOpcoes; }
            if (event.key.code == sf::Keyboard::E) { gAbaAtual = (gAbaAtual + 1) % TOTAL_ABAS; gOpcaoSel = 0; TocarSomNavegar(); opcoes = GetOpcoes(); totalOpcoes = (int)opcoes.size(); spacing = paginaH / (float)totalOpcoes; }

            if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) { gOpcaoSel = (gOpcaoSel - 1 + totalOpcoes) % totalOpcoes; TocarSomNavegar(); }
            if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) { gOpcaoSel = (gOpcaoSel + 1) % totalOpcoes; TocarSomNavegar(); }

            if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) { VoltarValor(gOpcaoSel);  TocarSomNavegar(); }
            if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) { AvancarValor(gOpcaoSel); TocarSomNavegar(); }

            if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
                opcoes = GetOpcoes();
                if (opcoes[gOpcaoSel].label == "VOLTAR") { TocarSomSelecionar(); SairDosAjustes(window); return false; }
                else TocarSomSelecionar();
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mp = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

            // Clique nas setas < > da aba
            std::string textoAbaTemp = "< " + std::string(NOMES_ABAS[gAbaAtual]) + " >";
            sf::Text txAbaTemp = CriarTexto(textoAbaTemp, TAM_ABA, sf::Color::White);
            sf::FloatRect abBounds = txAbaTemp.getLocalBounds();
            float abaW = abBounds.width;
            float abaLeft = POS_ABA_X - abaW / 2.f;
            float setaW = TAM_ABA * 1.2f;

            sf::FloatRect areaSetaEsq(abaLeft, POS_ABA_Y, setaW, (float)TAM_ABA + 8.f);
            sf::FloatRect areaSetaDir(abaLeft + abaW - setaW, POS_ABA_Y, setaW, (float)TAM_ABA + 8.f);

            if (areaSetaEsq.contains(mp)) {
                gAbaAtual = (gAbaAtual - 1 + TOTAL_ABAS) % TOTAL_ABAS;
                gOpcaoSel = 0;
                TocarSomNavegar();
                opcoes = GetOpcoes(); totalOpcoes = (int)opcoes.size(); spacing = paginaH / (float)totalOpcoes;
            }
            else if (areaSetaDir.contains(mp)) {
                gAbaAtual = (gAbaAtual + 1) % TOTAL_ABAS;
                gOpcaoSel = 0;
                TocarSomNavegar();
                opcoes = GetOpcoes(); totalOpcoes = (int)opcoes.size(); spacing = paginaH / (float)totalOpcoes;
            }
            else {
                opcoes = GetOpcoes();
                totalOpcoes = (int)opcoes.size();
                spacing = paginaH / (float)totalOpcoes;
                for (int i = 0; i < totalOpcoes; i++) {
                    float posY = startY + i * spacing;
                    if (mp.y >= posY && mp.y <= posY + (float)TAM_OPCAO + 8.f) {
                        gOpcaoSel = i;
                        if (opcoes[i].label == "VOLTAR") { TocarSomSelecionar(); SairDosAjustes(window); return false; }
                    }
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        }

        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mp = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            opcoes = GetOpcoes();
            totalOpcoes = (int)opcoes.size();
            spacing = paginaH / (float)totalOpcoes;
            for (int i = 0; i < totalOpcoes; i++) {
                float posY = startY + i * spacing;
                sf::FloatRect area(POS_OPCOES_ESQ_X, posY, LARGURA_REF - POS_OPCOES_ESQ_X, (float)TAM_OPCAO + 8.f);
                if (area.contains(mp) && gOpcaoSel != i) {
                    gOpcaoSel = i;
                    TocarSomNavegar();
                }
            }
        }
    }

    window.clear(sf::Color(10, 10, 15));

    if (gFundoSprite && gFundoTextura) {
        sf::Vector2u texSize = gFundoTextura->getSize();
        float scale = std::max(LARGURA_REF / texSize.x, ALTURA_REF / texSize.y);
        gFundoSprite->setScale(scale, scale);
        gFundoSprite->setPosition(
            (LARGURA_REF - texSize.x * scale) / 2.f,
            (ALTURA_REF - texSize.y * scale) / 2.f);
        gFundoSprite->setColor(sf::Color(255, 255, 255, 160));
        window.draw(*gFundoSprite);
    }

    sf::RectangleShape overlay(sf::Vector2f(LARGURA_REF, ALTURA_REF));
    overlay.setFillColor(sf::Color(0, 0, 0, 210));
    window.draw(overlay);

    if (gLivroSprite && gLivroTextura) {
        gLivroSprite->setScale(gLivroW / gLivroTextura->getSize().x,
            gLivroH / gLivroTextura->getSize().y);
        gLivroSprite->setPosition(gLivroX, gLivroY);
        window.draw(*gLivroSprite);
    }

    float paginaW = gLivroW * 0.38f;

    std::string textoAba = "< " + std::string(NOMES_ABAS[gAbaAtual]) + " >";
    sf::Text txAba = CriarTexto(textoAba, TAM_ABA, sf::Color(255, 255, 255, 255));
    sf::FloatRect ab = txAba.getLocalBounds();
    txAba.setOrigin(ab.width / 2.f, 0.f);
    txAba.setPosition(POS_ABA_X, POS_ABA_Y);
    window.draw(txAba);

    sf::Text dicaAba = CriarTexto("Q/E: MUDAR ABA", TAM_DICA_ABA, sf::Color(255, 255, 255, 100));
    sf::FloatRect da = dicaAba.getLocalBounds();
    dicaAba.setOrigin(da.width / 2.f, 0.f);
    dicaAba.setPosition(POS_DICA_ABA_X, POS_DICA_ABA_Y);
    window.draw(dicaAba);

    opcoes = GetOpcoes();
    totalOpcoes = (int)opcoes.size();
    spacing = paginaH / (float)totalOpcoes;

    for (int i = 0; i < totalOpcoes; i++) {
        float posY = startY + i * spacing;
        bool  sel = (i == gOpcaoSel);

        sf::Color corLabel = sel ? sf::Color(255, 255, 255, 255) : sf::Color(40, 20, 5, 255);
        sf::Color corValor = sel ? sf::Color(255, 255, 255, 255) : sf::Color(40, 20, 5, 255);

        sf::Text txLabel = CriarTexto(opcoes[i].label, TAM_OPCAO, corLabel);
        txLabel.setPosition(POS_OPCOES_ESQ_X + 8.f, posY);
        window.draw(txLabel);

        if (sel) {
            sf::Text seta = CriarTexto(">", TAM_OPCAO, sf::Color(255, 255, 255, 255));
            seta.setPosition(POS_OPCOES_ESQ_X - 2.f, posY);
            window.draw(seta);
        }

        if (!opcoes[i].valor.empty()) {
            std::string valorStr = sel ? "< " + opcoes[i].valor + " >" : opcoes[i].valor;
            sf::Text txValor = CriarTexto(valorStr, TAM_OPCAO, corValor);
            txValor.setPosition(POS_OPCOES_DIR_X + 8.f, posY);
            window.draw(txValor);

            if (opcoes[i].label == "VOL. MUSICA" || opcoes[i].label == "VOL. EFEITOS" || opcoes[i].label == "VOL. INTERFACE") {
                int vol = 0;
                if (opcoes[i].label == "VOL. MUSICA")    vol = gVolMusica;
                if (opcoes[i].label == "VOL. EFEITOS")   vol = gVolEfeitos;
                if (opcoes[i].label == "VOL. INTERFACE") vol = gVolInterface;

                float barW2 = paginaW * 0.50f;
                float barY = posY + TAM_OPCAO + 4.f;

                sf::RectangleShape barFundo(sf::Vector2f(barW2, 4.f));
                barFundo.setPosition(POS_OPCOES_DIR_X + 8.f, barY);
                barFundo.setFillColor(sf::Color(60, 35, 8, 150));
                window.draw(barFundo);

                sf::RectangleShape barPreench(sf::Vector2f(barW2 * vol / 100.f, 7.f));
                barPreench.setPosition(POS_OPCOES_DIR_X + 8.f, barY);
                barPreench.setFillColor(sf::Color(40, 20, 5, 255));
                window.draw(barPreench);
            }
        }
    }

    sf::Text dica = CriarTexto("SETAS: NAVEGAR   ENTER: CONFIRMAR   ESC: VOLTAR",
        TAM_DICA_RODAPE, sf::Color(210, 185, 130, 200));
    sf::FloatRect dr = dica.getLocalBounds();
    dica.setOrigin(dr.width / 2.f, 0.f);
    dica.setPosition(POS_DICA_RODAPE_X, POS_DICA_RODAPE_Y);
    window.draw(dica);

    window.display();
    return true;
}

void LimparAjustes()
{
    if (gFundoSprite) { delete gFundoSprite;  gFundoSprite = nullptr; }
    if (gFundoTextura) { delete gFundoTextura; gFundoTextura = nullptr; }
    if (gLivroSprite) { delete gLivroSprite;  gLivroSprite = nullptr; }
    if (gLivroTextura) { delete gLivroTextura; gLivroTextura = nullptr; }
    gLivroW = 0.f;
    gLivroH = 0.f;
}