#include <SFML/Graphics.hpp>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

int main() {
    // Testa SFML
    sf::RenderWindow window(sf::VideoMode(480, 200), "Teste SFML + FFmpeg");

    // Testa FFmpeg
    avformat_network_init();

    unsigned ver = avformat_version();
    std::cout << "FFmpeg avformat versao: "
        << AV_VERSION_MAJOR(ver) << "."
        << AV_VERSION_MINOR(ver) << "."
        << AV_VERSION_MICRO(ver) << std::endl;

    // Texto na janela
    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/arialbd.ttf");

    sf::Text texto;
    texto.setFont(font);
    texto.setString("SFML + FFmpeg funcionando!");
    texto.setCharacterSize(28);
    texto.setFillColor(sf::Color::Green);
    sf::FloatRect r = texto.getLocalBounds();
    texto.setOrigin(r.width / 2.f, r.height / 2.f);
    texto.setPosition(240.f, 100.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        window.clear(sf::Color(20, 20, 20));
        window.draw(texto);
        window.display();
    }

    return 0;
}