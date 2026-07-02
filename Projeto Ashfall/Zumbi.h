#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <vector>
class TileMap;
enum class DirecaoZumbi {
    Direita, Esquerda, Frente, Tras
};
enum class EstadoZumbi {
    Parado,
    Andando,
    Atacando1,
    AtacandoSoco,
    Morrendo,
    Morto
};
class Zumbi {
public:
    Zumbi();
    ~Zumbi();
    Zumbi(Zumbi&&) = default;
    Zumbi& operator=(Zumbi&&) = default;
    Zumbi(const Zumbi&) = delete;
    Zumbi& operator=(const Zumbi&) = delete;
    bool Carregar();
    void Atualizar(float dt, sf::Vector2f posElena);
    void Desenhar(sf::RenderTarget& window);
    void SetPosicao(sf::Vector2f pos) { gPosicao = pos; }
    void SetTileMap(TileMap* mapa) { gMapa = mapa; }
    void SetPodeAtacar(bool v);
    void SetSlotAlvo(sf::Vector2f slot);
    void SetDesvioPerpendicular(float d);
    void ReceberDano();
    bool EstaMorto() const { return gEstado == EstadoZumbi::Morto; }
    bool PodeAtacar() const;
    sf::FloatRect GetHitbox() const;
    sf::Vector2f  GetPosicao() const;
    void Empurrar(sf::Vector2f dir);
private:
    void CarregarAnimacao(const std::string& chave, const std::string& caminho, int totalFrames);
    void AtualizarAnimacao(float dt);
    void AtualizarIA(float dt, sf::Vector2f posElena);
    std::string ChaveAtual() const;
    std::string DirParaString(DirecaoZumbi dir) const;
    struct Animacao {
        sf::Texture* textura = nullptr;
        int totalFrames = 8;
        int larguraFrame = 0;
        int alturaFrame = 0;
    };
    std::map<std::string, Animacao> gAnimacoes;
    sf::Sprite   gSprite;
    sf::Vector2f gPosicao = { 0.f, 0.f };
    sf::Vector2f gSlotAlvo = { 0.f, 0.f };
    float        gVelocidade = 70.f;
    float        gEscala = 3.f;
    float        gDesvioPerpendicular = 0.f;
    DirecaoZumbi gDirecao = DirecaoZumbi::Direita;
    DirecaoZumbi gDirecaoTravada = DirecaoZumbi::Direita;
    EstadoZumbi  gEstado = EstadoZumbi::Parado;
    int   gVida = 10;
    float gCampoVisao = 300.f;
    float gTimerAtaque = 0.f;
    float gCooldownAtaque = 1.5f;
    bool  gAtacando = false;
    bool  gPodeAtacar = true;
    bool  gTemSlot = false;
    int   gFrameAtual = 0;
    float gTimerFrame = 0.f;
    float gTempoFrame = 0.1f;
    bool  gAnimFim = false;
    float gTimerPatrulha = 0.f;
    float gTempoPatrulha = 2.f;
    float gTimerTrocaDirecao = 0.f;
    sf::Vector2f gDirecaoPatrulha = { 1.f, 0.f };
    TileMap* gMapa = nullptr;
};
class GerenciadorZumbis {
public:
    GerenciadorZumbis();
    ~GerenciadorZumbis();
    bool Carregar();
    void SpawnarAleatorio(int quantidade, int mapaLarguraPx, int mapaAlturaPx);
    void SetTileMap(TileMap* mapa) { gMapa = mapa; }
    void Atualizar(float dt, sf::Vector2f posElena, std::vector<sf::FloatRect>& projeteis);
    void Desenhar(sf::RenderTarget& window);
    int          GetQuantidade()                       const { return (int)gZumbis.size(); }
    sf::Vector2f GetPosicaoZumbi(int i)                const { return gZumbis[i].GetPosicao(); }
    void         DesenharZumbi(sf::RenderTarget& w, int i) { gZumbis[i].Desenhar(w); }
private:
    std::vector<Zumbi> gZumbis;
    TileMap* gMapa = nullptr;
    bool     gCarregado = false;
};