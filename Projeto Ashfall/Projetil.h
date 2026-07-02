#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
class TileMap;
struct Projetil {
    sf::Vector2f posicao;
    sf::Vector2f velocidade;
    float        rotacao = 0.f;
    bool         ativo = true;
    int          frame = 0;
    float        timerAnim = 0.f;
};
class GerenciadorProjeteis {
public:
    GerenciadorProjeteis();
    ~GerenciadorProjeteis();
    bool Carregar();
    void SetTileMap(TileMap* mapa) { gMapa = mapa; }
    void Disparar(sf::Vector2f origem, sf::Vector2f direcao, float rotacao);
    void Atualizar(float dt, int mapaLarguraPx, int mapaAlturaPx);
    void Desenhar(sf::RenderTarget& window);
    void Limpar();
    std::vector<sf::FloatRect> GetHitboxes() const;
private:
    static const int       TOTAL_FRAMES = 4;
    static const int       FRAME_W = 14;
    static const int       FRAME_H = 5;
    static const int       VELOCIDADE = 600;
    static constexpr float TEMPO_FRAME = 0.05f;
    sf::Texture           gTextura;
    std::vector<Projetil> gProjeteis;
    bool                  gCarregado = false;
    TileMap* gMapa = nullptr;
};