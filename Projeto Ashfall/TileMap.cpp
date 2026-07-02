#include "TileMap.h"
#include <windows.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cctype>

const float TileMap::ESCALA = 3.f;
const std::vector<Objeto> TileMap::gVazio;

TileMap::TileMap() {}

TileMap::~TileMap()
{
    for (auto& ts : gTilesets)
        if (ts.textura) delete ts.textura;
}

std::string TileMap::LerArquivo(const std::string& caminho) const
{
    std::ifstream f(caminho);
    if (!f.is_open()) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::string TileMap::AtributoXML(const std::string& xml, const std::string& attr) const
{
    std::string busca = attr + "=\"";
    size_t pos = xml.find(busca);
    if (pos == std::string::npos) return "";
    pos += busca.size();
    size_t fim = xml.find('"', pos);
    if (fim == std::string::npos) return "";
    return xml.substr(pos, fim - pos);
}

std::string TileMap::TagXML(const std::string& xml, const std::string& tag) const
{
    std::string abertura = "<" + tag;
    std::string fechamento = "</" + tag + ">";
    size_t inicio = xml.find(abertura);
    if (inicio == std::string::npos) return "";
    size_t fim = xml.find(fechamento, inicio);
    if (fim == std::string::npos) {
        size_t autoFim = xml.find("/>", inicio);
        if (autoFim == std::string::npos) return "";
        return xml.substr(inicio, autoFim + 2 - inicio);
    }
    return xml.substr(inicio, fim + fechamento.size() - inicio);
}

std::vector<std::string> TileMap::TodasTagsXML(const std::string& xml, const std::string& tag) const
{
    std::vector<std::string> resultado;
    std::string abertura = "<" + tag;
    std::string fechamento = "</" + tag + ">";
    size_t pos = 0;
    while (true) {
        size_t inicio = xml.find(abertura, pos);
        if (inicio == std::string::npos) break;
        size_t fim = xml.find(fechamento, inicio);
        if (fim == std::string::npos) {
            size_t autoFim = xml.find("/>", inicio);
            if (autoFim == std::string::npos) break;
            resultado.push_back(xml.substr(inicio, autoFim + 2 - inicio));
            pos = autoFim + 2;
        }
        else {
            resultado.push_back(xml.substr(inicio, fim + fechamento.size() - inicio));
            pos = fim + fechamento.size();
        }
    }
    return resultado;
}

static const std::string B64_CHARS =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string DecodeBase64(const std::string& in)
{
    std::string out;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (c == '\n' || c == '\r' || c == ' ') continue;
        if (c == '=') break;
        auto pos = B64_CHARS.find(c);
        if (pos == std::string::npos) continue;
        val = (val << 6) + (int)pos;
        valb += 6;
        if (valb >= 0) {
            out += (char)((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    return out;
}

static std::string TrimStr(const std::string& s)
{
    auto a = s.find_first_not_of(" \t\n\r");
    auto b = s.find_last_not_of(" \t\n\r");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

static std::string BuscarSrcImagem(const std::string& xml)
{
    size_t imgPos = xml.find("<image ");
    if (imgPos == std::string::npos) return "";
    size_t imgEnd = xml.find(">", imgPos);
    if (imgEnd == std::string::npos) return "";
    std::string imgTag = xml.substr(imgPos, imgEnd - imgPos);
    size_t srcPos = imgTag.find("source=\"");
    if (srcPos == std::string::npos) return "";
    srcPos += 8;
    size_t srcEnd = imgTag.find('"', srcPos);
    if (srcEnd == std::string::npos) return "";
    return imgTag.substr(srcPos, srcEnd - srcPos);
}

static bool DelimitadorTag(char c)
{
    return c == ' ' || c == '>' || c == '\t' || c == '\n' || c == '\r' || c == '/';
}

static std::vector<std::string> ExtrairTilesReais(const std::string& blocoTileset)
{
    std::vector<std::string> resultado;
    size_t pos = 0;
    while (true) {
        size_t i = blocoTileset.find("<tile", pos);
        if (i == std::string::npos) break;
        char prox = (i + 5 < blocoTileset.size()) ? blocoTileset[i + 5] : '\0';
        if (!DelimitadorTag(prox)) {
            pos = i + 5;
            continue;
        }
        size_t fim = blocoTileset.find("</tile>", i);
        if (fim == std::string::npos) {
            size_t autoFim = blocoTileset.find("/>", i);
            if (autoFim == std::string::npos) break;
            resultado.push_back(blocoTileset.substr(i, autoFim + 2 - i));
            pos = autoFim + 2;
        }
        else {
            resultado.push_back(blocoTileset.substr(i, fim + 7 - i));
            pos = fim + 7;
        }
    }
    return resultado;
}

void TileMap::ParseTileset(const std::string& xml, const std::string& pastaBase)
{
    TilesetInfo ts;
    ts.firstGid = std::stoi(AtributoXML(xml, "firstgid").empty() ? "1" : AtributoXML(xml, "firstgid"));
    ts.tileW = std::stoi(AtributoXML(xml, "tilewidth").empty() ? "16" : AtributoXML(xml, "tilewidth"));
    ts.tileH = std::stoi(AtributoXML(xml, "tileheight").empty() ? "16" : AtributoXML(xml, "tileheight"));
    ts.colunas = std::stoi(AtributoXML(xml, "columns").empty() ? "1" : AtributoXML(xml, "columns"));
    if (ts.colunas <= 0) ts.colunas = 1;

    std::string src = AtributoXML(xml, "source");
    bool ehExterno = !src.empty() && src.find(".tsx") != std::string::npos;

    if (ehExterno) {
        std::string conteudoTsx = LerArquivo(pastaBase + src);
        if (!conteudoTsx.empty()) {
            auto tileTags = ExtrairTilesReais(conteudoTsx);
            bool ehColecao = false;
            for (auto& t : tileTags)
                if (t.find("<image") != std::string::npos) { ehColecao = true; break; }

            if (ehColecao) {
                std::string pastaBaseTsx = pastaBase + src;
                size_t sep = pastaBaseTsx.find_last_of("/\\");
                std::string pastaTsx = (sep != std::string::npos) ? pastaBaseTsx.substr(0, sep + 1) : pastaBase;
                ts.ehColecao = true;
                for (auto& tileXml : tileTags) {
                    std::string idStr = AtributoXML(tileXml, "id");
                    if (idStr.empty()) continue;
                    int localId = std::stoi(idStr);
                    std::string imgSrc = BuscarSrcImagem(tileXml);
                    if (imgSrc.empty()) continue;

                    TileTextura tt;
                    tt.localId = localId;
                    tt.textura = new sf::Texture();
                    if (!tt.textura->loadFromFile(pastaTsx + imgSrc)) {
                        delete tt.textura;
                        tt.textura = nullptr;
                    }
                    else {
                        tt.textura->setSmooth(false);
                        tt.largura = (int)tt.textura->getSize().x;
                        tt.altura = (int)tt.textura->getSize().y;
                    }
                    ts.tileTexturas.push_back(tt);
                }
            }
            else {
                std::string imgSrc = BuscarSrcImagem(conteudoTsx);
                std::string imgTag = TagXML(conteudoTsx, "image");
                ts.largura = std::stoi(AtributoXML(imgTag, "width").empty() ? "0" : AtributoXML(imgTag, "width"));
                ts.altura = std::stoi(AtributoXML(imgTag, "height").empty() ? "0" : AtributoXML(imgTag, "height"));
                std::string tw = AtributoXML(conteudoTsx, "tilewidth");
                std::string th = AtributoXML(conteudoTsx, "tileheight");
                std::string co = AtributoXML(conteudoTsx, "columns");
                if (!tw.empty()) ts.tileW = std::stoi(tw);
                if (!th.empty()) ts.tileH = std::stoi(th);
                if (!co.empty()) ts.colunas = std::stoi(co);
                if (ts.colunas <= 0) ts.colunas = 1;

                std::string pastaBaseTsx = pastaBase + src;
                size_t sep = pastaBaseTsx.find_last_of("/\\");
                std::string pastaTsx = (sep != std::string::npos) ? pastaBaseTsx.substr(0, sep + 1) : pastaBase;

                if (!imgSrc.empty()) {
                    ts.textura = new sf::Texture();
                    if (!ts.textura->loadFromFile(pastaTsx + imgSrc)) {
                        delete ts.textura;
                        ts.textura = nullptr;
                    }
                    else {
                        ts.textura->setSmooth(false);
                        int realW = (int)ts.textura->getSize().x;
                        int realH = (int)ts.textura->getSize().y;
                        if (realW > 0) ts.largura = realW;
                        if (realH > 0) ts.altura = realH;
                        if (ts.tileW > 0 && ts.largura > 0)
                            ts.colunas = ts.largura / ts.tileW;
                        if (ts.colunas <= 0) ts.colunas = 1;
                    }
                }
            }
        }
    }
    else {
        auto tileTags = ExtrairTilesReais(xml);
        bool temColecaoInline = false;
        for (auto& t : tileTags)
            if (t.find("<image") != std::string::npos) { temColecaoInline = true; break; }

        if (temColecaoInline) {
            ts.ehColecao = true;
            for (auto& tileXml : tileTags) {
                std::string idStr = AtributoXML(tileXml, "id");
                if (idStr.empty()) continue;
                int localId = std::stoi(idStr);
                std::string imgSrc = BuscarSrcImagem(tileXml);
                if (imgSrc.empty()) continue;

                TileTextura tt;
                tt.localId = localId;
                tt.textura = new sf::Texture();
                if (!tt.textura->loadFromFile(pastaBase + imgSrc)) {
                    delete tt.textura;
                    tt.textura = nullptr;
                }
                else {
                    tt.textura->setSmooth(false);
                    tt.largura = (int)tt.textura->getSize().x;
                    tt.altura = (int)tt.textura->getSize().y;
                }
                ts.tileTexturas.push_back(tt);
            }
        }
        else {
            std::string imgSrc = BuscarSrcImagem(xml);
            std::string imgTag = TagXML(xml, "image");
            ts.largura = std::stoi(AtributoXML(imgTag, "width").empty() ? "0" : AtributoXML(imgTag, "width"));
            ts.altura = std::stoi(AtributoXML(imgTag, "height").empty() ? "0" : AtributoXML(imgTag, "height"));
            if (ts.largura > 0 && ts.tileW > 0 && ts.colunas <= 1)
                ts.colunas = ts.largura / ts.tileW;
            if (ts.colunas <= 0) ts.colunas = 1;
            if (!imgSrc.empty()) {
                ts.textura = new sf::Texture();
                if (!ts.textura->loadFromFile(pastaBase + imgSrc)) {
                    delete ts.textura;
                    ts.textura = nullptr;
                }
                else {
                    ts.textura->setSmooth(false);
                    int realW = (int)ts.textura->getSize().x;
                    int realH = (int)ts.textura->getSize().y;
                    if (realW > 0) ts.largura = realW;
                    if (realH > 0) ts.altura = realH;
                    if (ts.tileW > 0 && ts.largura > 0)
                        ts.colunas = ts.largura / ts.tileW;
                    if (ts.colunas <= 0) ts.colunas = 1;
                }
            }
        }
    }
    gTilesets.push_back(ts);
}

void TileMap::ParseCamada(const std::string& xml)
{
    Camada camada;
    camada.nome = AtributoXML(xml, "name");
    int largura = std::stoi(AtributoXML(xml, "width").empty() ? "0" : AtributoXML(xml, "width"));
    int altura = std::stoi(AtributoXML(xml, "height").empty() ? "0" : AtributoXML(xml, "height"));

    std::string dataTag = TagXML(xml, "data");
    std::string encoding = AtributoXML(dataTag, "encoding");

    size_t pos1 = dataTag.find('>');
    size_t pos2 = dataTag.rfind('<');
    if (pos1 == std::string::npos || pos2 == std::string::npos || pos2 <= pos1) return;
    std::string dados = dataTag.substr(pos1 + 1, pos2 - pos1 - 1);

    camada.tiles.resize(altura, std::vector<int>(largura, 0));

    if (encoding == "base64") {
        std::string raw = DecodeBase64(TrimStr(dados));
        int tileCount = largura * altura;
        const uint32_t FLIP_MASK = 0x1FFFFFFFu;
        for (int i = 0; i < tileCount && (i * 4 + 3) < (int)raw.size(); ++i) {
            uint32_t gid = (uint8_t)raw[i * 4]
                | ((uint8_t)raw[i * 4 + 1] << 8)
                | ((uint8_t)raw[i * 4 + 2] << 16)
                | ((uint8_t)raw[i * 4 + 3] << 24);
            gid &= FLIP_MASK;
            camada.tiles[i / largura][i % largura] = (int)gid;
        }
    }
    else {
        std::stringstream ss(dados);
        std::string val;
        int lin = 0, col = 0;
        while (std::getline(ss, val, ',')) {
            val.erase(std::remove(val.begin(), val.end(), '\n'), val.end());
            val.erase(std::remove(val.begin(), val.end(), '\r'), val.end());
            val.erase(std::remove(val.begin(), val.end(), ' '), val.end());
            if (!val.empty()) {
                try {
                    if (lin < altura && col < largura)
                        camada.tiles[lin][col] = std::stoi(val);
                }
                catch (...) {}
                col++;
                if (col >= largura) { col = 0; lin++; }
            }
        }
    }
    gCamadas.push_back(camada);
}

void TileMap::ParseObjectGroup(const std::string& xml)
{
    std::string grupo = AtributoXML(xml, "name");

    std::string grupoLower = grupo;
    for (auto& c : grupoLower) c = (char)std::tolower((unsigned char)c);
    bool ehColisao =
        grupoLower.find("colis") != std::string::npos; 

    float offX = std::stof(AtributoXML(xml, "offsetx").empty() ? "0" : AtributoXML(xml, "offsetx"));
    float offY = std::stof(AtributoXML(xml, "offsety").empty() ? "0" : AtributoXML(xml, "offsety"));
    auto objTags = TodasTagsXML(xml, "object");
    for (auto& objXml : objTags) {
        Objeto obj;
        obj.nome = AtributoXML(objXml, "name");
        obj.tipo = AtributoXML(objXml, "type");
        float x = std::stof(AtributoXML(objXml, "x").empty() ? "0" : AtributoXML(objXml, "x")) + offX;
        float y = std::stof(AtributoXML(objXml, "y").empty() ? "0" : AtributoXML(objXml, "y")) + offY;
        float w = std::stof(AtributoXML(objXml, "width").empty() ? "0" : AtributoXML(objXml, "width"));
        float h = std::stof(AtributoXML(objXml, "height").empty() ? "0" : AtributoXML(objXml, "height"));
        obj.rect = { x * ESCALA, y * ESCALA, w * ESCALA, h * ESCALA };

        std::string gidStr = AtributoXML(objXml, "gid");
        if (!gidStr.empty()) {
            uint32_t gidBruto = (uint32_t)std::stoul(gidStr);
            const uint32_t FLIP_MASK = 0x1FFFFFFFu;
            obj.gid = (int)(gidBruto & FLIP_MASK);
            obj.rect = { x * ESCALA, y * ESCALA, w * ESCALA, h * ESCALA };
            gObjetosTile.push_back(obj);
            continue;
        }

        if (w == 0.f && h == 0.f) {
            std::string pts = AtributoXML(objXml, "points");
            if (!pts.empty()) {
                std::vector<sf::Vector2f> poligono;
                std::stringstream ss(pts);
                std::string par;
                while (ss >> par) {
                    size_t v = par.find(',');
                    if (v == std::string::npos) continue;
                    float dpx = std::stof(par.substr(0, v));
                    float dpy = std::stof(par.substr(v + 1));
                    poligono.push_back({ (x + dpx) * ESCALA, (y + dpy) * ESCALA });
                }
                if (ehColisao && poligono.size() >= 2)
                    gColisoesPoly.push_back(poligono);
                continue;
            }
        }

        if (obj.nome == "SpawnElena" || obj.tipo == "trigger_spawn")
            gSpawnElena = { obj.rect.left, obj.rect.top };

        if (ehColisao)
            gColisoes.push_back(obj.rect);
        else
            gObjetos[grupo].push_back(obj);
    }
}

bool TileMap::Carregar(const std::string& caminhoTMX)
{
    std::string xml = LerArquivo(caminhoTMX);
    if (xml.empty()) return false;

    std::string pastaBase = "";
    size_t sep = caminhoTMX.find_last_of("/\\");
    if (sep != std::string::npos)
        pastaBase = caminhoTMX.substr(0, sep + 1);

    std::string mapTag = TagXML(xml, "map");
    gTileW = std::stoi(AtributoXML(mapTag, "tilewidth").empty() ? "16" : AtributoXML(mapTag, "tilewidth"));
    gTileH = std::stoi(AtributoXML(mapTag, "tileheight").empty() ? "16" : AtributoXML(mapTag, "tileheight"));
    gLargura = std::stoi(AtributoXML(mapTag, "width").empty() ? "0" : AtributoXML(mapTag, "width"));
    gAltura = std::stoi(AtributoXML(mapTag, "height").empty() ? "0" : AtributoXML(mapTag, "height"));

    auto tilesetTags = TodasTagsXML(xml, "tileset");
    for (auto& ts : tilesetTags)
        ParseTileset(ts, pastaBase);

    auto camadaTags = TodasTagsXML(xml, "layer");
    for (auto& c : camadaTags)
        ParseCamada(c);

    auto objGroupTags = TodasTagsXML(xml, "objectgroup");
    for (auto& og : objGroupTags)
        ParseObjectGroup(og);

    return !gCamadas.empty();
}

void TileMap::DesenharObjetosTile(sf::RenderTarget& window, sf::View& camera)
{
    const float MARGEM = 64.f * ESCALA;
    float camLeft = camera.getCenter().x - camera.getSize().x / 2.f - MARGEM;
    float camTop = camera.getCenter().y - camera.getSize().y / 2.f - MARGEM;
    float camRight = camLeft + camera.getSize().x + MARGEM * 2.f;
    float camBottom = camTop + camera.getSize().y + MARGEM * 2.f;

    for (auto& obj : gObjetosTile) {
        uint32_t gid = (uint32_t)obj.gid;

        TilesetInfo* ts = nullptr;
        for (int i = (int)gTilesets.size() - 1; i >= 0; i--) {
            if (gid >= (uint32_t)gTilesets[i].firstGid) { ts = &gTilesets[i]; break; }
        }
        if (!ts) continue;

        int localId = gid - ts->firstGid;

        if (ts->ehColecao) {
            sf::Texture* tex = nullptr;
            int imgW = 0, imgH = 0;
            for (auto& tt : ts->tileTexturas) {
                if (tt.localId == localId) {
                    tex = tt.textura;
                    imgW = tt.largura;
                    imgH = tt.altura;
                    break;
                }
            }
            if (!tex) continue;

            float px = obj.rect.left;
            float py = obj.rect.top - (float)imgH * ESCALA;

            if (px + imgW * ESCALA < camLeft || px > camRight) continue;
            if (py + imgH * ESCALA < camTop || py > camBottom) continue;

            sf::Sprite sprite(*tex);
            sprite.setScale(ESCALA, ESCALA);
            sprite.setPosition(std::round(px), std::round(py));
            window.draw(sprite);
        }
        else if (ts->textura && ts->colunas > 0) {
            int tx = (localId % ts->colunas) * ts->tileW;
            int ty = (localId / ts->colunas) * ts->tileH;

            float px = obj.rect.left;
            float py = obj.rect.top - ts->tileH * ESCALA;

            if (px + ts->tileW * ESCALA < camLeft || px > camRight) continue;
            if (py + ts->tileH * ESCALA < camTop || py > camBottom) continue;

            sf::Sprite sprite(*ts->textura);
            sprite.setTextureRect(sf::IntRect(tx, ty, ts->tileW, ts->tileH));
            sprite.setScale(ESCALA, ESCALA);
            sprite.setPosition(std::round(px), std::round(py));
            window.draw(sprite);
        }
    }
}

void TileMap::Desenhar(sf::RenderTarget& window, sf::View& camera, const std::string& ate)
{
    float tileW = gTileW * ESCALA;
    float tileH = gTileH * ESCALA;
    float camLeft = camera.getCenter().x - camera.getSize().x / 2.f;
    float camTop = camera.getCenter().y - camera.getSize().y / 2.f;
    float camRight = camLeft + camera.getSize().x;
    float camBottom = camTop + camera.getSize().y;
    int colMin = std::max(0, (int)(camLeft / tileW) - 2);
    int colMax = std::min(gLargura - 1, (int)(camRight / tileW) + 2);
    int linMin = std::max(0, (int)(camTop / tileH) - 2);
    int linMax = std::min(gAltura - 1, (int)(camBottom / tileH) + 2);

    for (auto& camada : gCamadas) {
        if (!ate.empty() && camada.nome == ate) break;
        for (int lin = linMin; lin <= linMax; lin++) {
            for (int col = colMin; col <= colMax; col++) {
                int gid = camada.tiles[lin][col];
                if (gid <= 0) continue;
                TilesetInfo* ts = nullptr;
                for (int i = (int)gTilesets.size() - 1; i >= 0; i--) {
                    if (gid >= gTilesets[i].firstGid) { ts = &gTilesets[i]; break; }
                }
                if (!ts || !ts->textura || ts->colunas <= 0) continue;
                int localId = gid - ts->firstGid;
                int tx = (localId % ts->colunas) * ts->tileW;
                int ty = (localId / ts->colunas) * ts->tileH;
                sf::Sprite sprite(*ts->textura);
                sprite.setTextureRect(sf::IntRect(tx, ty, ts->tileW, ts->tileH));
                sprite.setScale(ESCALA, ESCALA);
                sprite.setPosition(std::round(col * tileW), std::round(lin * tileH));
                window.draw(sprite);
            }
        }
    }

    DesenharObjetosTile(window, camera);
}

void TileMap::DesenharAcima(sf::RenderTarget& window, sf::View& camera, const std::string& partir)
{
    float tileW = gTileW * ESCALA;
    float tileH = gTileH * ESCALA;
    float camLeft = camera.getCenter().x - camera.getSize().x / 2.f;
    float camTop = camera.getCenter().y - camera.getSize().y / 2.f;
    float camRight = camLeft + camera.getSize().x;
    float camBottom = camTop + camera.getSize().y;
    int colMin = std::max(0, (int)(camLeft / tileW) - 1);
    int colMax = std::min(gLargura - 1, (int)(camRight / tileW) + 1);
    int linMin = std::max(0, (int)(camTop / tileH) - 1);
    int linMax = std::min(gAltura - 1, (int)(camBottom / tileH) + 1);

    bool encontrou = false;
    for (auto& camada : gCamadas) {
        if (!encontrou && camada.nome == partir) encontrou = true;
        if (!encontrou) continue;
        for (int lin = linMin; lin <= linMax; lin++) {
            for (int col = colMin; col <= colMax; col++) {
                int gid = camada.tiles[lin][col];
                if (gid <= 0) continue;
                TilesetInfo* ts = nullptr;
                for (int i = (int)gTilesets.size() - 1; i >= 0; i--) {
                    if (gid >= gTilesets[i].firstGid) { ts = &gTilesets[i]; break; }
                }
                if (!ts || !ts->textura || ts->colunas <= 0) continue;
                int localId = gid - ts->firstGid;
                int tx = (localId % ts->colunas) * ts->tileW;
                int ty = (localId / ts->colunas) * ts->tileH;
                sf::Sprite sprite(*ts->textura);
                sprite.setTextureRect(sf::IntRect(tx, ty, ts->tileW, ts->tileH));
                sprite.setScale(ESCALA, ESCALA);
                sprite.setPosition(std::round(col * tileW), std::round(lin * tileH));
                window.draw(sprite);
            }
        }
    }
}

static bool SegmentosCruzam(sf::Vector2f p1, sf::Vector2f p2,
    sf::Vector2f p3, sf::Vector2f p4)
{
    auto cross = [](sf::Vector2f a, sf::Vector2f b) { return a.x * b.y - a.y * b.x; };
    sf::Vector2f r = { p2.x - p1.x, p2.y - p1.y };
    sf::Vector2f s = { p4.x - p3.x, p4.y - p3.y };
    float rxs = cross(r, s);
    if (std::fabs(rxs) < 1e-6f) return false; 
    sf::Vector2f qp = { p3.x - p1.x, p3.y - p1.y };
    float t = cross(qp, s) / rxs;
    float u = cross(qp, r) / rxs;
    return (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f);
}

static bool PontoEmPoligono(sf::Vector2f p, const std::vector<sf::Vector2f>& poly)
{
    bool dentro = false;
    size_t n = poly.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        if (((poly[i].y > p.y) != (poly[j].y > p.y)) &&
            (p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) /
                (poly[j].y - poly[i].y) + poly[i].x))
            dentro = !dentro;
    }
    return dentro;
}

bool TileMap::Colide(const sf::FloatRect& rect) const
{
    for (auto& c : gColisoes)
        if (rect.intersects(c)) return true;

    sf::Vector2f cantos[4] = {
        { rect.left, rect.top },
        { rect.left + rect.width, rect.top },
        { rect.left + rect.width, rect.top + rect.height },
        { rect.left, rect.top + rect.height }
    };
    for (auto& poly : gColisoesPoly) {
        if (poly.size() < 2) continue;

        for (int k = 0; k < 4; k++)
            if (PontoEmPoligono(cantos[k], poly)) return true;

        for (auto& v : poly)
            if (rect.contains(v)) return true;

        for (size_t i = 0; i + 1 < poly.size(); i++) {
            for (int k = 0; k < 4; k++) {
                if (SegmentosCruzam(poly[i], poly[i + 1],
                    cantos[k], cantos[(k + 1) % 4]))
                    return true;
            }
        }
    }
    return false;
}

const std::vector<Objeto>& TileMap::GetObjetos(const std::string& grupo) const
{
    auto it = gObjetos.find(grupo);
    if (it != gObjetos.end()) return it->second;
    return gVazio;
}