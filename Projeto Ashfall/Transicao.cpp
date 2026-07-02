#include "Transicao.h"
#include "Resolucao.h"
#include <SFML/Graphics.hpp>

static const std::string SHADER_PATH = "C:/ProjectAshfall/Data/shaders/iris.frag";

static sf::Shader* gShaderIris = nullptr;
static bool gShaderOk = false;

enum class FaseTransicao { Inativa, Abrindo };

static FaseTransicao gFase = FaseTransicao::Inativa;
static float gTimer = 0.f;
static bool gJaTrocouEstado = false;

static const float DURACAO_ABRIR = 0.99f;

void InicializarTransicao()
{
    if (!gShaderIris && sf::Shader::isAvailable()) {
        gShaderIris = new sf::Shader();
        gShaderOk = gShaderIris->loadFromFile(SHADER_PATH, sf::Shader::Fragment);
        if (gShaderOk) {
            gShaderIris->setUniform("texture", sf::Shader::CurrentTexture);
        }
        else {
            delete gShaderIris;
            gShaderIris = nullptr;
        }
    }
}

void IniciarTransicaoIris()
{
    gFase = FaseTransicao::Abrindo;
    gTimer = 0.f;
    gJaTrocouEstado = false;
}

bool TransicaoEstaAtiva()
{
    return gFase != FaseTransicao::Inativa;
}

bool TransicaoEstaNoMeio()
{
    if (gFase == FaseTransicao::Abrindo && !gJaTrocouEstado) {
        gJaTrocouEstado = true;
        return true;
    }
    return false;
}

void AtualizarTransicao(float dt)
{
    if (gFase == FaseTransicao::Inativa) return;

    gTimer += dt;

    if (gFase == FaseTransicao::Abrindo && gTimer >= DURACAO_ABRIR) {
        gFase = FaseTransicao::Inativa;
        gTimer = 0.f;
    }
}

void DesenharTransicao(sf::RenderWindow& window)
{
    if (gFase == FaseTransicao::Inativa) return;

    float progresso = gTimer / DURACAO_ABRIR;
    if (progresso > 1.f) progresso = 1.f;
    if (progresso < 0.f) progresso = 0.f;

    sf::Vector2u tamanhoJanela = window.getSize();

    if (gShaderOk && gShaderIris) {
        sf::View viewAtual = window.getView();
        window.setView(window.getDefaultView());

        sf::RectangleShape full(sf::Vector2f((float)tamanhoJanela.x, (float)tamanhoJanela.y));
        full.setPosition(0.f, 0.f);
        gShaderIris->setUniform("progresso", progresso);
        gShaderIris->setUniform("resolucao", sf::Glsl::Vec2((float)tamanhoJanela.x, (float)tamanhoJanela.y));
        window.draw(full, gShaderIris);

        window.setView(viewAtual);
    }
    else {
        sf::View viewAtual = window.getView();
        window.setView(window.getDefaultView());

        sf::RectangleShape fallback(sf::Vector2f((float)tamanhoJanela.x, (float)tamanhoJanela.y));
        fallback.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(255 * (1.f - progresso))));
        window.draw(fallback);

        window.setView(viewAtual);
    }
}

void LimparTransicao()
{
    if (gShaderIris) {
        delete gShaderIris;
        gShaderIris = nullptr;
    }
    gFase = FaseTransicao::Inativa;
}