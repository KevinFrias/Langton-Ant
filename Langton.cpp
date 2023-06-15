#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>
#include <gtk/gtk.h>
#include <semaphore>
#include <chrono>
#define PB push_back
using namespace std;

// Declaracion del size total de programa junto con el size de cada celda
int n = 1400;
int n_archivo = 0;
int sizeCelda_X, sizeCelda_Y;

// Matrices necesarias para los diferentes estados del juego
vector < vector <bool>> matrix(n, vector(n, false));
vector < vector <bool>> matrix_clean(n, vector(n, false));
vector < vector <bool>> matrix_next_gen(n, vector(n, false));

// Arreglos para manetener un control de todos los valores que se usan en las graficas
vector <int> valores_grafica_normal;
vector <int> valores_grafica_entriopia;
unordered_map <int,int> entropy;

// Arreglos para las celdas que se encuentran vivas
vector <list <int>> live_cells(n);
vector <list <int>> live_cells_clean(n);

// Conjunto para tener control de las reglas del juego
set <int> regla_nacimineto;
set <int> regla_sobrevivir;

// Definimos RGB de los diferentes colores que se ocupan dentro del programa
int color_muerto[] = {0,0,0};
int color_vivo[] = {255,255,255};

// Declaracion de variables para tener un control del scroll que se aplica
int index_visual_x = 0;
int index_visual_y = 0;
int valor_scroll = 1;

// Variables necesarias para mostrar en pantalla su valor
int total_iteraciones = 0;
int total_celdas_vivas = 0;
int total_celdas_vivas_entriopia = 0;


// varibles necesarias para el control del zoom
int index_zoom = 13;
vector <int> zoom = {/*1, 2,*/ 4, 5, 7, 10, 14, 20, 25, 28, 35, 50, 70, 100, 140, 175, 350, 700};


// Definimos banderas que nos ayudan a mantener el control de acciones especificas del programa
bool bandera_automatico = false;
bool bandera_nulo = true;

// Definimos la fuente que vamos a ocupar dentro de la ventana
sf::Font font;
sf::RenderTexture inner;

// Declaramos un mutex para bloquear el acceso de los diferentes hilos a la matrix
mutex mtx;

// Size del margen en pixeles
const float margin = 10.f;

// Cadena para poder escribir y leer de un archivo de texto
string valores_archivo;


pair<sf::RectangleShape, sf::Text> createButton(int szBtnX, int szBtny, int posX, int posY, string texto, int szTexto, int posTX, int posTY){
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

void handleNextStep(int x1, int x2, int y1, int y2, int iteracion){
    // Reiniciamos la cantidad total de celdas
    total_celdas_vivas = 0;

    // Hacemos un recorrido por toda la matriz definida
    for (int i = x1; i <= x2; i++){
        for (int j = y1; j <= y2; j++) {
            // Declaramos las variables necesarias para el conteo total de la poblacion para 
            // aplicar las reglas necesarias y los valores necesarios para la entropia
            int poblacion = 0;
            int poblacion2 = 0;
            int temp = 0;

            // Checamos si es que se aplica condición de frontera y hacemos los calculos necesarios
            if (bandera_nulo){
                // Lado izquierdo
                if (i - 1 >= 0){
                    if (j - 1 >= 0) {
                        temp = (int)(matrix[j - 1][i - 1]);
                        poblacion += temp;
                        poblacion2 |= (temp << 8);
                    }
                    
                    temp = (int)(matrix[j][i - 1]);
                    poblacion += temp;
                    poblacion2 |= (temp << 7);

                    if (j + 1 < n){ 
                        temp = (int)matrix[j + 1][i - 1];
                        poblacion += temp;
                        poblacion2 |= (temp << 6);
                    }
                }

                // Lado central
                if (j - 1 >= 0){
                    temp = (int)matrix[j-1][i];
                    poblacion += temp;
                    poblacion2 |= (temp << 5);
                }

                poblacion2 |= ((int)matrix[j][i] << 4);

                if (j + 1 < n){
                    temp = (int)matrix[j+1][i];
                    poblacion += temp;
                    poblacion2 |= (temp << 3);
                }

                // Lado Derecho
                if (i + 1 < n){
                    if (j - 1 >= 0){
                        temp = matrix[j - 1][i+1];
                        poblacion += temp;
                        poblacion2 |= (temp << 2);
                    }

                    temp = matrix[j][i+1];
                    poblacion += temp;
                    poblacion2 |= (temp << 1);

                    if (j + 1 < n) {
                        temp = matrix[j+1][i+1];
                        poblacion += temp;
						poblacion2 |= (temp << 0);
                    }
                }


            }
            else{
                int indexIzquierda = (i - 1 < 0 ? n - 1 : i - 1);
                int indexDerecha = (i + 1 >= n ? 0 : i + 1);
                
                int indexArriba = (j - 1 < 0 ? n - 1 : j - 1);
                int indexAbajo = (j + 1 >= n ? 0 : j + 1);

                // Lado inzquierdo
                poblacion += matrix[indexArriba][indexIzquierda];
                poblacion += matrix[j][indexIzquierda];
                poblacion += matrix[indexAbajo][indexIzquierda];
                poblacion2 |= (matrix[indexArriba][indexIzquierda] << 8);
                poblacion2 |= (matrix[j][indexIzquierda] << 7);
                poblacion2 |= (matrix[indexAbajo][indexIzquierda] << 6);

                // Lado central
                poblacion += matrix[indexArriba][i];
                poblacion += matrix[indexAbajo][i];
                poblacion2 |= (matrix[indexArriba][i] << 5);
                poblacion2 |= (matrix[j][i] << 4);
                poblacion2 |= (matrix[indexAbajo][i] << 3);

                // Lado Derechp=o
                poblacion += matrix[indexArriba][indexDerecha];
                poblacion += matrix[j][indexDerecha];
                poblacion += matrix[indexAbajo][indexDerecha];
                poblacion2 |= (matrix[indexArriba][indexDerecha] << 2);
                poblacion2 |= (matrix[j][indexDerecha] << 1);
                poblacion2 |= (matrix[indexAbajo][indexDerecha] << 0);

            }

            // Sobrevivir
            if (matrix[j][i]){
                // Hacemos un simple chequeo de que la cantidad de celdas de esa region sea parte de 
                // la regla para sobrevivir
                if (regla_sobrevivir.count(poblacion)){
                    // Agregamos la celda actual a la estructura encargada de dibujar las celdas vivas
                    live_cells[j].PB(i);
                    matrix_next_gen[j][i] = true;
                    total_celdas_vivas++;
                }
                else matrix_next_gen[j][i] = false;
            }   
            else { // Nacimineto
                // Al ser nacimiento checamos si es que la cantidad de vecinos es suficiente para que una célula nazca
                if (regla_nacimineto.count(poblacion)){
                    // Agregamos la celda actual a la estructura encargada de dibujar las celdas vivas
                    live_cells[j].PB(i);
                    // Aumentamos la cantidad de celdas vivas
                    total_celdas_vivas++;
                    matrix_next_gen[j][i] = true;
                }
            }
            // Agregamos la densidad poblacional a la estructura correspondiente
            entropy[poblacion2]++;
        }
    }

    // Agregamos la densidad poblacional a la estructura correspondiente
    valores_grafica_normal.PB(total_celdas_vivas);

    return;
}

void updateGameVisual(){
    // Limpiamos el tablero de juego con el color correspondiente
    inner.clear(sf::Color(color_muerto[0], color_muerto[1], color_muerto[2]));
    
    // Asignamos el tamaño correspondiente a cada celda dependiendo el zoom actual
    sizeCelda_X = zoom[index_zoom];
    sizeCelda_Y = zoom[index_zoom];

    // Hacemos los calculos necesarios para saber cuantas celdas son visibles con la cantidad de zoom actual
    int cantidad_x = 1400/sizeCelda_X;
    int cantidad_y = 700/sizeCelda_Y;

    // Creamos la figura base de cada celda viva
    sf :: RectangleShape celda(sf::Vector2f(sizeCelda_X, sizeCelda_Y));
    int index = index_visual_x;

    while(cantidad_x--){
        if (live_cells[index].size()) {
            // Si la celda está dentro de la cantidad visible, la dibujamos
            for (list<int>:: iterator it = live_cells[index].begin(); it != live_cells[index].end(); it++){
                celda.setPosition((index-index_visual_x)*sizeCelda_X, (*(it) - index_visual_y)*sizeCelda_Y);
                celda.setFillColor(sf::Color(color_vivo[0], color_vivo[1], color_vivo[2]));
                inner.draw(celda);
            }
        } 

        index++;
    }

    return;
}

void selectColor(int opcion){

    // Creamos la venta
    string titutlo = "";

    if (opcion == 1) titutlo = "vivas ";
    else titutlo = "muertas ";

    sf::RenderWindow windowSelectColor(sf::VideoMode(755, 400), "Selecciona un color para las celdas " + titutlo);
    sf::Color selectedColor = sf::Color::White;
    
    // Creamos la dimension que va a tener cada color muestra
    const int tileSize = 40;

    // Creamos toda la paleta de colores
    vector<sf::RectangleShape> palette;
    vector<sf::RectangleShape> palette_clean;
    sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));

    // Creamos las variables para cada color de la paleta
    int r, g, b, y = 20;

    for(int i = 0; i < 4; i++) {
        int x = 20;
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                tile.setPosition(x, y);

                // Ocupamos un bitset para la creación de los diferentes colores
                bitset<8> binary_num;
                
                binary_num = (i << 6 | j << 4 | k << 2);
                r = binary_num.to_ulong();

                binary_num = (i << 4 | j << 2 | k << 6);
                g = binary_num.to_ulong();


                binary_num = (i << 2 | j << 6 | k << 4);
                b = binary_num.to_ulong();

                x += 45;

                sf::Color color(r, g, b);

                tile.setFillColor(color);
                palette.push_back(tile);

            }

        }
        y += 45;
    } 

    palette_clean = palette;

    // Cremoas el color para confirmar la selección
    auto buttonOk = createButton(100, 50, 327, 300, "OK", 24, 32, 10);

    int pressed = -1;

    while (windowSelectColor.isOpen()) {
        sf::Event event;
        while (windowSelectColor.pollEvent(event)) {
            if (event.type == sf::Event::Closed) windowSelectColor.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                
                // Get the mouse position relative to the window
                sf::Vector2i mousePosition = sf::Mouse::getPosition(windowSelectColor);

                // Hacemos un recorrido por todos los colores disponibles y checamos si alguno de ellos fue presionado
                for (int i = 0; i < palette.size(); ++i){
                    if (palette[i].getGlobalBounds().contains(mousePosition.x, mousePosition.y)){
                        // En caso de que algun color sea presioado, ocupamos la pelata de colores sin ningun cambio y la asignamos a la que
                        // vamos a mostrar
                        palette = palette_clean;

                        // Asignamos el color que se selecciono en una variable
                        selectedColor = palette[i].getFillColor();

                        // Del color seleccionado le agregamos una ayuda visual para mostrar en que posicion se encuentra
                        palette[i].setSize(palette[i].getSize() - sf::Vector2f(margin * 2.f, margin * 2.f));
                        palette[i].setPosition(palette[i].getPosition() + sf::Vector2f(margin, margin));
                    }
                }


                // Una vez presionado el boton de OK, asignamos el color a la celda
                if (buttonOk.first.getGlobalBounds().contains(mousePosition.x, mousePosition.y)){
                    r = (int)selectedColor.r;
                    g = (int)selectedColor.g;
                    b = (int)selectedColor.b;

                    // Unicamente checamos si tenemos que asignar el color a la celda viva o muerta
                    if (opcion == 1){ 
                        color_vivo[0] = r;
                        color_vivo[1] = g;
                        color_vivo[2] = b;
                    }
                    else {
                        color_muerto[0] = r;
                        color_muerto[1] = g;
                        color_muerto[2] = b;
                    }
                    windowSelectColor.close();
                }
            }
        }

        // Colocamos todos los elementos de la pantalla y los mostramos

        windowSelectColor.clear(sf::Color::White);
        windowSelectColor.draw(buttonOk.first);
        windowSelectColor.draw(buttonOk.second);


        for (int i = 0; i < palette.size(); ++i) windowSelectColor.draw(palette[i]);

        windowSelectColor.display();
    }
}

void updateColors(){
    // Creamos la ventana donde manejaremos el cambio de color de las celdas
    sf::RenderWindow windowColor(sf::VideoMode(660, 250), "Cambio de color");

    auto buttonOk = createButton(100, 50, 280, 170, "OK", 24, 32, 10);

    sf::RectangleShape alive(sf::Vector2f(200, 80));
    sf::RectangleShape death(sf::Vector2f(200, 80));

    alive.setFillColor(sf::Color(color_vivo[0], color_vivo[1], color_vivo[2]));
    death.setFillColor(sf::Color(color_muerto[0], color_muerto[1], color_muerto[2]));

    alive.setPosition(65, 50);
    death.setPosition(395, 50);

    sf::Text text1("Celdas Vivas", font, 24);
    text1.setFillColor(sf::Color::Black);
    text1.setPosition(90, 15);

    sf::Text text2("Celdas Muertas", font, 24);
    text2.setFillColor(sf::Color::Black);
    text2.setPosition(405, 15);



    while (windowColor.isOpen()){
        sf::Event event;
        while (windowColor.pollEvent(event)){
            if (event.type == sf::Event::Closed) windowColor.close();

            // En caso de que el evento sea en donde se presiona el boton de el mouse, checamos que sea el izquierdo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                // Obtenemos la posicion del mouse
                sf::Vector2i mousePos = sf::Mouse::getPosition(windowColor);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                if (buttonOk.first.getGlobalBounds().contains(mousePosF)) windowColor.close();
                if (alive.getGlobalBounds().contains(mousePosF)){
                    // Cambiamos el color vivo
                    windowColor.setActive(false);
                    selectColor(1);
                    alive.setFillColor(sf::Color(color_vivo[0], color_vivo[1], color_vivo[2]));
                    windowColor.setActive(true);

                }

                if (death.getGlobalBounds().contains(mousePosF)){
                    // Cambiamos el color vivo
                    windowColor.setActive(false);
                    selectColor(2);
                    death.setFillColor(sf::Color(color_muerto[0], color_muerto[1], color_muerto[2]));
                    windowColor.setActive(true);
                }

            }
        }

        windowColor.clear(sf::Color(247,155,131));

        // Draw the preview rectangle and the palette to the window
        windowColor.draw(alive);
        windowColor.draw(death);

        windowColor.draw(text1);
        windowColor.draw(text2);

        windowColor.draw(buttonOk.first);
        windowColor.draw(buttonOk.second);

        windowColor.display();
    }

   return;
}

void nueva_regla(){
    sf::RenderWindow windowRegla(sf::VideoMode(500, 300), "Nueva regla");

    // Limpiamos la pantalla principal y le colocamos un color de fondo
    windowRegla.clear(sf::Color(92,117,140));

    // Creamos los botones para confirmar o cancelar la seleccion de una nueva regla
    auto botonOK = createButton(100, 60, 120, 200, "OK", 20, 30, 16);
    auto botonCancelar = createButton(100, 60, 280, 200, "Cancelar", 20, 8, 16);

    // Creamos el cuadrante para poder ingresar la regla
    sf::RectangleShape inputBox(sf::Vector2f(300, 50));
    inputBox.setFillColor(sf::Color::White);
    inputBox.setPosition(100, 100);

    sf::Text text("|", font, 24);
    text.setFillColor(sf::Color::Black);
    text.setPosition(104, 108);

    // Declaración de las variables para mostrar la entrada del usuario y la que mostramos
    // ocupando SlidingWindow
    string input;
    string mostrar;
    int index = 0;
    bool banderaOk = false;

    while (windowRegla.isOpen()){
        sf::Event event;
        windowRegla.clear(sf::Color(92,117,140));

        while (windowRegla.pollEvent(event)){
            if (event.type == sf::Event::Closed) windowRegla.close();

            else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b'){
                    // Hacemos el manejo de borrar el ultimo caracter
                    if (!input.empty()){
                        input.pop_back();
                        index = max(0, index - 1);;
                    }
                }
                // Si alguna tecla del alfabeto fue presionada
                else if (event.text.unicode < 128) {
                    // La agregamos a la cadena que vamos a mostrar
                    input += static_cast<char>(event.text.unicode);
                }

                // Se ocupa 21, ya que es la cantidad máxima de valores que se muestra en el input
                if (input.size() > 21){
                    int temp = input.size();
                    mostrar = input.substr(index, min(21, temp));
                }
                else mostrar = input;

                text.setString(mostrar);
            }

            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // Get the mouse position relative to the window
                sf::Vector2i mousePosition = sf::Mouse::getPosition(windowRegla);

                // Una vez presionado el boton de OK, asignamos el color a la celda
                if (botonOK.first.getGlobalBounds().contains(mousePosition.x, mousePosition.y)){
                    banderaOk = true;
                    windowRegla.close();
                }
                if (botonCancelar.first.getGlobalBounds().contains(mousePosition.x, mousePosition.y)){
                    windowRegla.close();
                }
            }
        }

            windowRegla.draw(inputBox);
            windowRegla.draw(text);
            windowRegla.draw(botonOK.first);
            windowRegla.draw(botonOK.second);

            windowRegla.draw(botonCancelar.first);
            windowRegla.draw(botonCancelar.second);

            windowRegla.display();
    }


    if (input.size() > 3 && banderaOk){
        // Limpiamos los conjuntos para poder agregar los valores de la nueva regla
        regla_nacimineto.clear();
        regla_sobrevivir.clear();
        
        string survivial = "";
        string birth = "";
        bool bandera = false;

        // Dividimos la entrada para tener un control más fácil
        for (int i = 0; i < input.size(); i++){
            if (input[i] == '/') bandera = true;

            if (bandera) survivial += input[i];
            else birth += input[i];
        }

        // Agregamos en la regla de nacimiento en caso de que sea un numero
        for (int i = 0; i < birth.size(); i++)
            if (birth[i] >= '0' && birth[i] <= '9') regla_nacimineto.insert(birth[i]-'0');
        
        // Agregamos en la regla de sobrevivencia en caso de que sea un numero
        for (int i = 0; i < survivial.size(); i++)
            if (survivial[i] >= '0' && survivial[i] <= '9') regla_sobrevivir.insert(survivial[i]-'0');
    }

    return;
}

void updateValores(){
    // Reiniciamos todos los valores 
    index_visual_x = 0;
    index_visual_y = 0;
    valor_scroll = 1;
    total_celdas_vivas = 0;
    total_iteraciones = 0;

    // Limpiamos las estructuras donde se encuentra el juego y las celdas vivas
    matrix.assign(matrix_clean.begin(), matrix_clean.end());
    live_cells.assign(live_cells_clean.begin(), live_cells_clean.end());

    // Reiniciamos los valores que se ocupan al momento de visualizar las gráficas
    valores_grafica_entriopia.clear();
    valores_grafica_normal.clear();
    entropy.clear();

    // En cada archivo se guarda el tamaño del tablero de juego en caso de que se hayan
    //configurado de manera diferente
    int size_archivo = valores_archivo.size();
    int index = 0;

    string numero = "";

    while(index < size_archivo && valores_archivo[index] != '\n'){
        numero += valores_archivo[index];
        index++;
    }

    // Obtenemos el tamaño del tablero de juego
    n_archivo = stoi(numero);

    // Recorremos todo el tablero de juego del archivo para crear el nuevo
    for (int i = 0; i < min(n_archivo, n); i++){
        for (int j = 0; j < min(n, n_archivo); j++){
            if (valores_archivo[index++] == '1'){
                matrix[i][j] = true;
                live_cells[i].PB(j);
            }
        }
    }


    // Calculamos la entropía del nuevo tablero de juego
    for (int j = 0; j < n ; j++){
            for (int i = 0; i < n; i++){
                int poblacion = 0;
                if (bandera_nulo){
					// Lado izquierdo
					if (i - 1 >= 0){
						if (j - 1 >= 0) poblacion |= ((int)matrix[j-1][i-1] << 8);
                        poblacion |= ((int)matrix[j][i-1] << 7);
						if (j + 1 < n) poblacion |= ((int)matrix[j+1][i-1] << 6);
					}
					// Lado central
					if (j - 1 >= 0) poblacion |= ((int)matrix[j-1][i] << 5);
                    poblacion |= ((int)matrix[j][i] << 4);
					if (j + 1 < n) poblacion |= ((int)matrix[j+1][i] << 3);

					// Lado Derecho
					if (i + 1 < n){
						if (j - 1 >= 0) poblacion |= ((int)matrix[j-1][i+1] << 2);
                        poblacion |= ((int)matrix[j][i+1] << 1);
						if (j + 1 < n) poblacion |= ((int)matrix[j+1][i+1] << 0);
					}
                }
				else{
                    int indexIzquierda = (i - 1 < 0 ? n - 1 : i - 1);
                    int indexDerecha = (i + 1 >= n ? 0 : i + 1);
                    
					int indexArriba = (j - 1 < 0 ? n - 1 : j - 1);
                    int indexAbajo = (j + 1 >= n ? 0 : j + 1);

					// Lado inzquierdo
					poblacion |= (matrix[indexArriba][indexIzquierda] << 8);
					poblacion |= (matrix[j][indexIzquierda] << 7);
					poblacion |= (matrix[indexAbajo][indexIzquierda] << 6);
					// Lado cetral
					poblacion |= (matrix[indexArriba][i] << 5);
					poblacion |= (matrix[j][i] << 4);
					poblacion |= (matrix[indexAbajo][i] << 3);
					// Lado Derecho
					poblacion |= (matrix[indexArriba][indexDerecha] << 2);
					poblacion |= (matrix[j][indexDerecha] << 1);
					poblacion |= (matrix[indexAbajo][indexDerecha] << 0);

				}
                entropy[poblacion]++;
            }
        }

    double entropia_valor = 0.0;
    double total_celdas = (double)(n*n);

    for (auto it : entropy){
        double p = (double) it.second / (total_celdas);
        entropia_valor -= (p * log2(p));
    }

    valores_grafica_entriopia.PB(entropia_valor);
    entropy.clear();

    return;
}

void abrirArchivo(GtkDialog *dialog, gint response_id, gpointer user_data) {
    // Si la respuesat fue "Aceptar", abrimos el archivo
    if (response_id == GTK_RESPONSE_ACCEPT) {
        string ruta_archivo = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        ifstream file(ruta_archivo);

        if (file.is_open()) {
            // Leemos todo el contenido del archivo de texto
            string file_contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            valores_archivo = file_contents;

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
        string contenido_archivo = "";

        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                contenido_archivo += (matrix[i][j] ? "1" : "0");
            }
        }

         // Open the file for writing
        ofstream file(ruta_archivo);
        if (!file.is_open()) {
            // En caso de que se tenga un error al momento de querer crear el archivo, cerramos la ventana para el manejor del archivo y 
            // acabamos el proceso
            cerr << "Failed to open file for writing: " << ruta_archivo << endl;
            gtk_widget_destroy(GTK_WIDGET(dialog));
            gtk_main_quit();
            return;
        }

        file << numero << endl;
        // Escribimos los datos de la matrix a el archivo
        file << contenido_archivo;
        // Cerramos el proceso del archivo
        file.close();
    }

    // Una vez terminado el proceso, se puede quitar la ventana para la seleccion del archivo
    gtk_widget_destroy(GTK_WIDGET(dialog));

    // Terminamos el loop principal para el manejo de archivos
    gtk_main_quit();
    return;
}

void handlerArchivo (string action){

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
    for (int i = 0; i < valores_grafica_normal.size(); i++)
        file << valores_grafica_normal[i] << endl;

    // Cerramos el archivo
    file.close();


    // Cremos el archivo para guardar los datos de la densidad poblacional
    ofstream file2("entriopia.txt");

    // Ingresamos los datos al archivo
    for (int i = 0; i < valores_grafica_entriopia.size(); i++)
        file2 << valores_grafica_entriopia[i] << endl;

    // Cerramos el archivo
    file2.close();

    // Ejecutamos el programa para la visualización de las gráficas y limpiamos los archivos creados
    int a = system("python3 graphs.py");
    int b = system("rm normal.txt");
    int c = system("rm entriopia.txt");

    return;    

}

void actionHandler(string action){

    if (action == "Evolucion Automatica" || action == "Siguiente Evolucion") {

        // En caso de que sea seleccionada la evolución automática, cambiamos la bandera referente
        if (action == "Evolucion Automatica") bandera_automatico = true;
        else bandera_automatico = false;

        // Aumentamos el numero total de iteraciones
        total_iteraciones++;

        // Reiniciamos las celdas vivas para la nueva generacion
        live_cells.assign(live_cells_clean.begin(), live_cells_clean.end());

        // Relizamos la siguiente evolución
        handleNextStep(0, n-1, 0, n-1, 0); 

        // Limpiamos el tablero de la nueva generacion y asignamos la siguiente evolución al
        // tablero base
        matrix.assign(matrix_next_gen.begin(), matrix_next_gen.end());
        matrix_next_gen.assign(matrix_clean.begin(), matrix_clean.end());

/////////////////////////////////////////////////////////////////////////////
        // Calculamos la entropia de la nueva evolución
        double entropia_valor = 0.0;
        double total_celdas = (double)(n*n);

        for (auto it : entropy){
            double p = (double) it.second / (total_celdas);
            entropia_valor -= (p * log2(p));
        }

        valores_grafica_entriopia.PB(entropia_valor);
/////////////////////////////////////////////////////////////////////////////

        entropy.clear();
    }

    // Detenemos la evolución automatica si es el caso
    if (action == "Detener") bandera_automatico = false;

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
        total_celdas_vivas = 0;
        total_iteraciones = 0;

        matrix = matrix_clean;
        entropy.clear();
        live_cells = live_cells_clean;
        valores_grafica_entriopia.clear();
        valores_grafica_normal.clear();

        updateGameVisual();
    }

    if (action == "Inicializar Juego"){ 
        // Reiniciamos todos los valores para la creación de un nuevo juego 
        index_visual_x = 0;
        index_visual_y = 0;
        valor_scroll = 1;
        total_celdas_vivas = 0;
        total_iteraciones = 0;

        matrix.assign(matrix_clean.begin(), matrix_clean.end());
        live_cells.assign(live_cells_clean.begin(), live_cells_clean.end());


        valores_grafica_entriopia.clear();
        valores_grafica_normal.clear();
        entropy.clear();

        // Pondremos los valores aleatoriamente con la ayuda del tiempo actual
        srand(time(NULL));
    
        // Llenamos la matriz con los valores aleatorios
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (!(rand() & 1)){
                    matrix[j][i] = true;
                    live_cells[j].PB(i);
                    total_celdas_vivas++;
                }
            }
        }

        for (int j = 0; j < n ; j++){
            for (int i = 0; i < n; i++){
                int poblacion = 0;
                if (bandera_nulo){
					// Lado izquierdo
					if (i - 1 >= 0){
						if (j - 1 >= 0) poblacion |= ((int)matrix[j-1][i-1] << 8);
                        poblacion |= ((int)matrix[j][i-1] << 7);
						if (j + 1 < n) poblacion |= ((int)matrix[j+1][i-1] << 6);
					}
					// Lado central
					if (j - 1 >= 0) poblacion |= ((int)matrix[j-1][i] << 5);
                    poblacion |= ((int)matrix[j][i] << 4);
					if (j + 1 < n) poblacion |= ((int)matrix[j+1][i] << 3);

					// Lado Derecho
					if (i + 1 < n){
						if (j - 1 >= 0) poblacion |= ((int)matrix[j-1][i+1] << 2);
                        poblacion |= ((int)matrix[j][i+1] << 1);
						if (j + 1 < n) poblacion |= ((int)matrix[j+1][i+1] << 0);
					}
                }
				else{
                    int indexIzquierda = (i - 1 < 0 ? n - 1 : i - 1);
                    int indexDerecha = (i + 1 >= n ? 0 : i + 1);
                    
					int indexArriba = (j - 1 < 0 ? n - 1 : j - 1);
                    int indexAbajo = (j + 1 >= n ? 0 : j + 1);

					// Lado inzquierdo
					poblacion |= (matrix[indexArriba][indexIzquierda] << 8);
					poblacion |= (matrix[j][indexIzquierda] << 7);
					poblacion |= (matrix[indexAbajo][indexIzquierda] << 6);
					// Lado cetral
					poblacion |= (matrix[indexArriba][i] << 5);
					poblacion |= (matrix[j][i] << 4);
					poblacion |= (matrix[indexAbajo][i] << 3);
					// Lado Derecho
					poblacion |= (matrix[indexArriba][indexDerecha] << 2);
					poblacion |= (matrix[j][indexDerecha] << 1);
					poblacion |= (matrix[indexAbajo][indexDerecha] << 0);

				}
                entropy[poblacion]++;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Calulamos la entropia del nuevo juego creado
        double entropia_valor = 0.0;
        double total_celdas = (double)(n*n);

        for (auto it : entropy){
            double p = (double) it.second / (total_celdas);
            entropia_valor -= (p * log2(p));
        }

        valores_grafica_entriopia.PB(entropia_valor);

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        entropy.clear();
    }

    return;
}




int main() {

    // Creamos la ventana principal en la cual tendra todos los botones y la ventana del juego
    sf::RenderWindow outerWindow(sf::VideoMode(1650, 880), "Conway's Game of Life");

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
    inner.clear(sf::Color(color_muerto[0], color_muerto[1], color_muerto[2]));

// BOTONES  ------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Asignamos la fuente que vamos a ocupar en el programa
    font.loadFromFile("./Fonts/arial.ttf");
    
    // Creamos todos los diferentes botones y los ponemos dentro de un vector para facilitar su dibujo en pantalla y el evento de click
    vector<std::pair<sf::RectangleShape, sf::Text>> buttons = {
        createButton(180,60, 20, 30, "Evolucion Automatica", 17, 6, 17),
        createButton(180,60, 20, 120, "Detener", 17, 55, 17),
        createButton(180,60, 20, 210, "Siguiente Evolucion", 17, 15, 17),
        createButton(60, 50, 30, 300, "-", 25, 26, 8),
        createButton(60, 50, 120, 300, "+", 25, 24, 10),

        createButton(180, 50, 1200, 20, "Inicializar Juego", 17, 20, 14),
        createButton(180, 50, 1450, 20, "Limpiar Juego", 17, 36, 14),

        createButton(60, 30, 300, 30, "Toro", 20, 6, 2),
        createButton(60, 30, 370, 30, "Nulo", 20, 6, 2),

        createButton(50, 45, 80, 400, "^", 32, 16, 5),
        createButton(50, 45, 80, 450, "v", 24, 18, 5),
        createButton(50, 40, 20, 425, "<", 24, 17, 5),
        createButton(50, 40, 140, 425, ">", 24, 17, 5),

        createButton(180, 60, 20, 620, "Seleccionar Color", 17, 20, 17),
        createButton(180, 60, 20, 710, "Definir regla B/S", 17, 22, 18),

        createButton(80, 50, 20, 800, "Guardar", 14, 15, 17),
        createButton(80, 50, 110, 800, "Abrir", 14, 25  , 17),


        createButton(180, 50, 1450, 810, "Mostrar Graficas", 17, 18, 14)
    };
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Definimos las reglas iniciales del juego
    regla_nacimineto.insert(3);
    regla_sobrevivir.insert(2);
    regla_sobrevivir.insert(3);

    // Agregamos los recuadros para mostrar la cantidad de celdas vivas y la iteracion actual

    sf::RectangleShape label_iteraciones(sf::Vector2f(200, 50));
    label_iteraciones.setFillColor(sf::Color(194, 196, 208));
    label_iteraciones.setPosition(400, 810);

    sf::Text text_iteraciones("Iteracion : " + to_string(total_iteraciones), font, 20);
    text_iteraciones.setFillColor(sf::Color::Black);
    text_iteraciones.setPosition(label_iteraciones.getPosition().x + 5, label_iteraciones.getPosition().y + 13);


    sf::RectangleShape label_celdas_vivas(sf::Vector2f(200, 50));
    label_celdas_vivas.setFillColor(sf::Color(194, 196, 208));
    label_celdas_vivas.setPosition(650, 810);

    sf::Text text_celdas_vivas("Celdas vivas: " + to_string(total_celdas_vivas), font, 18);
    text_celdas_vivas.setFillColor(sf::Color::Black);
    text_celdas_vivas.setPosition(label_celdas_vivas.getPosition().x + 5, label_celdas_vivas.getPosition().y + 13);


    // Bucle que ocupamos para la pantalla mientras ésta esté presente
    while (outerWindow.isOpen()) {
       //  updateGameVisual();
        sf::Event event;
        
        // Limpiamos la pantalla principal y le colocamos un color de fondo
        outerWindow.clear(sf::Color(92,117,140));

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

                            if (action == "Seleccionar Color"){ 
                                outerWindow.setVisible(false);
                                updateColors();
                                outerWindow.setVisible(true);
                            }
                            else if (action == "Definir regla B/S"){
                                outerWindow.setVisible(false);
                                nueva_regla();
                                outerWindow.setVisible(true);
                            }
                            else if(action == "Abrir" || action == "Guardar"){
                                outerWindow.setVisible(false);
                                handlerArchivo(action);
                                if (action == "Abrir") updateValores();
                                outerWindow.setVisible(true);
                            }
                            else if (action == "Mostrar Graficas" && !bandera_automatico){
                                outerWindow.setVisible(false);
                                showGraphs();
                                outerWindow.setVisible(true);

                            }

                            else actionHandler(action);
                    }
                }


                // Dentro de la sigueinte condicional, checamos si es que se presiono alguna celda dentro del tablero
                // de esa manera saber que celda debemos cambiar su estado
                if (innerSprite.getGlobalBounds().contains(mousePosF) && !bandera_automatico) {
                    // Primero debemos encotnrar las coordenadas de la celda presionada
                    int x = (int)mousePosF.x - 230;
                    int y = (int)mousePosF.y - 90;

                    // Asignamos el tama;o de la celda y para poder crearla 
                    sizeCelda_X = zoom[index_zoom];
                    sizeCelda_Y = zoom[index_zoom];

                    // Como estamos ocupando una matrix para el estado de las celdas, debemos saber los indices de esas celdas
                    int index_x = x / sizeCelda_X;
                    int index_y = y / sizeCelda_Y;

                    // Por como se manejan las celdas, es necesario hacer el siguiente paso
                    swap(index_x, index_y);

                    index_x += index_visual_y;
                    index_y += index_visual_x;

                    // Cambiamos la celda a su estado contrario
                    if (matrix[index_y][index_x] ) matrix[index_y][index_x] = false;
                    else  matrix[index_y][index_x] = true;

                    // En caso de que la celda este viva, hay que agregarla al arreglo que ocupamos para dibujar todo el juego
                    // Pero en ambos casos dibujamos la celda en el tablero

                    sf :: RectangleShape celda(sf::Vector2f(sizeCelda_X, sizeCelda_Y));
                    celda.setPosition((index_y)*sizeCelda_X, (index_x)*sizeCelda_Y);

                    if (matrix[index_y][index_x]) {
                        live_cells[index_y].PB(index_x); 
                        celda.setFillColor(sf::Color(color_vivo[0], color_vivo[1], color_vivo[2]));
                        total_celdas_vivas++;
                    }
                    else{ // En caso contrario, tenemos que quitar esa misma celda
                        for (list<int>:: iterator it = live_cells[index_y].begin(); it != live_cells[index_y].end(); it++){
                            if (*it == index_x){
                                live_cells[index_y].erase(it);
                                // Creamos la celda, la posicionamos, ponemos color y la ponemos en el tablero 
                                celda.setFillColor(sf::Color(color_muerto[0], color_muerto[1], color_muerto[2]));
                                inner.draw(celda);
                                total_celdas_vivas--;
                                break;
                            }
                        }
                    }

                    inner.draw(celda);

                }
            }
        }

        if (bandera_automatico) actionHandler("Evolucion Automatica");

        updateGameVisual();

        // Primero mostramos la pantalla del juego para que no se tenga algun efecto de parpadeo
        inner.display();

        // Colocamos en la ventana principal la ventana del juego
        outerWindow.draw(innerSprite);
        
        // -------------------------------------------------------------------------------------------------------------------------
        // Colocamos los diferentes botones
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


        text_iteraciones.setString("Iteracion : " + to_string(total_iteraciones));
        text_celdas_vivas.setString("Celdas vivas : " + to_string(total_celdas_vivas));


        outerWindow.draw(label_iteraciones);
        outerWindow.draw(text_iteraciones);

        outerWindow.draw(label_celdas_vivas);
        outerWindow.draw(text_celdas_vivas);


        // Mostramos la pantalla principal con todos los elementos que colocamos anteriormente
        outerWindow.display();
    }

    return 0;
}
