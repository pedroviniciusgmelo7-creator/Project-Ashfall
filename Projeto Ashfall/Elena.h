#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <map>
class TileMap;
enum class DirecaoElena {
    Frente, Tras, Direita, Esquerda, NE, NO, SE, SO
};
enum class EstadoElena {
    ParadaSemArma,
    AndandoSemArma,
    ParadaComArmaNameo,
    AndandoComArmaNameo,
    ParadaAtirando,
    AndandoAtirando,
    ParadaRecarregando,
    AndandoRecarregando
};
class Elena {
public:
    Elena();
    ~Elena();
    bool Carregar();
    void Atualizar(float dt, int mapaLarguraPx, int mapaAlturaPx);
    void ProcessarEvento(const sf::Event& event);
    void Desenhar(sf::RenderTarget& window);
    void SetTileMap(TileMap* mapa) { gMapa = mapa; }
    sf::Vector2f GetPosicao()      const { return gPosicao; }
    void         SetPosicao(float x, float y) { gPosicao = { x, y }; }
    int          GetMunicao()      const { return gMunicaoAtual; }
    bool         GetSemMunicao()   const { return gSemMunicao; }
    bool         GetRecarregando() const { return gRecarregando; }
    bool         GetAtirando()     const { return gAtirando; }
    DirecaoElena GetDirecao()      const { return gDirecao; }
private:
    void CarregarAnimacao(const std::string& chave, const std::string& caminho, int totalFrames);
    void AtualizarAnimacao(float dt);
    std::string ChaveAtual() const;
    bool TemFrame(const std::string& chave) const;
    void ProcessarInput(float dt, int mapaLarguraPx, int mapaAlturaPx);
    struct Animacao {
        sf::Texture* textura = nullptr;
        int totalFrames = 8;
        int larguraFrame = 48;
        int alturaFrame = 64;
    };
    std::map<std::string, Animacao> gAnimacoes;
    sf::Sprite   gSprite;
    sf::Vector2f gPosicao = { 400.f, 300.f };
    float        gVelocidade = 80.f;
    float        gEscala = 3.f;
    DirecaoElena gDirecao = DirecaoElena::Tras;
    EstadoElena  gEstado = EstadoElena::ParadaSemArma;
    bool  gTemArma = false;
    bool  gAtirando = false;
    bool  gRecarregando = false;
    bool  gSemMunicao = false;
    int   gMunicaoAtual = 30;
    static const int       MUNICAO_MAX = 30;
    static constexpr float COOLDOWN_TIRO = 0.08f;
    float gTimerTiro = 0.f;
    bool  gVPressionadoAntes = false;
    bool  gRPressionadoAntes = false;
    int   gFrameAtual = 0;
    float gTimerFrame = 0.f;
    TileMap* gMapa = nullptr;
};