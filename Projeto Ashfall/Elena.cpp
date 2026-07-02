#include "Elena.h"
#include "TileMap.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

static const std::string B_SEMARMA_PARADA = "C:/ProjectAshfall/Data/Avatar/elena/paradasemarma/";
static const std::string B_SEMARMA_ANDANDO = "C:/ProjectAshfall/Data/Avatar/elena/andandosemarma/";
static const std::string B_COMARMA_NAMAO_PAR = "C:/ProjectAshfall/Data/Avatar/elena/paradacomarmanamao/";
static const std::string B_COMARMA_NAMAO_AND = "C:/ProjectAshfall/Data/Avatar/elena/andandocomarmanamao/";
static const std::string B_MIRANDO_ANDANDO = "C:/ProjectAshfall/Data/Avatar/elena/andandocomarma/";
static const std::string B_ATIRANDO_PARADA = "C:/ProjectAshfall/Data/Avatar/elena/paradaatirando/";
static const std::string B_ATIRANDO_ANDANDO = "C:/ProjectAshfall/Data/Avatar/elena/andandoatirando/";
static const std::string B_RECARREG_PARADA = "C:/ProjectAshfall/Data/Avatar/elena/paradarecarregando/";
static const std::string B_RECARREG_ANDANDO = "C:/ProjectAshfall/Data/Avatar/elena/andandorecarregando/";

Elena::Elena() {}

Elena::~Elena()
{
    for (auto& par : gAnimacoes)
        if (par.second.textura) delete par.second.textura;
}

void Elena::CarregarAnimacao(const std::string& chave, const std::string& caminho, int totalFrames)
{
    Animacao anim;
    anim.textura = new sf::Texture();
    anim.totalFrames = totalFrames;
    if (!anim.textura->loadFromFile(caminho)) {
        delete anim.textura;
        anim.textura = nullptr;
        return;
    }
    anim.larguraFrame = (int)anim.textura->getSize().x / totalFrames;
    anim.alturaFrame = (int)anim.textura->getSize().y;
    gAnimacoes[chave] = anim;
}

bool Elena::TemFrame(const std::string& chave) const
{
    auto it = gAnimacoes.find(chave);
    return (it != gAnimacoes.end() && it->second.textura != nullptr);
}

bool Elena::Carregar()
{
    CarregarAnimacao("paradasemarma_Tras", B_SEMARMA_PARADA + "paradapratras.png", 8);
    CarregarAnimacao("paradasemarma_Frente", B_SEMARMA_PARADA + "paradafrente.png", 8);
    CarregarAnimacao("paradasemarma_Direita", B_SEMARMA_PARADA + "paradadireita.png", 8);
    CarregarAnimacao("paradasemarma_Esquerda", B_SEMARMA_PARADA + "paradaesquerda.png", 8);
    CarregarAnimacao("paradasemarma_NE", B_SEMARMA_PARADA + "paradadireita.png", 8);
    CarregarAnimacao("paradasemarma_NO", B_SEMARMA_PARADA + "paradaesquerda.png", 8);
    CarregarAnimacao("paradasemarma_SE", B_SEMARMA_PARADA + "paradadireita.png", 8);
    CarregarAnimacao("paradasemarma_SO", B_SEMARMA_PARADA + "paradaesquerda.png", 8);

    CarregarAnimacao("andandosemarma_Tras", B_SEMARMA_ANDANDO + "andandopratras.png", 8);
    CarregarAnimacao("andandosemarma_Frente", B_SEMARMA_ANDANDO + "andandoprafrente.png", 8);
    CarregarAnimacao("andandosemarma_Direita", B_SEMARMA_ANDANDO + "andandodireita.png", 8);
    CarregarAnimacao("andandosemarma_Esquerda", B_SEMARMA_ANDANDO + "andandoesquerda.png", 8);
    CarregarAnimacao("andandosemarma_NE", B_SEMARMA_ANDANDO + "andandodireitahorizontalfrente.png", 8);
    CarregarAnimacao("andandosemarma_NO", B_SEMARMA_ANDANDO + "andandoesquerdahorizontalfrente.png", 8);

    CarregarAnimacao("paradacomarmanamao_Tras", B_COMARMA_NAMAO_PAR + "paradacomarmanamaopratras.png", 8);
    CarregarAnimacao("paradacomarmanamao_Frente", B_COMARMA_NAMAO_PAR + "paradacomarmanamaofrente.png", 8);
    CarregarAnimacao("paradacomarmanamao_Direita", B_COMARMA_NAMAO_PAR + "paradacomarmanamaodireita.png", 8);
    CarregarAnimacao("paradacomarmanamao_Esquerda", B_COMARMA_NAMAO_PAR + "paradacomarmanamaoesquerda.png", 8);
    CarregarAnimacao("paradacomarmanamao_NE", B_COMARMA_NAMAO_PAR + "paradacomarmanamaodiagonaldireitafrente.png", 8);
    CarregarAnimacao("paradacomarmanamao_NO", B_COMARMA_NAMAO_PAR + "paradacomarmanamaodiagonalesquerdafrente.png", 8);
    CarregarAnimacao("paradacomarmanamao_SE", B_COMARMA_NAMAO_PAR + "paradacomarmanamaodireita.png", 8);
    CarregarAnimacao("paradacomarmanamao_SO", B_COMARMA_NAMAO_PAR + "paradacomarmanamaoesquerda.png", 8);

    CarregarAnimacao("andandocomarmanamao_Tras", B_COMARMA_NAMAO_AND + "andandocomarmasemmirarpratras.png", 8);
    CarregarAnimacao("andandocomarmanamao_Frente", B_COMARMA_NAMAO_AND + "andandocomarmasemmirarfrente.png", 8);
    CarregarAnimacao("andandocomarmanamao_Direita", B_COMARMA_NAMAO_AND + "andandocomarmasemmirarpradireita.png", 8);
    CarregarAnimacao("andandocomarmanamao_Esquerda", B_COMARMA_NAMAO_AND + "andandocomarmasemmirarpraesquerda.png", 8);
    CarregarAnimacao("andandocomarmanamao_NE", B_COMARMA_NAMAO_AND + "andandocomarmasemmirardiagonaldireitafrente.png", 8);
    CarregarAnimacao("andandocomarmanamao_NO", B_COMARMA_NAMAO_AND + "andandocomarmasemmirardiagonalesquerdafrente.png", 8);

    CarregarAnimacao("paradamirando_Tras", B_MIRANDO_ANDANDO + "paradamirando.png", 3);
    CarregarAnimacao("paradamirando_Frente", B_MIRANDO_ANDANDO + "paradamirando.png", 3);
    CarregarAnimacao("paradamirando_Direita", B_MIRANDO_ANDANDO + "paradamirando.png", 3);
    CarregarAnimacao("paradamirando_Esquerda", B_MIRANDO_ANDANDO + "paradamirando.png", 3);
    CarregarAnimacao("paradamirando_NE", B_MIRANDO_ANDANDO + "paradamirando.png", 3);
    CarregarAnimacao("paradamirando_NO", B_MIRANDO_ANDANDO + "paradamirando.png", 3);
    CarregarAnimacao("paradamirando_SE", B_MIRANDO_ANDANDO + "paradamirando.png", 3);
    CarregarAnimacao("paradamirando_SO", B_MIRANDO_ANDANDO + "paradamirando.png", 3);

    CarregarAnimacao("andandomirando_Tras", B_MIRANDO_ANDANDO + "andandocomarmapratras.png", 8);
    CarregarAnimacao("andandomirando_Frente", B_MIRANDO_ANDANDO + "andandocomarmafrente.png", 8);
    CarregarAnimacao("andandomirando_Direita", B_MIRANDO_ANDANDO + "andandocomarmadireita.png", 8);
    CarregarAnimacao("andandomirando_Esquerda", B_MIRANDO_ANDANDO + "andandocomarmaesquerda.png", 8);
    CarregarAnimacao("andandomirando_NE", B_MIRANDO_ANDANDO + "andandocomarmahorizontaldireitafrente.png", 8);
    CarregarAnimacao("andandomirando_NO", B_MIRANDO_ANDANDO + "andandocomarmahorizontalesquerdafrente.png", 8);
    CarregarAnimacao("andandomirando_SE", B_MIRANDO_ANDANDO + "andandocomarmahorizontaldireitatras.png", 8);
    CarregarAnimacao("andandomirando_SO", B_MIRANDO_ANDANDO + "andandocomarmahorizontalesquerdatras.png", 8);

    CarregarAnimacao("paradaatirando_Tras", B_ATIRANDO_PARADA + "paradaatirandopratras.png", 8);
    CarregarAnimacao("paradaatirando_Frente", B_ATIRANDO_PARADA + "paradaatirandofrente.png", 8);
    CarregarAnimacao("paradaatirando_Direita", B_ATIRANDO_PARADA + "paradaatirandodireita.png", 8);
    CarregarAnimacao("paradaatirando_Esquerda", B_ATIRANDO_PARADA + "paradaatirandoesquerda.png", 8);
    CarregarAnimacao("paradaatirando_NE", B_ATIRANDO_PARADA + "paradaatirandohorizontaldireitafrente.png", 8);
    CarregarAnimacao("paradaatirando_NO", B_ATIRANDO_PARADA + "paradaatirandohorizontalesquerdafrente.png", 8);
    CarregarAnimacao("paradaatirando_SE", B_ATIRANDO_PARADA + "paradaatirandohorizontaldiretatras.png", 8);
    CarregarAnimacao("paradaatirando_SO", B_ATIRANDO_PARADA + "paradaatirandohorizontalesquerdatras.png", 8);

    CarregarAnimacao("andandoatirando_Tras", B_ATIRANDO_ANDANDO + "andandoatirandopratras.png", 8);
    CarregarAnimacao("andandoatirando_Frente", B_ATIRANDO_ANDANDO + "andandoatirandofrente.png", 8);
    CarregarAnimacao("andandoatirando_Direita", B_ATIRANDO_ANDANDO + "andandoatirandodireita.png", 8);
    CarregarAnimacao("andandoatirando_Esquerda", B_ATIRANDO_ANDANDO + "andandoatirandoesquerda.png", 8);
    CarregarAnimacao("andandoatirando_NE", B_ATIRANDO_ANDANDO + "andandoatirandohorizontaldireitafrente.png", 8);
    CarregarAnimacao("andandoatirando_NO", B_ATIRANDO_ANDANDO + "andandoatirandohorizontalesquerdafrente.png", 8);
    CarregarAnimacao("andandoatirando_SE", B_ATIRANDO_ANDANDO + "andandoatirandohorizontaldireitatras.png", 8);
    CarregarAnimacao("andandoatirando_SO", B_ATIRANDO_ANDANDO + "andandoatirandohorizontalesquerdatras.png", 8);

    CarregarAnimacao("paradarecarregando_Tras", B_RECARREG_PARADA + "recarregandoparadatras.png", 8);
    CarregarAnimacao("paradarecarregando_Frente", B_RECARREG_PARADA + "recarregandofrente.png", 8);
    CarregarAnimacao("paradarecarregando_Direita", B_RECARREG_PARADA + "recarregandodireita.png", 8);
    CarregarAnimacao("paradarecarregando_Esquerda", B_RECARREG_PARADA + "recarregandoesquerda.png", 8);
    CarregarAnimacao("paradarecarregando_NE", B_RECARREG_PARADA + "recarregandohorizontaldireitafrente.png", 8);
    CarregarAnimacao("paradarecarregando_NO", B_RECARREG_PARADA + "recarregandohorizontalesquerdafrente.png", 8);

    CarregarAnimacao("andandorecarregando_Tras", B_RECARREG_ANDANDO + "carregandoarmapratras.png", 8);
    CarregarAnimacao("andandorecarregando_Frente", B_RECARREG_ANDANDO + "carregandoarmafrente.png", 8);
    CarregarAnimacao("andandorecarregando_Direita", B_RECARREG_ANDANDO + "carregandoarmadireita.png", 8);
    CarregarAnimacao("andandorecarregando_Esquerda", B_RECARREG_ANDANDO + "carregandoarmaesquerd.png", 8);
    CarregarAnimacao("andandorecarregando_NE", B_RECARREG_ANDANDO + "carregandoarmahorizontaldireitafrente.png", 8);
    CarregarAnimacao("andandorecarregando_NO", B_RECARREG_ANDANDO + "carregandoarmahorizontalesquerdafrente.png", 8);
    CarregarAnimacao("andandorecarregando_SE", B_RECARREG_ANDANDO + "carregandoarmahorizontaldireitatras.png", 8);
    CarregarAnimacao("andandorecarregando_SO", B_RECARREG_ANDANDO + "carregandoarmahorizontalesquerdatras.png", 8);

    return !gAnimacoes.empty();
}

std::string Elena::ChaveAtual() const
{
    std::string estado;
    switch (gEstado) {
    case EstadoElena::ParadaSemArma:       estado = "paradasemarma";       break;
    case EstadoElena::AndandoSemArma:      estado = "andandosemarma";      break;
    case EstadoElena::ParadaComArmaNameo:  estado = "paradacomarmanamao";  break;
    case EstadoElena::AndandoComArmaNameo: estado = "andandocomarmanamao"; break;
    case EstadoElena::ParadaAtirando:      estado = "paradaatirando";      break;
    case EstadoElena::AndandoAtirando:     estado = "andandoatirando";     break;
    case EstadoElena::ParadaRecarregando:  estado = "paradarecarregando";  break;
    case EstadoElena::AndandoRecarregando: estado = "andandorecarregando"; break;
    default:                               estado = "paradasemarma";       break;
    }
    std::string dir;
    switch (gDirecao) {
    case DirecaoElena::Frente:   dir = "Frente";   break;
    case DirecaoElena::Tras:     dir = "Tras";     break;
    case DirecaoElena::Direita:  dir = "Direita";  break;
    case DirecaoElena::Esquerda: dir = "Esquerda"; break;
    case DirecaoElena::NE:       dir = "NE";       break;
    case DirecaoElena::NO:       dir = "NO";       break;
    case DirecaoElena::SE:       dir = "SE";       break;
    case DirecaoElena::SO:       dir = "SO";       break;
    default:                     dir = "Tras";     break;
    }
    return estado + "_" + dir;
}


void Elena::ProcessarEvento(const sf::Event& event)
{
    (void)event;
}

void Elena::AtualizarAnimacao(float dt)
{
    if (gTimerTiro > 0.f) {
        gTimerTiro -= dt;
        if (gTimerTiro <= 0.f) {
            gTimerTiro = 0.f;
            gAtirando = false;
        }
    }

    bool recarregando = (gEstado == EstadoElena::ParadaRecarregando ||
        gEstado == EstadoElena::AndandoRecarregando);
    bool paradaOciosa = (gEstado == EstadoElena::ParadaSemArma ||
        gEstado == EstadoElena::ParadaComArmaNameo);

    float tempo;
    if (recarregando)      tempo = 0.12f;
    else if (paradaOciosa) tempo = 0.2f;
    else                   tempo = 0.08f;

    gTimerFrame += dt;
    if (gTimerFrame >= tempo) {
        gTimerFrame = 0.f;
        auto it = gAnimacoes.find(ChaveAtual());
        if (it != gAnimacoes.end() && it->second.textura) {
            gFrameAtual++;
            if (gRecarregando && gFrameAtual >= it->second.totalFrames) {
                gMunicaoAtual = MUNICAO_MAX;
                gRecarregando = false;
                gAtirando = false;
                gFrameAtual = 0;
            }
            else if (gFrameAtual >= it->second.totalFrames) {
                gFrameAtual = 0;
            }
        }
    }
}

void Elena::ProcessarInput(float dt, int mapaLarguraPx, int mapaAlturaPx)
{
    bool vPress = sf::Keyboard::isKeyPressed(sf::Keyboard::V);
    if (vPress && !gVPressionadoAntes) {
        gTemArma = !gTemArma;
        gAtirando = false;
        gRecarregando = false;
        gMunicaoAtual = MUNICAO_MAX;
        gFrameAtual = 0;
        gTimerTiro = 0.f;
    }
    gVPressionadoAntes = vPress;

    bool rPress = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
    if (rPress && !gRPressionadoAntes && gTemArma && !gRecarregando && gMunicaoAtual < MUNICAO_MAX) {
        gRecarregando = true;
        gAtirando = false;
        gFrameAtual = 0;
        gTimerFrame = 0.f;
    }
    gRPressionadoAntes = rPress;

    if (gTemArma && !gRecarregando) {
        bool enterPress = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
        if (enterPress && gMunicaoAtual > 0) {
            if (gTimerTiro <= 0.f) {
                gAtirando = true;
                gMunicaoAtual--;
                gTimerTiro = COOLDOWN_TIRO;
                if (gMunicaoAtual <= 0) {
                    gAtirando = false;
                    gRecarregando = true;
                    gFrameAtual = 0;
                    gTimerFrame = 0.f;
                }
            }
        }
        else if (!enterPress) {
            gAtirando = false;
        }
    }


    bool cima = sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool baixo = sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool esq = sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool dir = sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    sf::Vector2f vel(0.f, 0.f);
    DirecaoElena novaDirecao = gDirecao;

    if (cima && dir) { vel = { gVelocidade, -gVelocidade }; novaDirecao = DirecaoElena::NE; }
    else if (cima && esq) { vel = { -gVelocidade, -gVelocidade }; novaDirecao = DirecaoElena::NO; }
    else if (baixo && dir) { vel = { gVelocidade,  gVelocidade }; novaDirecao = DirecaoElena::SE; }
    else if (baixo && esq) { vel = { -gVelocidade,  gVelocidade }; novaDirecao = DirecaoElena::SO; }
    else if (cima) { vel = { 0.f,           -gVelocidade }; novaDirecao = DirecaoElena::Frente; }
    else if (baixo) { vel = { 0.f,            gVelocidade }; novaDirecao = DirecaoElena::Tras; }
    else if (dir) { vel = { gVelocidade,  0.f }; novaDirecao = DirecaoElena::Direita; }
    else if (esq) { vel = { -gVelocidade,  0.f }; novaDirecao = DirecaoElena::Esquerda; }

    bool movendo = (vel.x != 0.f || vel.y != 0.f);

    if (movendo) {
        std::string estadoStr;
        if (gRecarregando)  estadoStr = "andandorecarregando";
        else if (gAtirando) estadoStr = "andandoatirando";
        else if (gTemArma)  estadoStr = "andandocomarmanamao";
        else                estadoStr = "andandosemarma";

        std::string dirStr;
        switch (novaDirecao) {
        case DirecaoElena::Frente:   dirStr = "Frente";   break;
        case DirecaoElena::Tras:     dirStr = "Tras";     break;
        case DirecaoElena::Direita:  dirStr = "Direita";  break;
        case DirecaoElena::Esquerda: dirStr = "Esquerda"; break;
        case DirecaoElena::NE:       dirStr = "NE";       break;
        case DirecaoElena::NO:       dirStr = "NO";       break;
        case DirecaoElena::SE:       dirStr = "SE";       break;
        case DirecaoElena::SO:       dirStr = "SO";       break;
        default:                     dirStr = "Tras";     break;
        }

        if (TemFrame(estadoStr + "_" + dirStr)) {
            gDirecao = novaDirecao;
        }
        else {
            vel = { 0.f, 0.f };
            movendo = false;
        }
    }


    if (gRecarregando)
        gEstado = movendo ? EstadoElena::AndandoRecarregando : EstadoElena::ParadaRecarregando;
    else if (gAtirando)
        gEstado = movendo ? EstadoElena::AndandoAtirando : EstadoElena::ParadaAtirando;
    else if (gTemArma)
        gEstado = movendo ? EstadoElena::AndandoComArmaNameo : EstadoElena::ParadaComArmaNameo;
    else
        gEstado = movendo ? EstadoElena::AndandoSemArma : EstadoElena::ParadaSemArma;

    if (!movendo) {
        if (gDirecao == DirecaoElena::NE || gDirecao == DirecaoElena::SE)
            gDirecao = DirecaoElena::Direita;
        else if (gDirecao == DirecaoElena::NO || gDirecao == DirecaoElena::SO)
            gDirecao = DirecaoElena::Esquerda;
        if (!gRecarregando && !gAtirando)
            gFrameAtual = 0;
    }

    gPosicao += vel * dt;

    // Colisao com o mapa
    if (gMapa) {
        float spriteW = 48.f * gEscala;
        float spriteH = 64.f * gEscala;
        sf::FloatRect hitbox(gPosicao.x + 20.f, gPosicao.y + 80.f, spriteW - 40.f, spriteH - 90.f);
        if (gMapa->Colide(hitbox)) {
            gPosicao -= vel * dt;
        }
    }

    float spriteW = 48.f * gEscala;
    float spriteH = 64.f * gEscala;
    if (gPosicao.x < 0.f)                     gPosicao.x = 0.f;
    if (gPosicao.y < 0.f)                     gPosicao.y = 0.f;
    if (gPosicao.x + spriteW > mapaLarguraPx) gPosicao.x = (float)mapaLarguraPx - spriteW;
    if (gPosicao.y + spriteH > mapaAlturaPx)  gPosicao.y = (float)mapaAlturaPx - spriteH;
}

void Elena::Atualizar(float dt, int mapaLarguraPx, int mapaAlturaPx)
{
    ProcessarInput(dt, mapaLarguraPx, mapaAlturaPx);
    AtualizarAnimacao(dt);
}

void Elena::Desenhar(sf::RenderTarget& window)
{
    auto it = gAnimacoes.find(ChaveAtual());
    if (it == gAnimacoes.end() || !it->second.textura) return;

    const Animacao& anim = it->second;
    gSprite.setTexture(*anim.textura);
    gSprite.setTextureRect(sf::IntRect(gFrameAtual * anim.larguraFrame, 0, anim.larguraFrame, anim.alturaFrame));
    gSprite.setScale(gEscala, gEscala);
    gSprite.setPosition(std::floor(gPosicao.x), std::floor(gPosicao.y));
    window.draw(gSprite);
}