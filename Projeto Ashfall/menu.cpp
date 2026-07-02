#include "Menu.h"
#include "Resolucao.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>

static AVFormatContext* gFmtCtx = nullptr;
static AVCodecContext* gCodecCtx = nullptr;
static SwsContext* gSwsCtx = nullptr;
static AVFrame* gFrame = nullptr;
static AVFrame* gFrameRGB = nullptr;
static uint8_t* gBuffer = nullptr;
static int              gVideoStream = -1;

static int gVideoW = 1280;
static int gVideoH = 720;

static sf::Texture* gVideoTextura = nullptr;
static sf::Sprite* gVideoSprite = nullptr;
static sf::Texture* gTitleTextura = nullptr;
static sf::Sprite* gTitleSprite = nullptr;
static bool         gTitleCarregado = false;
static sf::Vector2u gTitleSize;
static sf::Texture* gPressKeyTextura = nullptr;
static sf::Sprite* gPressKeySprite = nullptr;
static bool         gPressKeyCarregado = false;
static sf::Vector2u gPressKeySize;

static float       gVideoAcumulador = 0.f;
static const float gVideoFrameTempo = 1.f / 30.f;

static sf::Clock* gAnimClock = nullptr;
static std::mt19937 gRng{ std::random_device{}() };

enum class EstadoTitulo { Aceso, Apagando, Apagado, Acendendo };

static EstadoTitulo gEstadoTitulo = EstadoTitulo::Aceso;
static float        gTempoEstado = 0.f;
static float        gDuracaoEstadoAtual = 1.5f;

bool ObterResolucaoVideo(const std::string& caminho, int& w, int& h)
{
    AVFormatContext* fmt = nullptr;
    if (avformat_open_input(&fmt, caminho.c_str(), nullptr, nullptr) < 0)
        return false;
    avformat_find_stream_info(fmt, nullptr);
    for (unsigned i = 0; i < fmt->nb_streams; i++) {
        if (fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            w = fmt->streams[i]->codecpar->width;
            h = fmt->streams[i]->codecpar->height;
            avformat_close_input(&fmt);
            return true;
        }
    }
    avformat_close_input(&fmt);
    return false;
}

static bool AbrirVideo(const std::string& caminho)
{
    if (avformat_open_input(&gFmtCtx, caminho.c_str(), nullptr, nullptr) < 0)
        return false;

    avformat_find_stream_info(gFmtCtx, nullptr);

    for (unsigned i = 0; i < gFmtCtx->nb_streams; i++) {
        if (gFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            gVideoStream = (int)i;
            break;
        }
    }
    if (gVideoStream < 0) return false;

    AVCodecParameters* par = gFmtCtx->streams[gVideoStream]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(par->codec_id);
    gCodecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(gCodecCtx, par);
    gCodecCtx->thread_count = 4;
    gCodecCtx->thread_type = FF_THREAD_FRAME;
    avcodec_open2(gCodecCtx, codec, nullptr);

    gFrame = av_frame_alloc();
    gFrameRGB = av_frame_alloc();
    gVideoW = gCodecCtx->width;
    gVideoH = gCodecCtx->height;

    int bufSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, gVideoW, gVideoH, 1);
    gBuffer = (uint8_t*)av_malloc(bufSize);
    av_image_fill_arrays(gFrameRGB->data, gFrameRGB->linesize,
        gBuffer, AV_PIX_FMT_RGBA, gVideoW, gVideoH, 1);

    gSwsCtx = sws_getContext(gVideoW, gVideoH, gCodecCtx->pix_fmt,
        gVideoW, gVideoH, AV_PIX_FMT_RGBA,
        SWS_LANCZOS | SWS_ACCURATE_RND | SWS_FULL_CHR_H_INT,
        nullptr, nullptr, nullptr);

    gVideoTextura = new sf::Texture();
    gVideoTextura->create(gVideoW, gVideoH);
    gVideoTextura->setSmooth(true);

    gVideoSprite = new sf::Sprite();
    gVideoSprite->setTexture(*gVideoTextura);

    return true;
}

static void RebobinarVideo()
{
    av_seek_frame(gFmtCtx, gVideoStream, 0, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(gCodecCtx);
}

static bool AvancarFrame()
{
    AVPacket packet;
    int tentativas = 0;

    while (tentativas < 2) {
        while (av_read_frame(gFmtCtx, &packet) >= 0) {
            if (packet.stream_index == gVideoStream) {
                if (avcodec_send_packet(gCodecCtx, &packet) == 0) {
                    if (avcodec_receive_frame(gCodecCtx, gFrame) == 0) {
                        sws_scale(gSwsCtx,
                            gFrame->data, gFrame->linesize, 0, gVideoH,
                            gFrameRGB->data, gFrameRGB->linesize);
                        gVideoTextura->update(gFrameRGB->data[0]);
                        av_packet_unref(&packet);
                        return true;
                    }
                }
            }
            av_packet_unref(&packet);
        }
        RebobinarVideo();
        tentativas++;
    }
    return false;
}

static float CalcularAlphaTitulo(float deltaTime)
{
    const float ALPHA_ACESO = 1.00f;
    const float ALPHA_APAGADO = 0.00f;
    const float DUR_ACESO_MIN = 0.8f;
    const float DUR_ACESO_MAX = 1.5f;
    const float DUR_APAGANDO = 0.6f;
    const float DUR_APAGADO = 0.4f;
    const float DUR_ACENDENDO = 0.5f;

    gTempoEstado += deltaTime;

    if (gTempoEstado >= gDuracaoEstadoAtual) {
        gTempoEstado = 0.f;
        switch (gEstadoTitulo) {
        case EstadoTitulo::Aceso:
            gEstadoTitulo = EstadoTitulo::Apagando;
            gDuracaoEstadoAtual = DUR_APAGANDO;
            break;
        case EstadoTitulo::Apagando:
            gEstadoTitulo = EstadoTitulo::Apagado;
            gDuracaoEstadoAtual = DUR_APAGADO;
            break;
        case EstadoTitulo::Apagado:
            gEstadoTitulo = EstadoTitulo::Acendendo;
            gDuracaoEstadoAtual = DUR_ACENDENDO;
            break;
        case EstadoTitulo::Acendendo: {
            gEstadoTitulo = EstadoTitulo::Aceso;
            std::uniform_real_distribution<float> dist(DUR_ACESO_MIN, DUR_ACESO_MAX);
            gDuracaoEstadoAtual = dist(gRng);
            break;
        }
        }
    }

    float progresso = gTempoEstado / gDuracaoEstadoAtual;

    switch (gEstadoTitulo) {
    case EstadoTitulo::Aceso:    return ALPHA_ACESO;
    case EstadoTitulo::Apagando: {
        float easeOut = 1.f - (1.f - progresso) * (1.f - progresso);
        return ALPHA_ACESO + (ALPHA_APAGADO - ALPHA_ACESO) * easeOut;
    }
    case EstadoTitulo::Apagado:  return ALPHA_APAGADO;
    case EstadoTitulo::Acendendo: {
        float easeIn = progresso * progresso;
        return ALPHA_APAGADO + (ALPHA_ACESO - ALPHA_APAGADO) * easeIn;
    }
    }
    return ALPHA_ACESO;
}

void InicializarMenu()
{
    gAnimClock = new sf::Clock();

    AbrirVideo("C:/ProjectAshfall/data/videos/ashfall.mp4");

    gTitleTextura = new sf::Texture();
    if (gTitleTextura->loadFromFile("C:/ProjectAshfall/data/imagens/menu_inicial/projec.png")) {
        gTitleTextura->setSmooth(false);
        gTitleSize = gTitleTextura->getSize();
        gTitleSprite = new sf::Sprite();
        gTitleSprite->setTexture(*gTitleTextura);
        gTitleSprite->setOrigin(gTitleSize.x / 2.f, gTitleSize.y / 2.f);

        float escala = (LARGURA_REF * 0.55f) / gTitleSize.x;
        gTitleSprite->setScale(escala, escala);
        gTitleCarregado = true;
    }
    else {
        delete gTitleTextura;
        gTitleTextura = nullptr;
    }

    gPressKeyTextura = new sf::Texture();
    if (gPressKeyTextura->loadFromFile("C:/ProjectAshfall/data/imagens/menu_inicial/tecla.png")) {
        gPressKeyTextura->setSmooth(true);
        gPressKeySize = gPressKeyTextura->getSize();
        gPressKeySprite = new sf::Sprite();
        gPressKeySprite->setTexture(*gPressKeyTextura);
        gPressKeySprite->setOrigin(gPressKeySize.x / 2.f, gPressKeySize.y / 2.f);

        float escala = (LARGURA_REF * 0.48f) / gPressKeySize.x;
        gPressKeySprite->setScale(escala, escala);
        gPressKeyCarregado = true;
    }
    else {
        delete gPressKeyTextura;
        gPressKeyTextura = nullptr;
    }

    gEstadoTitulo = EstadoTitulo::Aceso;
    gTempoEstado = 0.f;
    gDuracaoEstadoAtual = 1.2f;
}

void AtualizarEDesenharMenu(sf::RenderWindow& window)
{
    AplicarView(window);

    static sf::Clock deltaClock;
    float deltaTime = deltaClock.restart().asSeconds();
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    if (gVideoSprite && gVideoTextura) {
        float escalaX = LARGURA_REF / gVideoW;
        float escalaY = ALTURA_REF / gVideoH;
        float escala = std::max(escalaX, escalaY);
        gVideoSprite->setScale(escala, escala);
        float lw = gVideoW * escala;
        float lh = gVideoH * escala;
        gVideoSprite->setPosition(
            (LARGURA_REF - lw) / 2.f,
            (ALTURA_REF - lh) / 2.f);
    }

    if (gFmtCtx && gVideoTextura) {
        gVideoAcumulador += deltaTime;
        while (gVideoAcumulador >= gVideoFrameTempo) {
            gVideoAcumulador -= gVideoFrameTempo;
            AvancarFrame();
        }
    }

    float t = gAnimClock ? gAnimClock->getElapsedTime().asSeconds() : 0.f;
    float titleAlpha = CalcularAlphaTitulo(deltaTime);

    float titleCenterX = LARGURA_REF / 2.f;
    float titleCenterY = ALTURA_REF * 0.43f;

    float blinkAlpha = 0.65f + 0.35f * std::sin(t * 3.0f);
    sf::Color pressColor(255, 255, 255, static_cast<sf::Uint8>(255 * blinkAlpha));

    float pressKeyY = titleCenterY;
    if (gTitleCarregado) {
        sf::Vector2f escalaT = gTitleSprite->getScale();
        float        alturaEscalada = gTitleSize.y * escalaT.y;
        pressKeyY = titleCenterY + (alturaEscalada / 2.f);
    }

    if (gPressKeySprite) {
        gPressKeySprite->setColor(pressColor);
        gPressKeySprite->setPosition(LARGURA_REF / 2.f, pressKeyY);
    }

    window.clear(sf::Color::Black);

    if (gFmtCtx && gVideoTextura && gVideoSprite)
        window.draw(*gVideoSprite);

    sf::RectangleShape filtroEscuro(sf::Vector2f(LARGURA_REF, ALTURA_REF));
    filtroEscuro.setPosition(0.f, 0.f);
    filtroEscuro.setFillColor(sf::Color(0, 0, 0, 130));
    window.draw(filtroEscuro);

    if (gTitleCarregado && gTitleSprite && titleAlpha > 0.01f) {
        sf::Vector2f escalaBase = gTitleSprite->getScale();

        sf::Sprite titleRed = *gTitleSprite;
        titleRed.setColor(sf::Color(255, 50, 50, static_cast<sf::Uint8>(100 * titleAlpha)));
        titleRed.setPosition(titleCenterX + 2.f, titleCenterY);
        titleRed.setScale(escalaBase.x, escalaBase.y);
        window.draw(titleRed, sf::BlendAdd);

        sf::Sprite titleBlue = *gTitleSprite;
        titleBlue.setColor(sf::Color(50, 50, 255, static_cast<sf::Uint8>(100 * titleAlpha)));
        titleBlue.setPosition(titleCenterX - 2.f, titleCenterY);
        titleBlue.setScale(escalaBase.x, escalaBase.y);
        window.draw(titleBlue, sf::BlendAdd);

        sf::Uint8 alphaByte = static_cast<sf::Uint8>(255 * titleAlpha);
        gTitleSprite->setColor(sf::Color(255, 255, 255, alphaByte));
        gTitleSprite->setPosition(titleCenterX, titleCenterY);
        gTitleSprite->setScale(escalaBase.x, escalaBase.y);
        window.draw(*gTitleSprite);
    }

    if (gPressKeyCarregado && gPressKeySprite)
        window.draw(*gPressKeySprite);

    window.display();
}

void LimparRecursosMenu()
{
    if (gBuffer) { av_free(gBuffer);                  gBuffer = nullptr; }
    if (gFrame) { av_frame_free(&gFrame);            gFrame = nullptr; }
    if (gFrameRGB) { av_frame_free(&gFrameRGB);         gFrameRGB = nullptr; }
    if (gSwsCtx) { sws_freeContext(gSwsCtx);          gSwsCtx = nullptr; }
    if (gCodecCtx) { avcodec_free_context(&gCodecCtx);  gCodecCtx = nullptr; }
    if (gFmtCtx) { avformat_close_input(&gFmtCtx);    gFmtCtx = nullptr; }

    if (gVideoSprite) { delete gVideoSprite;    gVideoSprite = nullptr; }
    if (gVideoTextura) { delete gVideoTextura;   gVideoTextura = nullptr; }
    if (gTitleSprite) { delete gTitleSprite;    gTitleSprite = nullptr; }
    if (gTitleTextura) { delete gTitleTextura;   gTitleTextura = nullptr; }
    if (gPressKeySprite) { delete gPressKeySprite; gPressKeySprite = nullptr; }
    if (gPressKeyTextura) { delete gPressKeyTextura; gPressKeyTextura = nullptr; }
    if (gAnimClock) { delete gAnimClock;      gAnimClock = nullptr; }

    gVideoStream = -1;
    gTitleCarregado = false;
    gPressKeyCarregado = false;
}