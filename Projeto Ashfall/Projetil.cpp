#include "Projetil.h"
#include "TileMap.h"
#include <algorithm>
static const std::string CAMINHO = "C:/ProjectAshfall/Data/Avatar/bullet/framestiro.png";
GerenciadorProjeteis::GerenciadorProjeteis() {}
GerenciadorProjeteis::~GerenciadorProjeteis()
{
    Limpar();
}
bool GerenciadorProjeteis::Carregar()
{
    gCarregado = gTextura.loadFromFile(CAMINHO);
    gTextura.setSmooth(false);
    return gCarregado;
}
void GerenciadorProjeteis::Disparar(sf::Vector2f origem, sf::Vector2f direcao, float rotacao)
{
    Projetil p;
    p.posicao = origem;
    p.velocidade = direcao * (float)VELOCIDADE;
    p.rotacao = rotacao;
    p.ativo = true;
    p.frame = 0;
    p.timerAnim = 0.f;
    gProjeteis.push_back(p);
}
void GerenciadorProjeteis::Atualizar(float dt, int mapaLarguraPx, int mapaAlturaPx)
{
    for (auto& p : gProjeteis) {
        if (!p.ativo) continue;
        p.posicao += p.velocidade * dt;
        if (p.posicao.x < 0 || p.posicao.x > mapaLarguraPx ||
            p.posicao.y < 0 || p.posicao.y > mapaAlturaPx) {
            p.ativo = false;
            continue;
        }
        p.timerAnim += dt;
        if (p.timerAnim >= TEMPO_FRAME) {
            p.timerAnim = 0.f;
            p.frame = (p.frame + 1) % TOTAL_FRAMES;
        }
        if (gMapa) {
            sf::FloatRect hitbox(p.posicao.x - 4.f, p.posicao.y - 4.f, 8.f, 8.f);
            if (gMapa->Colide(hitbox))
                p.ativo = false;
        }
    }
    gProjeteis.erase(
        std::remove_if(gProjeteis.begin(), gProjeteis.end(),
            [](const Projetil& p) { return !p.ativo; }),
        gProjeteis.end());
}
void GerenciadorProjeteis::Desenhar(sf::RenderTarget& window)
{
    if (!gCarregado) return;
    for (auto& p : gProjeteis) {
        if (!p.ativo) continue;
        sf::Sprite sprite(gTextura);
        sprite.setTextureRect(sf::IntRect(p.frame * FRAME_W, 0, FRAME_W, FRAME_H));
        sprite.setOrigin(7.f, 2.5f);
        sprite.setPosition(p.posicao);
        sprite.setRotation(p.rotacao);
        sprite.setScale(3.f, 3.f);
        window.draw(sprite);
    }
}
std::vector<sf::FloatRect> GerenciadorProjeteis::GetHitboxes() const
{
    std::vector<sf::FloatRect> hbs;
    for (auto& p : gProjeteis)
        if (p.ativo)
            hbs.push_back(sf::FloatRect(p.posicao.x - 8.f, p.posicao.y - 8.f, 16.f, 16.f));
    return hbs;
}
void GerenciadorProjeteis::Limpar()
{
    gProjeteis.clear();
}