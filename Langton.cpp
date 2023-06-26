#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>
#include <gtk/gtk.h>
#define PB push_back
using namespace std;

// Declaracion del size total de programa junto con el size de cada celda
int n = 700;
int n_archivo = 0;
int sizeCelda_X, sizeCelda_Y;

// Declaracion de la estructura de la hormiga que se ocupara dentro del programa
struct Ant {
    int edad;
    int direccion;
    int tipo;
};

// La densidad de hormigas la guardaremos dentro de un arreglo para tener un control general de este
vector <int> densidad_hormigas(4);

// Variable de ayuda para el control de la distribucion de hormigas dentro del tablero
int distribucion_n = 350;

// Matrices necesarias para los diferentes estados de la simulacion
// Son enteros ya que guardaremos el tipo de hormiga en cada celda
vector <vector <bool>> matrix(n, vector <bool> (n, false));
vector <vector <bool>> matrix_clean(n, vector<bool> (n, false));
map <pair<int,int>, bool> cambios_matrix;


// Arreglo para mantener el control de las hormigas que existen
set <pair<int,int>> celdas_vivas;


// Arreglo para almacenar las hormigas que se encuentren en la simulacion
// Almacenamos las coordenadas de la hormiga y la informacion relevante a esa hormiga
map < pair<int,int>, Ant> hormigas;

// El arreglo temporal unicamente lo utilizaremos para mostrar los cambios que se hayan hecho 
// a las hormigas en un instante de tiempo
map < pair<int,int>, Ant> hormigas_temporal;


// Arreglo para mantener el control de las hormigas que mueren en la siguiente iteracion para evitar 
// errores dentro del codigo
set < pair<int,int> > borrar;
set<pair<int,int>> nacimiento_condicion;

// Indicador para conocer la seleccion de la hormiga a colocar en el tablero
int hormiga_tipo_tablero = 0;


// Arreglos para manetener un control de todos los valores que se usan en las graficas
vector <int> valores_grafica_densidad;
vector <int> valores_grafica_entriopia;
unordered_map <int,int> entropy;

// Definimos los valores RGB del fondo de la pantalla de la simulacion
int color_fondo[] = {0,0,0};
int color_celdas[] = {255,255,255};

vector <vector <int> > color_hormigas(4, vector <int> (3));

// Declaracion de variables para tener un control del scroll que se aplica
int index_visual_x = 0;
int index_visual_y = 0;
int valor_scroll = 1;

// Variables necesarias para mostrar en pantalla su valor
int total_iteraciones = 0;
int total_celdas_vivas = 0;
int total_celdas_vivas_entriopia = 0;

// varibles necesarias para el control del zoom
int index_zoom = 10;
vector <int> zoom = {/*1, 2,*/ 4, 5, 7, 10, 14, 20, 25, 28, 35, 50, 70, 100, 140, 175, 350, 700};

// Definimos banderas que nos ayudan a mantener el control de acciones especificas del programa
bool bandera_automatico = false;
bool bandera_nulo = true;

// Definimos la fuente que vamos a ocupar dentro de la ventana
sf::Font font;
sf::RenderTexture inner;

// Size del margen en pixeles
const float margin = 10.f;

// Cadena para poder escribir y leer de un archivo de texto
string valores_archivo;


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

void updateColors(){
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
   return;
}

void changeValues(){

    sf::RenderWindow window(sf::VideoMode(500, 550), "Cambiar Configuracion");

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

    return;
}

void updateConfiguration(){
    sf::RenderWindow windowConfig(sf::VideoMode(500, 350), "Configuracion");

    vector<std::pair<sf::RectangleShape, sf::Text>> buttons = {
        createRectangle(220, 60, 140, 30,  "Cambiar Color", 20, 45, 17),
        createRectangle(220, 60, 140, 160, "Cambiar Valores", 20, 35, 17),
    };

    auto botonOK = createRectangle(100, 60, 200, 270, "OK", 20, 30, 16);

    while (windowConfig.isOpen()){
        sf::Event event;
        while (windowConfig.pollEvent(event)){
            if (event.type == sf::Event::Closed) windowConfig.close();

            // En caso de que el evento sea en donde se presiona el boton de el mouse, checamos que sea el izquierdo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                // Obtenemos la posicion del mouse
                sf::Vector2i mousePos = sf::Mouse::getPosition(windowConfig);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                // Hacemos un recorrido por todos los botones creados
                for (int i = 0; i < buttons.size(); i++){
                    if (buttons[i].first.getGlobalBounds().contains(mousePosF)) {
                        if (buttons[i].second.getString() == "Cambiar Valores"){
                            windowConfig.setVisible(false);
                            changeValues();
                            windowConfig.setVisible(true);
                        }
                        else{
                            windowConfig.setVisible(false);
                            updateColors();
                            windowConfig.setVisible(true);
                        }
                    }
                }

                // Cerramos la pantalla en caso de que el boton OK sea presionado
                if (botonOK.first.getGlobalBounds().contains(mousePosF)) windowConfig.close();

            }
        }

        windowConfig.clear(sf::Color(51,65,78));

        // Draw the preview rectangle and the palette to the window
        for (auto& button : buttons) {
            windowConfig.draw(button.first);
            windowConfig.draw(button.second);
        }

        windowConfig.draw(botonOK.first);
        windowConfig.draw(botonOK.second);


        windowConfig.display();
    }

}

void updateGraphics(){

    // Limpiamos el tablero de juego con el color correspondiente
    inner.clear(sf::Color(color_fondo[0], color_fondo[1], color_fondo[2]));

    // Asignamos el tamaño correspondiente a cada celda dependiendo el zoom actual
    sizeCelda_X = zoom[index_zoom];
    sizeCelda_Y = zoom[index_zoom];

    // Creamos la figura base de cada celda viva
    sf :: RectangleShape celda(sf::Vector2f(sizeCelda_X, sizeCelda_Y));

    // Hacemos las operaciones para saber cuantas celdas debemos imprimir en ambos ejes
    int cantidad_x = 1400 /sizeCelda_X;
    int cantidad_y = 700/sizeCelda_Y;

    // Recorremos la estructura de datos en la que almacenamos las celdas vivas
    for (auto i: celdas_vivas){
        if (i.first - index_visual_x >= 0 && i.first - index_visual_x <= cantidad_x){
            if (i.second - index_visual_y >= 0 && i.second - index_visual_y <= cantidad_y){
                int x = i.first - index_visual_x;
                int y = i.second - index_visual_y;

                celda.setPosition(x * sizeCelda_X, y * sizeCelda_Y);

                celda.setFillColor(sf::Color(color_celdas[0], color_celdas[1], color_celdas[2]));
                inner.draw(celda);
            }
        }
    }

    // Ponemos todas las hormigas en el tablero
    // Si recorremos el arreglo temporal, tenemos que poner las etiquetas en cada celda
    for (auto i : hormigas_temporal){
        int x = i.first.first - index_visual_x;
        int y = i.first.second - index_visual_y;

        if (x >= 0 && x <= cantidad_x && y >= 0 && y <= cantidad_y){
            auto hormiga = createRectangle(sizeCelda_X, sizeCelda_Y, x*sizeCelda_X, y*sizeCelda_Y, ">", sizeCelda_X, sizeCelda_X / 5, - (sizeCelda_Y / 7));
            hormiga.first.setFillColor(sf::Color(color_hormigas[i.second.tipo][0], color_hormigas[i.second.tipo][1], color_hormigas[i.second.tipo][2]));

            string direccion_hormiga = "";

            // 1 -> Arriba, 2-> Derecha, 3 -> Abajo, 4 -> Izquierda
            if (i.second.direccion == 1) direccion_hormiga = "^";
            if (i.second.direccion == 2) direccion_hormiga = ">";
            if (i.second.direccion == 3) direccion_hormiga = "v";
            if (i.second.direccion == 4) direccion_hormiga = "<";

            hormiga.second.setString(direccion_hormiga);

            inner.draw(hormiga.first);
            inner.draw(hormiga.second);
        }
    }

    // Dibujamos las hormigas que tenemos pero unicamente el color que tiene cada una
    for (auto i : hormigas){
        int x = i.first.first - index_visual_x;
        int y = i.first.second - index_visual_y;

        if (x >= 0 && x <= cantidad_x && y >= 0 && y <= cantidad_y){
            celda.setPosition(x * sizeCelda_X, y * sizeCelda_Y);
            celda.setFillColor(sf::Color(color_hormigas[i.second.tipo][0], color_hormigas[i.second.tipo][1], color_hormigas[i.second.tipo][2]));
            inner.draw(celda);
        }
    }

    return ;
}

int getNewPosition(vector <int> direcciones){
    // Creamos un generador de numeros aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());

    // Seleccionamos un numero entre 1 y 2 para las posiciones disponibles
    std::uniform_int_distribution<> dis(0, direcciones.size() - 1);

    int randomIndex = dis(gen);

    return direcciones[randomIndex];
}

int getIndices(){
    // Generar un generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());

    // Crear una distribución uniforme para las coordenadas
    std::uniform_int_distribution<int> distribution(0, n - 1);

    return (distribution(gen));
}

int getProbability(){
     // Creamos un generador de numeros aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());

    // Seleccionamos un numero entre 1 y 2 para las posiciones disponibles
    std::uniform_int_distribution<> dis(0, 100);

    return dis(gen);
}

pair <int,int> getPossibleCell(int x, int y){
    if (bandera_nulo){
        // Checamos todas las celdas ayacentas a ambas celdas y ponemos la nueva celda en la primer celda disponible
        // Arriba
        if (y - 1 >= 0){
            for (int i = -1; i <= 2; i++)
                if (celdas_vivas.find({x + i, y - 1}) == celdas_vivas.end()) return {x + i, y - 1};
        }

        // Centro
        if (x - 1 >= 0)
            if (celdas_vivas.find({x - 1, y}) == celdas_vivas.end()) return {x - 1, y};
        if (x + 2 < n)
            if (celdas_vivas.find({x + 2, y}) == celdas_vivas.end()) return {x + 2, y};

        // Abajo
        if (y + 1 < n){
            for (int i = -1; i <= 2; i++)
                if (celdas_vivas.find({x + i, y + 1}) == celdas_vivas.end()) return {x + i, y + 1};
        }

        return {-1, -1};
    }
    else{
        int indexArriba  = (y - 1 >= 0 ? y - 1 : n - 1);
        int indexAbajo = (y + 1 < n? y + 1 : 0);

        for (int i = -1; i <= 2; i++){
            int new_x = (x + i < 0 ? n - 1 : (x + i < n ? x + i : 0));
            if (celdas_vivas.find({new_x, indexArriba}) == celdas_vivas.end()) return {new_x, indexArriba};
            if (celdas_vivas.find({new_x, indexAbajo}) == celdas_vivas.end()) return {new_x, indexAbajo};
        }

        // Centro
        if (celdas_vivas.find({(x - 1 >= 0 ? x - 1 : n - 1), y}) == celdas_vivas.end()) return {(x - 1 >= 0 ? x - 1 : n - 1), y};
        if (celdas_vivas.find({(x + 2 < n ? x + 2 : n - 1), y}) == celdas_vivas.end()) return {(x + 2 < n ? x + 2 : n - 1), y};

        return {-1, -1};
    }
}

pair <int,int> checkMovement(int direccion, int x, int y){
    // Direccion 5:
    // 1 -> arriba
    // 2 -> derecha
    // 3 -> abajo
    // 4 -> izquierda

    if (direccion == 1) {
        if (y - 1 >= 0) y -= 1;
        else 
            if (bandera_nulo) y = n - 1;
    }

    else if (direccion == 2){
        if (x + 1 < n) x += 1;
        else
            if (bandera_nulo) x = 0;;
    }

    else if (direccion == 3){
        if (y + 1 < n) y += 1;
        else
            if (bandera_nulo) y = 0;;
    }

    else{
        if (x - 1 >= 0) x -= 1;
        else
            if (bandera_nulo) x = n -1;;
    }

    if(hormigas.find({x, y}) == hormigas.end()) return {x,y};
    return {-1,-1};
}

pair <pair<int,int> ,Ant> checkFront(int direccion, int x, int y, bool bandera_nacimiento){
    int original_x = x;
    int original_y = y;

    int new_x = x;
    int new_y = y;
    int new_direccion = 1;

    // 1 -> arriba
    // 2 -> derecha
    // 3 -> abajo
    // 4 -> izquierda

    if (direccion == 1 || direccion == 3){
        // Si la direccion en la que llego la hormiga es 1, entonces cual debe ser su siguiente posicion a chear?
        // Pues el de la izquierda, bueno, dependiendo de la celda actual

        // En caso de que la celda donde se encuentre encendida, quiere decir que va a realizar un giro a la derecha
        if (matrix[original_y][original_x] == true){
            new_direccion = (direccion == 1 ? 2 : 4);
        }
        else // En caso contrario, va a realizar un giro a la izquierda
            new_direccion = (direccion == 1 ? 4 : 2);

        if (new_direccion == 4) {
            if (new_x - 1 >= 0) new_x -= 1;
            else if (bandera_nulo) new_x = n -1;
        }
        else {
            if (new_x + 1 < n) new_x += 1;
            else if (bandera_nulo) new_x = 0;
        }  
    }
    else{
        if (matrix[original_y][original_x] == true)
            new_direccion = (direccion == 2 ? 3 : 1);
        else
            new_direccion = (direccion == 2 ? 1 : 3);

        if (new_direccion == 1){
            if (new_y - 1 >= 0) new_y -= 1;
            else if (bandera_nulo) new_y = n -1;
        }
        else {
            if (new_y + 1 < n) new_y += 1;
            else if (bandera_nulo) new_y = 0;
        }
    }

    // Ya camprobamos que existe    una hormiga en frente
    if (hormigas.find({new_x, new_y}) != hormigas.end()){

        // Comprobamos de que la reina que se encuentre en frente sea reina y se encuentre con direccion a nosotros

        bool bandera_direccion = false;
        int existingDireccion = hormigas[{new_x, new_y}].direccion;
    
        if (matrix[y][x] == true && (new_direccion == (existingDireccion % 4) + 1)){
            bandera_direccion = true;
        }
        else{
            existingDireccion -= 1;
            existingDireccion = (existingDireccion == 0 ? 4 : existingDireccion);
            if (matrix[y][x] == false && (new_direccion == existingDireccion)) bandera_direccion = true;
        }

        if (bandera_nacimiento){
            // En caso de que alguna de las 2 hormigas involucradas ya haya sido tomada en cuenta, no sigo con la condicion ya que nacerian muchas 
            // mas hormigas
            if (nacimiento_condicion.find({x, y}) != nacimiento_condicion.end() || nacimiento_condicion.find({new_x, new_y}) != nacimiento_condicion.end())
                bandera_direccion = false;

            // Si yo soy reprodcutora, busco a la reina, y si se cumple la condicion de 180 grados, nace una nueva hormiga
            if (hormigas[{x,y}].tipo == 2 && hormigas[{new_x, new_y}].tipo == 0 && bandera_direccion){
                nacimiento_condicion.insert({x,y});
                nacimiento_condicion.insert({new_x,new_y});
                Ant actual;
                actual.edad = 0;
                actual.direccion = 0;
                actual.tipo = 3;
                pair <int,int> coordenadas_posibles = getPossibleCell(x, y);
                return {coordenadas_posibles, actual};
            }

            // Si yo soy reina, busca la reprodcutora
            if (hormigas[{x,y}].tipo == 0 && hormigas[{new_x, new_y}].tipo == 2 && bandera_direccion){
                nacimiento_condicion.insert({x,y});
                nacimiento_condicion.insert({new_x,new_y});
                Ant actual;
                actual.edad = 0;
                actual.direccion = 0;
                actual.tipo = 3;
                pair <int,int> coordenadas_posibles = getPossibleCell(x, y);
                return {coordenadas_posibles, actual};
            }
        }

        else{ // En caso de que no queramos buscar la condicion de nacimiento, buscaremos 2 hormigas reinas

            if (hormigas[{x,y}].tipo == 0 && hormigas[{new_x, new_y}].tipo == 0 && bandera_direccion){
                int edad_a = hormigas[{original_x, original_y}].edad;
                int edad_b = hormigas[{new_x, new_y}].edad;

                int delta = abs(edad_a - edad_b);
                pair <int,int> control;

                if (delta <= 10){
                    // Tenemos una probabilidad del 50% de que una hormiga desaparezca
                    int probabilidad = getProbability();
                    int condicional = (edad_a < 60 ? 50 : 20);

                    if (probabilidad <= condicional){
                        borrar.insert({original_x, original_y});
                        control.first = INT_MAX;
                    }

                    probabilidad = getProbability();
                    condicional = (edad_b < 60 ? 50 : 20);

                    if (probabilidad <= condicional) {
                        borrar.insert({new_x, new_y});
                        control.second = INT_MAX;
                    }
                }

                return {control, {}};
            }
        }

    }

    return {{-1, -1}, {}};
}

void nextState(){
    // Como unicamente tengo que tomar en cuenta las hormigas que estan vivas o que existen
    // entonces estaria mejor que unicamente ocupe el arreglo de donde se encutran las hormigas, no?
    // Distribucion del tipo de hormigas
    // 0 -> Reinas
    // 1 -> Trabajadoras
    // 2 -> Reproductoras
    // 3 -> Soldado


    std::srand(std::time(0));

    // Que tengo que recorrer? Tengo que checar el arreglo de las hormigas
    int cantidad_total = hormigas.size();
    map<pair<int,int>, Ant>:: iterator i = hormigas.begin();
    map <pair<int,int>, Ant> nacimiento;

    for(; i != hormigas.end(); i++){
        int x = i->first.first;
        int y = i->first.second;

        // Comprobamos si es la hormiga existe en la iteracion actual
        if (borrar.find({x,y}) == borrar.end()){
            // En caso de que la hormiga cumpla con la condicion maxima de iteraciones, se quitara de la simulacion
            if (i->second.edad == 80) borrar.insert({x, y});
            else {
                i->second.edad += 1;

                int direccion = i->second.direccion;
                // Checamos la condicion y comprobar si es que alguna nueva hormiga nace

                if (matrix[y][x] == true){
                    direccion = (direccion + 1) % 5;
                    direccion += (direccion == 0 ? 1 : 0);
                    cambios_matrix[{x,y}] = false;
                    celdas_vivas.erase({x, y});
                }
                else{
                    direccion -= 1;
                    direccion = (direccion == 0 ? 4 : direccion); 
                    cambios_matrix[{x,y}] = true;
                    celdas_vivas.insert({x, y});
                }

                pair <int,int> coordenadas = checkMovement(direccion, x , y);

                // Checamos la posicion despues del giro y si es posible se agrega a esa posicion
                if (coordenadas.first != -1){
                    auto actual = hormigas[{x, y}];
                    actual.direccion = direccion;

                    borrar.insert({x, y});

                    nacimiento[{coordenadas.first, coordenadas.second}] = actual;
                }
                else {
                    // En caso de que no nos podamos mover despues del giro, eso quiere decir que existe una hormiga en ese lugar
                    // por lo que aplicamos las condiciones de nacimiento y de muerte de hormigas
                    
                    // Comprobamos si la condicion en donde dos reinas se encuentren
                    if (i->second.tipo == 0) {
                        checkFront(i->second.direccion, x, y, false);
                    }

                    else{
                        // En caso de que la hormiga actual sea reproductora queremos comprobar si se da la condicion de nacimiento
                        if (i->second.tipo == 2){
                            auto hormiga_nacimiento = checkFront(i->second.direccion, x, y, true);
                            // En caso de que se cumpla la condicion de nacimiento, agregamos la hormiga al arreglo de nacimiento de hormigas
                            if (hormiga_nacimiento.first.first >= 0) {
                                int random_number = (std::rand() % 4);
                                hormiga_nacimiento.second.tipo = random_number;
                                random_number += 1;
                                hormiga_nacimiento.second.direccion = random_number;

                                nacimiento[hormiga_nacimiento.first] = hormiga_nacimiento.second;
                            }
                        }

                        // Ponemos dentro del arreglo de las nuevas direcciones la nuevas direcciones
                        // para poder seleccionarla de manera aleatoria, exluyo la direccion de la que vengo
                        // y la que no fue posible en un inicio
                        vector <int> direcciones = {direccion + 1, direccion + 2};

                        // Seleccionamos la nueva direccion
                        int nueva_direccion = getNewPosition(direcciones);
                        // Checamos si esa nueva posicion es valida
                        coordenadas = checkMovement(nueva_direccion, x , y);


                        // En caso de que sea posible agregamos esa nueva posicion
                        if (coordenadas.first != -1){
                            auto actual = hormigas[{x, y}];
                            actual.direccion = nueva_direccion;

                            borrar.insert({x, y});

                            nacimiento[{coordenadas.first, coordenadas.second}] = actual;
                        }
                    }
                }
            }
        }
    }

    // Agregamos todas las hormigas que salieron de la condicion de nacimiento
    for (auto i : borrar)
        hormigas.erase(hormigas.find({i.first, i.second}));

    for (auto i : nacimiento)
        hormigas[{i.first.first, i.first.second}] = i.second;

    for (auto i : cambios_matrix)
        matrix[i.first.second][i.first.first] = i.second; 

    borrar.clear();

    return;
}

void boardHandler(int x, int y){
    bool bandera_hormiga = (hormigas.find({x,y}) != hormigas.end());

    // En caso de que exista la hormiga en el arreglo general, la movemos al arreglo temporal
    if (bandera_hormiga){
        Ant actual = hormigas[{x, y}];
        hormigas.erase(hormigas.find({x,y}));

        actual.direccion += 1;
        actual.direccion %= 5;

        if (actual.direccion != 0)
            hormigas_temporal[{x,y}] = actual;
    }
    else{
        // Bandera para conocer si es que existe la hormiga en el arreglo temporal
        bool bandera_hormiga_temporal = (hormigas_temporal.find({x,y}) != hormigas_temporal.end());

        // Existe la hormiga en el arreglo temporal? -> cambiar la direccion unicamente
        if (bandera_hormiga_temporal){
            Ant actual = hormigas_temporal[{x, y}];
            actual.direccion += 1;
            actual.direccion %= 5;

            // En caso de que la direccion no sea la condicion para desaparecer unicamente cambiar su direccion
            if (actual.direccion != 0)
                hormigas_temporal[{x,y}] = actual;
            else // En caso contrario, quitamos a la hormiga del arreglo temporal
                hormigas_temporal.erase(hormigas_temporal.find({x,y}));
        }
        else{ // En caso de que la hormiga no existe en el arreglo temporal, quiere decir que es una nueva hormiga
            Ant actual;
            actual.direccion = 1;
            actual.edad = 0;

            /// --------------------------------------------------------------------------------------------------
            actual.tipo = hormiga_tipo_tablero;
            /// --------------------------------------------------------------------------------------------------

            hormigas_temporal[{x,y}] = actual;
        }
    }

}

void actionHandler(string action){

    if (action == "Evolucion Automatica" || action == "Siguiente Evolucion") {
        // En caso de que sea seleccionada la evolución automática, cambiamos la bandera referente
        if (action == "Evolucion Automatica") bandera_automatico = true;
        else bandera_automatico = false;

        // En caso de que se haya presionado algun boton referente al estado del juego, hay que mover todas las hormigas 
        // que estan dentro del arreglo temporal al arreglo general, significando que los cambios en las hormigas son aplicados

        for (auto i : hormigas_temporal) hormigas[i.first] = i.second;
        hormigas_temporal.clear();

        nextState();
        // Aumentamos el numero total de iteraciones

        total_iteraciones++;
        total_celdas_vivas = hormigas.size();

        valores_grafica_densidad.PB(hormigas.size());

        /*
        int total_hormigas_vivas = hormigas.size();

        // Comprobamos si es que realizamos una division entre 0
        // Si no es el caso realizamos la division normalmente
        if (total_hormigas_vivas != 0)
            valores_grafica_densidad.PB(total_celdas_vivas / total_hormigas_vivas);
        else // En caso contrario, ponemos un valor igual a 0
            valores_grafica_densidad.PB(0);
        */
    }

    // Detenemos la evolución automatica si es el caso
    if (action == "Detener") bandera_automatico = false;

    // Cambiamos la seleccion del tipo de hormiga que se agrega a la simulacion
    if (action == "Reina") hormiga_tipo_tablero = 0;
    if (action == "Trabajadora") hormiga_tipo_tablero = 1;
    if (action == "Reproductora") hormiga_tipo_tablero = 2;
    if (action == "Soldado") hormiga_tipo_tablero = 3;

    // Aumentamos o disminuimos la cantidad de zoom dependiendo del usuario
    if (action == "+" || action == "-") {
        int delta = (action == "+" ? 1 : -1);
        index_zoom += (index_zoom + delta < zoom.size() && index_zoom + delta >= 0 ? delta : 0);
    }

    // Cambiamos la condicion de frontera 
    if (action == "Toro" || action == "Nulo") bandera_nulo = (action == "Nulo") ? true : false;

    if (action == ">" || action == "<" || action == "v" || action == "^"){
        // Dependiendo de la acción presionada, modificamos el valore del scroll
        if (action == ">" && index_visual_x + valor_scroll < n) index_visual_x += valor_scroll;
        else if (action == "<" && index_visual_x - valor_scroll >= 0) index_visual_x -= valor_scroll;
        
        else if (action == "v" && index_visual_y + valor_scroll < n) index_visual_y += valor_scroll;
        else if (action == "^" && index_visual_y - valor_scroll >= 0) index_visual_y -= valor_scroll;
    }

    if (action == "Limpiar Juego"){
        // Reinicmaos todos los valores para el inicio de un nuevo juego
        total_celdas_vivas = hormigas.size();
        total_iteraciones = 0;

        cambios_matrix.clear();
        matrix = matrix_clean;

        celdas_vivas.clear();
        hormigas_temporal.clear();
        hormigas.clear();
        borrar.clear();
        nacimiento_condicion.clear();

        valores_grafica_densidad.clear();
        valores_grafica_entriopia.clear();
        entropy.clear();

        index_visual_x = 0;
        index_visual_y = 0;
        valor_scroll = 1;

        bandera_automatico = false;
    }

    if (action == "Inicializar Juego"){
        actionHandler("Limpiar Juego");

        int index = 3;

        while(index>= 0){
            int cantidad_hormigas_tipo = (distribucion_n * distribucion_n * densidad_hormigas[index]) / 100;
            while(cantidad_hormigas_tipo--){
                int x, y;

                while(true){
                    x = getIndices();                  
                    y = getIndices();
                    if (hormigas.find({x,y}) == hormigas.end()) break;
                }

                Ant hormiga_actual;
                hormiga_actual.edad = 0;
                hormiga_actual.direccion = 1;
                hormiga_actual.tipo = index;

                hormigas[{x,y}] = hormiga_actual;
            }

            index--;
        }
    }

    return;
}

void abrirArchivo(GtkDialog *dialog, gint response_id, gpointer user_data) {
    // Si la respuesat fue "Aceptar", abrimos el archivo
    if (response_id == GTK_RESPONSE_ACCEPT) {
        string ruta_archivo = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        ifstream file(ruta_archivo);

        if (file.is_open()) {
            actionHandler("Limpiar Juego");
            string line;
            getline(file, line);

            int nuevo_valor_n = stoi(line);

            bool bandera = true;
            // Leemos todo el contenido del archivo de texto
            while (getline(file, line)) {
                int contador = 0, x, y;
                string temp = "";
                Ant actual;

                if (line == "---") bandera = false;
                else {
                    for (int i = 0; i < line.size(); i++){
                        if (line[i] == ' '){
                            int valor = stoi(temp);
                            if (contador == 0) x = valor;
                            else if (contador == 1) y = valor;
                            else if (contador == 2) actual.edad = valor;
                            else if (contador == 3) actual.direccion = valor;
                            else actual.tipo = valor;
                            contador++;
                            temp = "";
                        }
                        else temp += line[i];
                    }
                }

                // Si la bandera es positiva, significa que sabemos los valores de las hormigas
                if (bandera) hormigas[{x,y}] = actual;
                else celdas_vivas.insert({x, y});

            }



            // Cerramos el proceso del archivo
            file.close();
            
          
        }
    }

    // Una vez terminado el proceso, se puede quitar la ventana para la seleccion del archivo
    gtk_widget_destroy(GTK_WIDGET(dialog));

    // Terminamos el loop principal para el manejo de archivos
    gtk_main_quit(); 
    return;
}

void guardarArchivo(GtkDialog *dialog, gint response_id, gpointer user_data) {

    if (response_id == GTK_RESPONSE_ACCEPT) {
        // Obtenemos la ruta del archivo seleccionado
        string ruta_archivo = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        string numero = to_string(n);

        // Abrimos el archivo para posteriormente escribir en el
        ofstream file(ruta_archivo);
        if (!file.is_open()) {
            // En caso de que se tenga un error al momento de querer crear el archivo, cerramos la ventana para el manejor del archivo y 
            // acabamos el proceso
            cerr << "Failed to open file for writing: " << ruta_archivo << endl;
            gtk_widget_destroy(GTK_WIDGET(dialog));
            gtk_main_quit();
            return;
        }

        // Escribimos el numero de la dimension de la matrix cuadrada
        file << numero << endl;

        // Aplicamos cualquier cambio que este pendiente en la simulacion
        for (auto i : hormigas_temporal) hormigas[i.first] = i.second;

        // Escribimos las posiciones de las hormigas junto con los parametros de cada hormiga
        for (auto i : hormigas){
            string hormiga_actual = to_string(i.first.first) + " " + to_string(i.first.second) + " ";
            hormiga_actual += to_string(i.second.edad) + " " + to_string(i.second.direccion) + " " + to_string(i.second.tipo) + " ";
            file << hormiga_actual << endl;
        }

        // Escribimos dentro del archivo un separador para saber cuando han acabado las hormigas
        file << "---" << endl;

        // Despues escribimos las posicones de las celdas vivas dentro de la matrix
        for (auto i : celdas_vivas){
            string coordenadas = to_string(i.first) + " " + to_string(i.second) + " ";
            file << coordenadas << endl;
        }


        // Cerramos el proceso del archivo
        file.close();
    }

    // Una vez terminado el proceso, se puede quitar la ventana para la seleccion del archivo
    gtk_widget_destroy(GTK_WIDGET(dialog));

    // Terminamos el loop principal para el manejo de archivos
    gtk_main_quit();
    return;
}

void archivoHandler(string action){

    if (action == "Abrir"){
        // Inicializamos el preoceso para las ventas 
        gtk_init(NULL, NULL);
        // Creamos la ventana para la seleccion del archivo
        GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

        // Agregamos un filtro al momento de mostrar los archivos de manera que solamente se muestren los que tengan extension ".txt"
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_add_pattern(filter, "*.txt");
        gtk_file_filter_set_name(filter, "Text files");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

        // Mostramos la ventana para poder guardar los archivos
        gtk_widget_show_all(dialog);
        // Conectamos la respuesta del usuario con una function
        g_signal_connect(dialog, "response", G_CALLBACK(abrirArchivo), NULL);

        // Iniciamos el loop principal para el manejo de archivos
        gtk_main();

    }
    else{
        // Inicializamos el preoceso para las ventas 
        gtk_init(NULL, NULL);

        // Creamos la ventana para guardar el archivo
        GtkWidget *dialog = gtk_file_chooser_dialog_new("Save As", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "_Cancel", GTK_RESPONSE_CANCEL, "_Save", GTK_RESPONSE_ACCEPT, NULL);
        // Mostramos la ventana para poder guardar los archivos
        gtk_widget_show_all(dialog);
        // Conectamos la respuesta del usuario con una function
        g_signal_connect(dialog, "response", G_CALLBACK(guardarArchivo), NULL);

        // Iniciamos el loop principal para el manejo de archivos
        gtk_main();
    }
    
    return;
}

void showGraphs(){

    // Cremos el archivo para guardar los datos de la densidad poblacional
    ofstream file("normal.txt");

    // Ingresamos los datos al archivo
    for (int i = 0; i < valores_grafica_densidad.size(); i++)
        file << valores_grafica_densidad[i] << endl;

    // Cerramos el archivo
    file.close();


    // Ejecutamos el programa para la visualización de las gráficas y limpiamos los archivos creados
    int a = system("python3 graphs.py");
    //int b = system("rm normal.txt");

    return;    
}


int main() {

    // x, y

    color_hormigas[0] = {182,0,0};
    color_hormigas[1] = {182,96,255};
    color_hormigas[2] = {0,182,255};
    color_hormigas[3] = {0,255,0};
    densidad_hormigas = {6, 22, 50, 22};



    // Creamos la ventana principal en la cual tendra todos los botones y la ventana del juego
    sf::RenderWindow outerWindow(sf::VideoMode(1650, 880), "Langton's Ant");

    // Le asignamos las dimensiones a la pantalla del juego
    inner.create(1400, 700);

    // Usamos la clase Sprite para poder dibujar todas las celdas dentro de la pantalla del juego
    sf::Sprite innerSprite(inner.getTexture());

    // Calculamos la posicion para poder colocar correctamente la pantalla del juego
    int distancia_X = (outerWindow.getSize().x - inner.getSize().x)  - 20;
    int distancia_Y = ((outerWindow.getSize().y - inner.getSize().y) / 2);

    // Posicionamos la pantalla del juego
    innerSprite.setPosition(sf::Vector2f(distancia_X, distancia_Y));

    // Asignamos el color de fondo o de las celdas muertas 
    inner.clear(sf::Color(color_fondo[0], color_fondo[1], color_fondo[2]));


// BOTONES  ------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Asignamos la fuente que vamos a ocupar en el programa
    font.loadFromFile("./Fonts/arial.ttf");
    
    // Creamos todos los diferentes botones y los ponemos dentro de un vector para facilitar su dibujo en pantalla y el evento de click
    vector<std::pair<sf::RectangleShape, sf::Text>> buttons = {
        createRectangle(180,60, 20, 30, "Evolucion Automatica", 17, 6, 17),
        createRectangle(180,60, 20, 120, "Detener", 17, 55, 17),
        createRectangle(180,60, 20, 210, "Siguiente Evolucion", 17, 15, 17),
        createRectangle(60, 50, 30, 300, "-", 25, 26, 8),
        createRectangle(60, 50, 120, 300, "+", 25, 24, 10),

        createRectangle(180, 50, 1200, 20, "Inicializar Juego", 17, 20, 14),
        createRectangle(180, 50, 1450, 20, "Limpiar Juego", 17, 36, 14),

        createRectangle(60, 30, 300, 30, "Toro", 20, 6, 2),
        createRectangle(60, 30, 370, 30, "Nulo", 20, 6, 2),

        createRectangle(50, 45, 80, 400, "^", 32, 16, 5),
        createRectangle(50, 45, 80, 450, "v", 24, 18, 5),
        createRectangle(50, 40, 20, 425, "<", 24, 17, 5),
        createRectangle(50, 40, 140, 425, ">", 24, 17, 5),

        createRectangle(180, 60, 20, 710, "Configuracion", 18, 30, 18),

        createRectangle(81, 50, 20, 800, "Guardar", 14, 15, 17),
        createRectangle(81, 50, 111, 800, "Abrir", 14, 25, 17),

        createRectangle(180, 50, 1450, 810, "Mostrar Graficas", 17, 18, 14),

        createRectangle(80, 50, 850, 810, "Reina", 16, 15, 17),
        createRectangle(120, 50, 950, 810, "Trabajadora", 16, 13, 17),
        createRectangle(120, 50, 1090, 810, "Reproductora", 16, 12, 17),
        createRectangle(90, 50, 1230, 810, "Soldado", 16, 15, 17),

    };

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Creamos un arreglo para poder tener las etiquetas necesarias en un solo lugar
    vector <std::pair<sf::RectangleShape, sf::Text>> etiquetas = {
        createRectangle(200, 50, 250, 810, "Iteracion : " + to_string(total_iteraciones), 20, 5, 13),
        createRectangle(200, 50, 500, 810, "Celdas vivas : " + to_string(total_celdas_vivas), 18, 5, 13)
    };
    

    // Bucle que ocupamos para la pantalla mientras ésta esté presente
    while (outerWindow.isOpen()) {
        // Declaramos el manejador de eventos
        sf::Event event;
        
        // Limpiamos la pantalla principal y le colocamos un color de fondo
        outerWindow.clear(sf::Color(51,65,78));

        // Checamos si es que en algun momento se tuvo algun evento
        while (outerWindow.pollEvent(event)) {
            // En caso de que el evento sea cerrar la ventana, cerramos completamente toda la ventana
            if (event.type == sf::Event::Closed)  outerWindow.close();

            // En caso de que el evento sea en donde se presiona el boton de el mouse, checamos que sea el izquierdo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                // Obtenemos la posicion del mouse
                sf::Vector2i mousePos = sf::Mouse::getPosition(outerWindow);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                // De todos los botones presionados checamos cual fue presionado
                for (int i = 0; i < buttons.size(); i++){
                    if (buttons[i].first.getGlobalBounds().contains(mousePosF)) {
                            // Si el boton fue presionado, lo mandamos a nuestra funcion de actionHandler
                            string action = buttons[i].second.getString();
                            if (action == "Configuracion" ){
                                outerWindow.setVisible(false);
                                updateConfiguration();
                                outerWindow.setVisible(true);
                            }
                            else if (action == "Guardar" || action == "Abrir"){
                                outerWindow.setVisible(false);
                                archivoHandler(action);
                                outerWindow.setVisible(true);
                            }
                            else if (action == "Mostrar Graficas"){
                                outerWindow.setVisible(false);
                                showGraphs();
                                outerWindow.setVisible(true);
                            }
                            else actionHandler(action);

                            // Seleccionar color
                    }
                }


                // Dentro de la sigueinte condicional, checamos si es que se presiono alguna celda dentro del tablero
                // de esa manera saber que celda debemos cambiar su estado
                if (innerSprite.getGlobalBounds().contains(mousePosF) && !bandera_automatico) {
                    // Primero debemos encotnrar las coordenadas de la celda presionada
                    // 230 -> La diferencia entre la ventana de la simulacion y la interfaz grafica
                    // 90 -> La diferencia entre la ventana de la simulacion y la interfaz grafica entre 2
                    // Esto para llevar la posicion del mouse a la region a la venta de la simulacion
                    // Para luego dividir entre el zoom de la simulacion para saber cuantas celdas son visibles 
                    // para despues movernos la cantidad de celdas necesarias dependiendo del scroll

                    int x = (((int)mousePosF.x - 230) / zoom[index_zoom]) + index_visual_x ;
                    int y = (((int)mousePosF.y - 90) / zoom[index_zoom]) + index_visual_y;

                    boardHandler(x, y);
                }
            }
        
        }

        // Cambiamos el color del boton de 'Evolucion Automatica' en caso de que la opcion sea seleccionada
        // para que se conozaca de mejor manera que la opcion fue seleccionada
        // sabiendo que el boton se encuentra en la primer posicion del arreglo de los botones
        if (bandera_automatico){
            actionHandler("Evolucion Automatica");
            buttons[0].first.setFillColor(sf::Color(96, 96, 96));
        }
        else{
            buttons[0].first.setFillColor(sf::Color(200, 200, 200));
        }

        // Sabiendo de los indices de los botones para los diferentes tipos de hormiga, mostramos de manera
        // mas clara que hormiga es seleccionada al momento de ponerla dentro de la simulacion
        for (int i = buttons.size() - 4; i < buttons.size(); i++)
            buttons[i].first.setFillColor(sf::Color(200,200,200,200));

        buttons[buttons.size() - 4 + hormiga_tipo_tablero].first.setFillColor(sf::Color(96, 96, 96));


        updateGraphics();

        // Funcion para la actualizacion de los elementos graficos de la sumulacion 
        // updateGameVisual();

        // Primero mostramos la pantalla del juego para que no se tenga algun efecto de parpadeo
        inner.display();

        // Colocamos en la ventana principal la ventana de la simulacion
        outerWindow.draw(innerSprite);
        
        // -------------------------------------------------------------------------------------------------------------------------
        // Colocamos los diferentes botones dentro de la interfaz grafica
        for (auto& button : buttons) {
            outerWindow.draw(button.first);

            if (button.second.getString() == "Toro"){
                if (bandera_nulo) button.first.setFillColor(sf::Color(200, 200, 200));
                else button.first.setFillColor(sf::Color(96, 96, 96));
            }

            if (button.second.getString() == "Nulo"){
                if (!bandera_nulo) button.first.setFillColor(sf::Color(200, 200, 200));
                else button.first.setFillColor(sf::Color(96, 96, 96));
            }

            outerWindow.draw(button.second);
        }
        // -------------------------------------------------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------------------------------------------------
        // Colocamos las diferentes etiquetas dentro de la interfaz grafica
        for(int i = 0; i < etiquetas.size(); i++){
            char etiqueta_char_inicial = etiquetas[i].second.getString()[0];
            
            // Hacemos una actualizacion en la informacion mostrada de las etiquetas dependiendo de la etiqueta
            if (etiqueta_char_inicial == 'I') etiquetas[i].second.setString("Iteracion : " + to_string(total_iteraciones));
            else if (etiqueta_char_inicial == 'C') etiquetas[i].second.setString("Celdas vivas : " + to_string(total_celdas_vivas));

            // Mostramos la etiqueta junto con el recuadro en la interfaz
            outerWindow.draw(etiquetas[i].first);
            outerWindow.draw(etiquetas[i].second);
        }
        // -------------------------------------------------------------------------------------------------------------------------

        // Mostramos la pantalla principal con todos los elementos que colocamos anteriormente
        outerWindow.display();
    }

    return 0;
}
