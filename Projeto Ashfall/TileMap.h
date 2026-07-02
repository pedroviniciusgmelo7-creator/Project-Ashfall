#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
struct TileTextura {
    sf::Texture* textura = nullptr;
    int          localId = 0;
    int          largura = 0;
    int          altura = 0;
};
struct TilesetInfo {
    sf::Texture* textura = nullptr;
    std::vector<TileTextura> tileTexturas;
    int          firstGid = 1;
    int          largura = 0;
    int          altura = 0;
    int          tileW = 16;
    int          tileH = 16;
    int          colunas = 1;
    bool         ehColecao = false;
};
struct Objeto {
    std::string   nome;
    std::string   tipo;
    sf::FloatRect rect;
    int           gid = 0;
};
struct Camada {
    std::string                   nome;
    std::vector<std::vector<int>> tiles;
};
class TileMap {
public:
    TileMap();
    ~TileMap();
    bool Carregar(const std::string& caminhoTMX);
    void Desenhar(sf::RenderTarget& window, sf::View& camera, const std::string& ate = "");
    void DesenharAcima(sf::RenderTarget& window, sf::View& camera, const std::string& partir = "Topo");
    void DesenharObjetosTile(sf::RenderTarget& window, sf::View& camera);
    int  GetLarguraPx() const { return gLargura * gTileW * 3; }
    int  GetAlturaPx()  const { return gAltura * gTileH * 3; }
    bool Colide(const sf::FloatRect& rect) const;
    sf::Vector2f GetSpawnElena() const { return gSpawnElena; }
    const std::vector<Objeto>& GetObjetos(const std::string& grupo) const;
private:
    void ParseTileset(const std::string& xml, const std::string& pastaBase);
    void ParseCamada(const std::string& xml);
    void ParseObjectGroup(const std::string& xml);
    std::string LerArquivo(const std::string& caminho) const;
    std::string AtributoXML(const std::string& xml, const std::string& attr) const;
    std::string TagXML(const std::string& xml, const std::string& tag) const;
    std::vector<std::string> TodasTagsXML(const std::string& xml, const std::string& tag) const;
    std::vector<TilesetInfo>               gTilesets;
    std::vector<Camada>                    gCamadas;
    std::vector<sf::FloatRect>             gColisoes;
    std::vector<std::vector<sf::Vector2f>> gColisoesPoly;
    std::vector<Objeto>                    gObjetosTile;
    std::map<std::string, std::vector<Objeto>> gObjetos;
    sf::Vector2f                           gSpawnElena = { 400.f, 300.f };
    int gTileW = 16;
    int gTileH = 16;
    int gLargura = 0;
    int gAltura = 0;
    static const float               ESCALA;
    static const std::vector<Objeto> gVazio;
};