#include "Zumbi.h"
#include "TileMap.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

static const std::string BASE = "C:/ProjectAshfall/Data/Avatar/z1/";

Zumbi::Zumbi() {}

Zumbi::~Zumbi()
{
    for (auto& par : gAnimacoes)
        if (par.second.textura) delete par.second.textura;
}

void Zumbi::CarregarAnimacao(const std::string& chave, const std::string& caminho, int totalFrames)
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
    anim.textura->setSmooth(false);
    gAnimacoes[chave] = anim;
}

bool Zumbi::Carregar()
{
    CarregarAnimacao("andando_Direita", BASE + "z1_andando_direita.png", 8);
    CarregarAnimacao("andando_Esquerda", BASE + "z1_andando_esquerda.png", 8);
    CarregarAnimacao("andando_Frente", BASE + "z1_andando_frente.png", 8);
    CarregarAnimacao("andando_Tras", BASE + "z1_andando_tras.png", 8);
    CarregarAnimacao("parado_Direita", BASE + "z1_parado_direita.png", 6);
    CarregarAnimacao("parado_Esquerda", BASE + "z1_parado_esquerda.png", 6);
    CarregarAnimacao("parado_Frente", BASE + "z1_parado_frente.png", 6);
    CarregarAnimacao("parado_Tras", BASE + "z1_parado_tras.png", 6);
    CarregarAnimacao("atacando1_Direita", BASE + "z1_atacando_direita.png", 15);
    CarregarAnimacao("atacando1_Esquerda", BASE + "z1_atacando_esquerda.png", 15);
    CarregarAnimacao("atacando1_Frente", BASE + "z1_atacando_frente.png", 15);
    CarregarAnimacao("atacando1_Tras", BASE + "z1_atacando_tras.png", 15);
    CarregarAnimacao("soco_Direita", BASE + "z1_atacando_soco_direita.png", 8);
    CarregarAnimacao("soco_Esquerda", BASE + "z1_atacando_soco_esquerda.png", 8);
    CarregarAnimacao("soco_Frente", BASE + "z1_atacando_soco_frente.png", 8);
    CarregarAnimacao("soco_Tras", BASE + "z1_atacando_soco_tras.png", 8);
    CarregarAnimacao("morrendo_Direita", BASE + "z1_morrendo_direita.png", 8);
    CarregarAnimacao("morrendo_Esquerda", BASE + "z1_morrendo_esquerda.png", 8);
    return !gAnimacoes.empty();
}

std::string Zumbi::DirParaString(DirecaoZumbi dir) const
{
    switch (dir) {
    case DirecaoZumbi::Direita:  return "Direita";
    case DirecaoZumbi::Esquerda: return "Esquerda";
    case DirecaoZumbi::Frente:   return "Frente";
    case DirecaoZumbi::Tras:     return "Tras";
    }
    return "Direita";
}

std::string Zumbi::ChaveAtual() const
{
    switch (gEstado) {
    case EstadoZumbi::Parado:       return "parado_" + DirParaString(gDirecao);
    case EstadoZumbi::Andando:      return "andando_" + DirParaString(gDirecao);
    case EstadoZumbi::Atacando1:    return "atacando1_" + DirParaString(gDirecaoTravada);
    case EstadoZumbi::AtacandoSoco: return "soco_" + DirParaString(gDirecaoTravada);
    case EstadoZumbi::Morrendo:
        return (gDirecao == DirecaoZumbi::Esquerda)
            ? "morrendo_Esquerda" : "morrendo_Direita";
    default: return "parado_" + DirParaString(gDirecao);
    }
}

sf::FloatRect Zumbi::GetHitbox() const
{
    float w = 16.f * gEscala;
    float h = 24.f * gEscala;
    return { gPosicao.x - w / 2.f, gPosicao.y - h, w * 0.6f, h * 0.8f };
}

sf::Vector2f Zumbi::GetPosicao() const { return gPosicao; }
void Zumbi::Empurrar(sf::Vector2f dir) { gPosicao += dir; }
bool Zumbi::PodeAtacar() const { return gPodeAtacar; }
void Zumbi::SetPodeAtacar(bool v) { gPodeAtacar = v; }
void Zumbi::SetSlotAlvo(sf::Vector2f slot) { gSlotAlvo = slot; gTemSlot = true; }
void Zumbi::SetDesvioPerpendicular(float d) { gDesvioPerpendicular = d; }

void Zumbi::ReceberDano()
{
    if (gEstado == EstadoZumbi::Morrendo || gEstado == EstadoZumbi::Morto) return;
    gVida--;
    if (gVida <= 0) {
        gEstado = EstadoZumbi::Morrendo;
        gFrameAtual = 0;
        gTimerFrame = 0.f;
        gAnimFim = false;
        gPodeAtacar = false;
        gTemSlot = false;
        gDesvioPerpendicular = 0.f;
    }
}

void Zumbi::AtualizarIA(float dt, sf::Vector2f posElena)
{
    if (gEstado == EstadoZumbi::Morrendo || gEstado == EstadoZumbi::Morto) return;
    if (gAtacando) return;

    float dx = posElena.x - gPosicao.x;
    float dy = posElena.y - gPosicao.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    float rangeAtaque = 45.f;

    if (dist > gCampoVisao) {
        gEstado = EstadoZumbi::Andando;
        gTimerAtaque = 0.f;
        gTimerPatrulha += dt;
        if (gTimerPatrulha >= gTempoPatrulha) {
            gTimerPatrulha = 0.f;
            gTempoPatrulha = 2.f + (float)(std::rand() % 3);
            int dir = std::rand() % 4;
            switch (dir) {
            case 0: gDirecaoPatrulha = { 1.f,  0.f }; gDirecao = DirecaoZumbi::Direita;  break;
            case 1: gDirecaoPatrulha = { -1.f,  0.f }; gDirecao = DirecaoZumbi::Esquerda; break;
            case 2: gDirecaoPatrulha = { 0.f,  1.f }; gDirecao = DirecaoZumbi::Tras;     break;
            case 3: gDirecaoPatrulha = { 0.f, -1.f }; gDirecao = DirecaoZumbi::Frente;   break;
            }
        }
        gPosicao += gDirecaoPatrulha * gVelocidade * 0.4f * dt;
        return;
    }

    if (dist < rangeAtaque&& gPodeAtacar) {
        if (std::abs(dx) >= std::abs(dy))
            gDirecao = dx > 0 ? DirecaoZumbi::Direita : DirecaoZumbi::Esquerda;
        else
            gDirecao = dy > 0 ? DirecaoZumbi::Tras : DirecaoZumbi::Frente;

        float tolerancia = 12.f;
        bool alinhado = false;

        if (gDirecao == DirecaoZumbi::Direita || gDirecao == DirecaoZumbi::Esquerda)
            alinhado = std::abs(dy) < tolerancia;
        else
            alinhado = std::abs(dx) < tolerancia;

        if (!alinhado) {
            gEstado = EstadoZumbi::Andando;
            float vx = 0.f, vy = 0.f;
            if (gDirecao == DirecaoZumbi::Direita || gDirecao == DirecaoZumbi::Esquerda)
                vy = (dy > 0 ? 1.f : -1.f) * gVelocidade * dt;
            else
                vx = (dx > 0 ? 1.f : -1.f) * gVelocidade * dt;

            if (std::abs(vx) >= std::abs(vy))
                gDirecao = vx > 0 ? DirecaoZumbi::Direita : DirecaoZumbi::Esquerda;
            else
                gDirecao = vy > 0 ? DirecaoZumbi::Tras : DirecaoZumbi::Frente;

            sf::Vector2f novaPosicao = { gPosicao.x + vx, gPosicao.y + vy };
            if (gMapa) {
                float w = 16.f * gEscala;
                float h = 24.f * gEscala;
                sf::FloatRect hitbox(novaPosicao.x - w / 2.f, novaPosicao.y - h, w * 0.6f, h * 0.8f);
                if (!gMapa->Colide(hitbox))
                    gPosicao = novaPosicao;
            }
            else {
                gPosicao = novaPosicao;
            }
            return;
        }

        gTimerAtaque += dt;
        if (gTimerAtaque >= gCooldownAtaque) {
            gTimerAtaque = 0.f;
            gAtacando = true;
            gFrameAtual = 0;
            gTimerFrame = 0.f;
            gDirecaoTravada = gDirecao;
            gEstado = (std::rand() % 2 == 0) ? EstadoZumbi::Atacando1 : EstadoZumbi::AtacandoSoco;
        }
        else {
            if (gEstado != EstadoZumbi::Atacando1 && gEstado != EstadoZumbi::AtacandoSoco)
                gEstado = EstadoZumbi::Parado;
        }
        return;
    }

    gEstado = EstadoZumbi::Andando;

    float len = dist > 0.f ? dist : 1.f;
    float nx = dx / len;
    float ny = dy / len;

    float px = -ny;
    float py = nx;

    float vx = (nx + px * gDesvioPerpendicular) * gVelocidade * dt;
    float vy = (ny + py * gDesvioPerpendicular) * gVelocidade * dt;

    float vmag = std::sqrt(vx * vx + vy * vy);
    if (vmag > 0.f) {
        vx = (vx / vmag) * gVelocidade * dt;
        vy = (vy / vmag) * gVelocidade * dt;
    }

    if (std::abs(vx) >= std::abs(vy))
        gDirecao = vx > 0 ? DirecaoZumbi::Direita : DirecaoZumbi::Esquerda;
    else
        gDirecao = vy > 0 ? DirecaoZumbi::Tras : DirecaoZumbi::Frente;

    sf::Vector2f novaPosicao = { gPosicao.x + vx, gPosicao.y + vy };
    if (gMapa) {
        float w = 16.f * gEscala;
        float h = 24.f * gEscala;
        sf::FloatRect hitbox(novaPosicao.x - w / 2.f, novaPosicao.y - h, w * 0.6f, h * 0.8f);
        if (!gMapa->Colide(hitbox))
            gPosicao = novaPosicao;
    }
    else {
        gPosicao = novaPosicao;
    }
}

void Zumbi::AtualizarAnimacao(float dt)
{
    std::string chave = ChaveAtual();
    auto it = gAnimacoes.find(chave);
    if (it == gAnimacoes.end() || !it->second.textura) return;

    int totalFrames = it->second.totalFrames;
    gFrameAtual = gFrameAtual % totalFrames;

    gTimerFrame += dt;
    if (gTimerFrame >= gTempoFrame) {
        gTimerFrame -= gTempoFrame;
        gFrameAtual = (gFrameAtual + 1) % totalFrames;
        if (gFrameAtual == 0) {
            gAnimFim = true;
            if (gEstado == EstadoZumbi::Morrendo)
                gEstado = EstadoZumbi::Morto;
            else if (gAtacando)
                gAtacando = false;
        }
    }
}

void Zumbi::Atualizar(float dt, sf::Vector2f posElena)
{
    if (gEstado == EstadoZumbi::Morto) return;
    AtualizarIA(dt, posElena);
    AtualizarAnimacao(dt);
}

void Zumbi::Desenhar(sf::RenderTarget& window)
{
    if (gEstado == EstadoZumbi::Morto) return;

    std::string chave = ChaveAtual();
    auto it = gAnimacoes.find(chave);
    if (it == gAnimacoes.end() || !it->second.textura) return;

    const Animacao& anim = it->second;
    int             frameSeguro = (gFrameAtual < anim.totalFrames) ? gFrameAtual : 0;

    gSprite.setTexture(*anim.textura);
    gSprite.setTextureRect(sf::IntRect(frameSeguro * anim.larguraFrame, 0, anim.larguraFrame, anim.alturaFrame));
    gSprite.setScale(gEscala, gEscala);

    float larguraAtual = anim.larguraFrame * gEscala;
    float alturaAtual = anim.alturaFrame * gEscala;

    gSprite.setPosition(
        std::floor(gPosicao.x - larguraAtual / 2.f),
        std::floor(gPosicao.y - alturaAtual)
    );
    window.draw(gSprite);
}

GerenciadorZumbis::GerenciadorZumbis() {}
GerenciadorZumbis::~GerenciadorZumbis() {}

bool GerenciadorZumbis::Carregar()
{
    Zumbi teste;
    gCarregado = teste.Carregar();
    return gCarregado;
}

void GerenciadorZumbis::SpawnarAleatorio(int quantidade, int mapaLarguraPx, int mapaAlturaPx)
{
    gZumbis.clear();
    gZumbis.reserve(quantidade);
    for (int i = 0; i < quantidade; i++) {
        Zumbi z;
        z.Carregar();
        z.SetTileMap(gMapa);
        float x = 100.f + (float)(std::rand() % (mapaLarguraPx - 200));
        float y = 100.f + (float)(std::rand() % (mapaAlturaPx - 200));
        z.SetPosicao({ x, y });
        gZumbis.push_back(std::move(z));
    }
}

void GerenciadorZumbis::Atualizar(float dt, sf::Vector2f posElena, std::vector<sf::FloatRect>& hitboxProjeteis)
{
    static const int  MAX_ATACANDO = 2;
    float raioAtaque = 45.f;
    float raioSeparacao = 48.f;
    float raioElena = 45.f;
    float raioBloqueia = 40.f;

    int atacandoAgora = 0;
    for (auto& z : gZumbis) {
        if (z.EstaMorto()) continue;
        sf::Vector2f p = z.GetPosicao();
        float ddx = p.x - posElena.x;
        float ddy = p.y - posElena.y;
        if (std::sqrt(ddx * ddx + ddy * ddy) < raioAtaque)
            atacandoAgora++;
    }

    for (int i = 0; i < (int)gZumbis.size(); i++) {
        if (gZumbis[i].EstaMorto()) continue;

        sf::Vector2f pi = gZumbis[i].GetPosicao();
        float dxi = posElena.x - pi.x;
        float dyi = posElena.y - pi.y;
        float disti = std::sqrt(dxi * dxi + dyi * dyi);

        if (disti < 1.f) continue;

        float nxi = dxi / disti;
        float nyi = dyi / disti;

        float desvioTotal = 0.f;
        int   bloqueadores = 0;

        for (int j = 0; j < (int)gZumbis.size(); j++) {
            if (i == j || gZumbis[j].EstaMorto()) continue;

            sf::Vector2f pj = gZumbis[j].GetPosicao();
            float dvx = pj.x - pi.x;
            float dvy = pj.y - pi.y;

            float proj = dvx * nxi + dvy * nyi;
            if (proj <= 0.f) continue;

            float perpx = dvx - proj * nxi;
            float perpy = dvy - proj * nyi;
            float perpDist = std::sqrt(perpx * perpx + perpy * perpy);

            if (perpDist < raioBloqueia && proj < disti) {
                float cross = nxi * dvy - nyi * dvx;
                float forca = (raioBloqueia - perpDist) / raioBloqueia;
                desvioTotal += (cross > 0 ? -1.f : 1.f) * forca * 0.8f;
                bloqueadores++;
            }
        }

        if (desvioTotal > 1.2f) desvioTotal = 1.2f;
        if (desvioTotal < -1.2f) desvioTotal = -1.2f;

        bool vagaDisponivel = atacandoAgora < MAX_ATACANDO;
        gZumbis[i].SetPodeAtacar(vagaDisponivel || disti < raioAtaque);
        gZumbis[i].SetDesvioPerpendicular(desvioTotal);
    }

    for (int i = 0; i < (int)gZumbis.size(); i++) {
        if (gZumbis[i].EstaMorto()) continue;
        gZumbis[i].Atualizar(dt, posElena);

        for (auto& hb : hitboxProjeteis) {
            if (gZumbis[i].GetHitbox().intersects(hb)) {
                gZumbis[i].ReceberDano();
                break;
            }
        }

        sf::Vector2f pi = gZumbis[i].GetPosicao();

        float dxE = pi.x - posElena.x;
        float dyE = pi.y - posElena.y;
        float distE = std::sqrt(dxE * dxE + dyE * dyE);
        if (distE < raioElena && distE > 0.f) {
            float emp = (raioElena - distE) / 2.f;
            gZumbis[i].Empurrar({ (dxE / distE) * emp, (dyE / distE) * emp });
        }

        for (int j = i + 1; j < (int)gZumbis.size(); j++) {
            if (gZumbis[j].EstaMorto()) continue;
            sf::Vector2f pj = gZumbis[j].GetPosicao();
            float ddx = pi.x - pj.x;
            float ddy = pi.y - pj.y;
            float dist = std::sqrt(ddx * ddx + ddy * ddy);
            if (dist < raioSeparacao && dist > 0.f) {
                float emp = (raioSeparacao - dist) * 0.15f;
                sf::Vector2f dir = { (ddx / dist) * emp, (ddy / dist) * emp };
                gZumbis[i].Empurrar(dir);
                gZumbis[j].Empurrar(-dir);
            }
        }
    }

    gZumbis.erase(
        std::remove_if(gZumbis.begin(), gZumbis.end(),
            [](const Zumbi& z) { return z.EstaMorto(); }),
        gZumbis.end());
}

void GerenciadorZumbis::Desenhar(sf::RenderTarget& window)
{
    for (auto& z : gZumbis)
        z.Desenhar(window);
}