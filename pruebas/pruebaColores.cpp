#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>
#define PB push_back
using namespace std;

// Definimos la fuente que vamos a ocupar dentro de la ventana
sf::Font font;

pair<sf::RectangleShape, sf::Text> createRectangle(int szBtnX, int szBtny, int posX, int posY, string texto, int szTexto, int posTX, int posTY){
    // Creamos la figura base dell boton, junto con el color y posicion
    sf::RectangleShape button(sf::Vector2f(szBtnX, szBtny));
    button.setFillColor(sf::Color(200, 200, 200));
    button.setPosition(posX, posY);

    // Creamos el texto de cada boton
    sf::Text text(texto, font, szTexto);
    text.setFillColor(sf::Color::Black);
    text.setPosition(button.getPosition().x + posTX, button.getPosition().y + posTY);

    // Regresamos ambos elementos anteriores
    return std::make_pair(button, text);
}

int main(){
    font.loadFromFile("./Fonts/arial.ttf");

    vector <vector<int> > color_hormigas(4, vector<int>(3));
    
    color_hormigas[0] = {182,0,0};
    color_hormigas[1] = {182,96,255};
    color_hormigas[2] = {0,182,255};
    color_hormigas[3] = {0,255,0};

    int color_fondo[] = {0,0,0};
    int color_celdas[] = {255,255,255};
    const float margin = 10.f;


    sf::RenderWindow window(sf::VideoMode(860, 450), "Cambio de 12");

    auto botonOK = createRectangle(100, 60, 380, 370, "OK", 20, 30, 16);

    // Creamos toda la paleta de colores
    vector<sf::RectangleShape> palette(64);

    for (int i = 0; i < 64; i++){
        sf::RectangleShape tile(sf::Vector2f(43, 43));
        int index_x = (i % 16) + 1;
        int index_y = (i / 16) + 1;

        int x = (10 * index_x) + (43 * (index_x - 1));
        int y = (15 * index_y) + (43 * (index_y - 1));

        tile.setPosition(x, y);

        palette[i] = tile;
    }

    vector <int> valores = {0, 96, 182, 255};

    for (int i = 0, index = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            for (int k = 0; k < 4; k++, index++){
                palette[index].setFillColor(sf::Color(valores[i], valores[j], valores[k]));
            }
        }
    }

    vector<sf::RectangleShape> palette_clean(palette.begin(), palette.end());

    vector <string> nombre_botones = {
        "Reina",
        "Trabajadora",
        "Reprodcutora",
        "Soldado",
        "Celda viva",
        "Celda Muerta"
    };

    vector <pair< sf::RectangleShape, sf::Text>>  botones(6);
    
    for (int i = 0; i < 6; i++){
        int x = (15 * (i + 1)) + (126 * i), y = 280;
        int distancia = 5;
        if (i == 0) distancia = 35;
        else if (i == 1) distancia = 10;
        else if (i == 3) distancia = 25;
        else if (i == 4) distancia = 18;

        botones[i] = createRectangle(126, 40, x, y, nombre_botones[i], 18, distancia, 10);
    }


    int seleccion = 0;
    int seleccion_color = 32;

    // Del color seleccionado le agregamos una ayuda visual para mostrar en que posicion se encuentra
    palette[seleccion_color].setSize(palette[seleccion_color].getSize() - sf::Vector2f(margin * 2.f, margin * 2.f));
    palette[seleccion_color].setPosition(palette[seleccion_color].getPosition() + sf::Vector2f(margin, margin));

    while (window.isOpen()) {
        sf::Event event;
        window.clear(sf::Color(51,65,78));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // En caso de que el evento sea en donde se presiona el boton de el mouse, checamos que sea el izquierdo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                // Obtenemos la posicion del mouse
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                if (botonOK.first.getGlobalBounds().contains(mousePosF)){
                    // Finalmente cerramos la ventana
                    window.close();
                }

                // Recoremos el arreglo de botones para saber la seleccion del elemento a cambiar color
                for (int i = 0; i < botones.size(); i++){
                    if (botones[i].first.getGlobalBounds().contains(mousePosF)){
                        seleccion = i;
                        if (i == 0) seleccion_color = 32;
                        else if (i == 1) seleccion_color = 39;
                        else if (i == 2) seleccion_color = 11;
                        else if (i == 3) seleccion_color = 12;
                        else if (i == 4) seleccion_color = 63;
                        else if (i == 5) seleccion_color = 0;
                    }
                }

                 // Hacemos un recorrido por todos los colores disponibles y checamos si alguno de ellos fue presionado
                for (int i = 0; i < palette.size(); ++i){
                    if (palette[i].getGlobalBounds().contains(mousePosF)) seleccion_color = i;
                    palette = palette_clean;

                    // Del color seleccionado le agregamos una ayuda visual para mostrar en que posicion se encuentra
                    palette[seleccion_color].setSize(palette[seleccion_color].getSize() - sf::Vector2f(margin * 2.f, margin * 2.f));
                    palette[seleccion_color].setPosition(palette[seleccion_color].getPosition() + sf::Vector2f(margin, margin));

                    // Aplicamos los cambios al color dependiendo de que boton fue seleccionado
                    int r = static_cast<int> (palette[seleccion_color].getFillColor().r);
                    int g = static_cast<int> (palette[seleccion_color].getFillColor().g);
                    int b = static_cast<int> (palette[seleccion_color].getFillColor().b);

                    if (seleccion < 4){
                        color_hormigas[seleccion][0] = r;
                        color_hormigas[seleccion][1] = g;
                        color_hormigas[seleccion][2] = b;

                    }
                    else{
                        if (seleccion == 4){
                            color_celdas[0] = r;
                            color_celdas[1] = g;
                            color_celdas[2] = b;
                        }
                        else{
                            color_fondo[0] = r;
                            color_fondo[1] = g;
                            color_fondo[2] = b;
                        }
                    }

                }

            }
        
        }

        for (auto &tile : palette){
            int r, g, b;
            bool bandera_color = true;

            // Quitamos todos los valores que son usados por las celdas
            if (seleccion != 4)
                if (sf::Color(color_celdas[0], color_celdas[1], color_celdas[2])  == tile.getFillColor())  bandera_color = false;

            if (seleccion != 5)
                if (sf::Color(color_fondo[0], color_fondo[1], color_fondo[2])  == tile.getFillColor())  bandera_color = false;

            for (int i = 0; i < color_hormigas.size(); i++){
                if (i != seleccion)
                    if (sf::Color(color_hormigas[i][0], color_hormigas[i][1], color_hormigas[i][2])  == tile.getFillColor()) bandera_color = false;
            }

            // No debemos mostrar el tile del color ya seleccionado
            if (bandera_color) window.draw(tile);
        }

        for (int i = 0; i < botones.size(); i++){
            if (seleccion == i) botones[i].first.setFillColor(sf::Color(96, 96, 96));
            else botones[i].first.setFillColor(sf::Color(200, 200, 200));
        }


        for (auto &boton : botones){
            window.draw(boton.first);
            window.draw(boton.second);
        }

        // Mostramos en pantalla el boton de OK
        window.draw(botonOK.first);
        window.draw(botonOK.second);


        // Mostramos todos los valores anteriores
        window.display();
    }

    return 0;
}