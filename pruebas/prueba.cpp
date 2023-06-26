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
    bool opcion = false;

    int distribucion_n = 100;
    vector <int> densidad_hormigas = {1, 55, 9, 35};

    sf::RenderWindow window(sf::VideoMode(500, 550), "Cambio de 12");

    // Creamos las etiquetas para mostrar los valores a cambiar
    vector < pair<sf::RectangleShape, sf::Text> > etiquetas = {
        createRectangle( 60, 40, 10,  30, "N : ", 22, 5, 5),
        createRectangle(180, 40, 10, 120, "Reina : ", 20, 5, 5),
        createRectangle(180, 40, 10, 210, "Trabajadora : ", 20, 5, 5),
        createRectangle(180, 40, 10, 300, "Reproductora : ", 20, 5, 5),
        createRectangle(180, 40, 10, 390, "Soldado : ", 20, 5, 5)
    };


    // Creamos los input boxes con los valores que ya se tienen dentro del programa
    vector < pair<sf::RectangleShape, sf::Text> > input_boxes = {
        createRectangle(250, 40, 200,  30, to_string(distribucion_n), 22, 5, 5),
        createRectangle(250, 40, 200, 120, to_string(densidad_hormigas[0]), 20, 5, 5),
        createRectangle(250, 40, 200, 210, to_string(densidad_hormigas[1]), 20, 5, 5),
        createRectangle(250, 40, 200, 300, to_string(densidad_hormigas[2]), 20, 5, 5),
        createRectangle(250, 40, 200, 390, to_string(densidad_hormigas[3]), 20, 5, 5)
    };

    auto botonOK = createRectangle(100, 60, 180, 470, "OK", 20, 30, 16);

    for (int i = 0; i < etiquetas.size(); i++){
        etiquetas[i].first.setFillColor(sf::Color(51, 65, 78));
        etiquetas[i].second.setFillColor(sf::Color(255, 255, 255));
    }

    for (int i = 0; i < input_boxes.size(); i++)
        input_boxes[i].first.setFillColor(sf::Color(255, 255, 255));


    int seleccion = -1;
    string actual = "";

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

                // Dentro del siguiente recorrido por todos los botones, identificamos cual fue presionado para saber que 
                // input box cambia conforme a la entrada de datos del usuario
                for (int i = 0; i < input_boxes.size(); i++){
                    if (input_boxes[i].first.getGlobalBounds().contains(mousePosF)) seleccion = i;
                }

                if (botonOK.first.getGlobalBounds().contains(mousePosF)){
                    // Aplicamos los cambios
                    // Cerramos ventana

                    for (int i = 0; i < input_boxes.size(); i++){
                        string temp = input_boxes[i].second.getString();

                        bool number = true;

                        // Checamos que los valores ingresados sean numeros
                        for (int i = 0; i < temp.size() && number; i++){
                            if (!(temp[i] >= '0' && temp[i] <= '9')) number = false;
                        }

                        // Revisamos si la cadena NO esta vacia y tiene una cantidad menor a 9 digitos y la bandera
                        // que nos dice si estamos trabajando con un numero o no
                        if (temp.size() > 0 && temp.size() <= 9 && number){
                            int valor = stoi(temp);

                            // Asignamos los valores a las variables
                            if (i == 0) distribucion_n = valor;
                            else densidad_hormigas[i - 1] = valor;
                        }
                    }

                    // Cerramos la pantalla para acabar con el proceso
                    window.close(); 
                }

            }

            // Si el evento fue la entrada de texto, significa que algun valor de los input boxes cambia
            // Pero tambien checamos si es que algun input box fue seleccionado
            else if (event.type == sf::Event::TextEntered && seleccion != -1) {
                actual = input_boxes[seleccion].second.getString();

                // Menejamos el caso en el que se presione la tecla de borrar
                if (event.text.unicode == '\b'){ 
                    // Revisamos si es que dentro de la cadena existen valores para poder retirarlos
                    if (actual.size() > 0) actual.pop_back();
                }

                // Manjemos si alguna tecla del alfabeto fue presionada 
                else if (event.text.unicode < 128) {
                    // La agregamos a la cadena que vamos a mostrar
                    actual += static_cast<char>(event.text.unicode);
                }

                // Hacemos un update de los valores mostrados dentro de los input boxes
                input_boxes[seleccion].second.setString(actual);
            }
        }
        

        // Mostramos en pantalla todas las etiquetas y los input boxes
        for (int i = 0; i < etiquetas.size(); i++){
            window.draw(etiquetas[i].first);
            window.draw(etiquetas[i].second);

            window.draw(input_boxes[i].first);
            window.draw(input_boxes[i].second);
        }

        // Mostramos en pantalla el boton de OK
        window.draw(botonOK.first);
        window.draw(botonOK.second);


        // Mostramos todos los valores anteriores
        window.display();
    }

    return 0;
}