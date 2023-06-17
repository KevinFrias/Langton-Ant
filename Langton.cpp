#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>
// #include <gtk/gtk.h>
#define PB push_back
using namespace std;

// Declaracion del size total de programa junto con el size de cada celda
int n = 1400;
int n_archivo = 0;
int sizeCelda_X, sizeCelda_Y;

// Declaracion de la estructura de la hormiga que se ocupara dentro del programa
struct Ant {
    int edad;
    int direccion;
    int tipo;
};

// La densidad de hormigas la guardaremos dentro de un arreglo para tener un control general de este

// Hashmap para el control del color de las hormigas dependiendo de su tipo en caso de 
// que se quieran mostrar las etiquetas
map <int, tuple<int,int,int>> colors;


// Matrices necesarias para los diferentes estados de la simulacion
// Son enteros ya que guardaremos el tipo de hormiga en cada celda
vector <vector <bool> > matrix(n, vector <bool> (n, false));
vector <vector <bool> > matrix_next(n, vector <bool> (n, false));
vector <vector <bool> > matrix_clean(n, vector <bool> (n, false));


// Arreglo para mantener el control de las hormigas que existen
set <pair<int,int>> celdas_vivas;


// Arreglo para almacenar las hormigas que se encuentren en la simulacion
// Almacenamos las coordenadas de la hormiga y la informacion relevante a esa hormiga
map < pair<int,int>, Ant> hormigas;

// El arreglo temporal unicamente lo utilizaremos para mostrar los cambios que se hayan hecho 
// a las hormigas en un instante de tiempo
map < pair<int,int>, Ant> hormigas_temporal;


// Arreglos para manetener un control de todos los valores que se usan en las graficas
vector <int> valores_grafica_normal;
vector <int> valores_grafica_entriopia;
unordered_map <int,int> entropy;

// Definimos los valores RGB del fondo de la pantalla de la simulacion
int color_fondo[] = {0,0,0};

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
int index_zoom = 15;
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
    /*
    // Creamos la ventana donde manejaremos el cambio de color de las celdas
    sf::RenderWindow windowColor(sf::VideoMode(660, 250), "Cambio de color");

    auto buttonOk = createRectangle(100, 50, 280, 170, "OK", 24, 32, 10);

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
*/
   return;
}

/*
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
*/

// Donde van a estar las hormigas? -> map <pair<int,int> ant>

// Como voy a pintar la simulacion? -> list <list> cells;


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
                celda.setFillColor(sf::Color(255, 255, 255));
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

    set <int> direccion = {2,3};

    // Seleccionamos un numero entre 1 y 2 para las posiciones disponibles
    std::uniform_int_distribution<> dis(0, direcciones.size() - 1);

    int randomIndex = dis(gen);

    return direcciones[randomIndex];
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


bool checkFront(int direccion, int x, int y, bool nacimiento){
    int tipo = hormigas[{x, y}].tipo;



}


void nextState(){
    // Como unicamente tengo que tomar en cuenta las hormigas que estan vivas o que existen
    // entonces estaria mejor que unicamente ocupe el arreglo de donde se encutran las hormigas, no?
    // Distribucion del tipo de hormigas
    // 0 -> Reinas
    // 1 -> Trabajadoras
    // 2 -> Reproductoras
    // 3 -> Soldado

    // Que tengo que recorrer? Tengo que checar el arreglo de las hormigas
    map<pair<int,int>, Ant>:: iterator i = hormigas.begin();

    map <pair<int,int>, Ant> nacimiento;

    for (; i != hormigas.end(); i++){
        int x = i->first.first;
        int y = i->first.second;

        // En caso de que la hormiga cumpla con la condicion maxima de iteraciones, se quitara de la simulacion
        if (i->second.edad == 80) hormigas.erase(i);
        else {
            i->second.edad += 1;
            
            // En caso de que la hormiga que estamos checando sea reproductora, comprobamos si es que se cumple
            // la regla de nacimiento
            if (i->second.tipo == 2){
                if (i->second.direccion == 1){
                    int arriba = i->first.second;
                    if (arriba - 1 >= 0) arriba -= 1;
                    else if (bandera_nulo) arriba = n -1;

                    if (hormigas.find({x, arriba}) != hormigas.end())
                        if (hormigas[{x, arriba}].tipo == 0 && hormigas[{x, arriba}].direccion == 3){
                            nacimiento[{x, y}] = i->second;
                            nacimiento[{x, y}].edad = 0;
                            nacimiento[{x, y}].tipo = 3;
                        }
                }

                else if (i->second.direccion == 2){
                    int derecha = i->first.first;
                    if (derecha + 1 < n) derecha += 1;
                    else if (bandera_nulo) derecha = 0;

                    if (hormigas.find({x, derecha}) != hormigas.end())
                        if (hormigas[{x, derecha}].tipo == 0 && hormigas[{x, derecha}].direccion == 4){
                            nacimiento[{x, y}] = i->second;
                            nacimiento[{x, y}].edad = 0;
                            nacimiento[{x, y}].tipo = 3;
                        }
                }

                else if (i->second.direccion == 3){
                    int abajo = i->first.second;
                    if (abajo + 1 < n) abajo += 1;
                    else if (bandera_nulo) abajo = 0;

                    if (hormigas.find({x, abajo}) != hormigas.end())
                        if (hormigas[{x, abajo}].tipo == 0 && hormigas[{x, abajo}].direccion == 1){
                            nacimiento[{x, y}] = i->second;
                            nacimiento[{x, y}].edad = 0;
                            nacimiento[{x, y}].tipo = 3;
                        }
                }

                else if (i->second.direccion == 4){
                    int izquierda = i->first.first;
                    if (izquierda - 1 >= 0) izquierda -= 1;
                    else if (bandera_nulo) izquierda = n - 1;

                    if (hormigas.find({x, izquierda}) != hormigas.end())
                        if (hormigas[{x, izquierda}].tipo == 0 && hormigas[{x, izquierda}].direccion == 2){
                            nacimiento[{x, y}] = i->second;
                            nacimiento[{x, y}].edad = 0;
                            nacimiento[{x, y}].tipo = 3;
                        }
                }
            }

            if (i->second.tipo == 0){


            }


            int direccion = i->second.direccion;

            // Checamos la condicion y comprobar si es que alguna nueva hormiga nace

            if (matrix[y][x]){
                direccion = (direccion + 1) % 5;
                direccion += (direccion == 0 ? 1 : 0);
            }
            else{
                direccion -= 1;
                direccion = (direccion == 0 ? 4 : direccion); 
            }

            if (matrix[y][x] == false){
                matrix[y][x] = true;
                celdas_vivas.insert({x, y});
            }
            else {
                matrix[y][x] = false;
                celdas_vivas.erase({x, y});
            }

            pair <int,int> coordenadas = checkMovement(direccion, x , y);

            if (coordenadas.first != -1){
                auto actual = hormigas[{x, y}];
                actual.direccion = direccion;
                hormigas.erase(hormigas.find({x,y}));

                hormigas[{coordenadas.first, coordenadas.second}] = actual;
            }

            else {
                // Ponemos dentro del arreglo de las nuevas direcciones la nuevas direcciones
                // para poder seleccionarla de manera aleatoria, exluyo la direccion de la que vengo
                // y la que no fue posible en un inicio
                vector <int> direcciones = {direccion + 1, direccion + 2};

                // Seleccionamos la nueva direccion
                int nueva_direaccion = getNewPosition({2, 3});
                // Checamos si esa nueva posicion es valida
                coordenadas = checkMovement(nueva_direaccion, x , y);

                // En caso de que sea posible agregamos esa nueva posicion
                if (coordenadas.first != -1){
                    auto actual = hormigas[{x, y}];
                    actual.direccion = nueva_direaccion;
                    hormigas.erase(hormigas.find({x,y}));
                    hormigas[{coordenadas.first, coordenadas.second}] = actual;
                }
            }

        }
    }

    // Agregamos todas las hormigas que salieron de la condicion de nacimiento
    for (auto i : nacimiento)
        hormigas[{i.first.first, i.first.second}] = i.second;

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
            actual.tipo = 1;
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

        // Aumentamos el numero total de iteraciones
        total_iteraciones++;

        // En caso de que se haya presionado algun boton referente al estado del juego, hay que mover todas las hormigas 
        // que estan dentro del arreglo temporal al arreglo general, significando que los cambios en las hormigas son aplicados

        for (auto i : hormigas_temporal) hormigas[i.first] = i.second;
        hormigas_temporal.clear();

        nextState();
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
    }

    if (action == "Inicializar Juego"){

    }

    return;
}


int main() {

    // x, y

    celdas_vivas.insert({3, 0});
    celdas_vivas.insert({5, 0});
    celdas_vivas.insert({7, 0});
    celdas_vivas.insert({9, 0});

    index_zoom -= 3;

    color_hormigas[0] = {255,99,71};
    color_hormigas[1] = {50,205,50};
    color_hormigas[2] = {175,238,238};
    color_hormigas[3] = {221,160,221};

/*
    Ant actual;
    actual.edad = 0;
    actual.direccion = 1;
    actual.tipo = 0;

    hormigas[{0,0}] = actual;
*/

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

        createRectangle(180, 60, 20, 620, "Seleccionar Color", 17, 20, 17),
        createRectangle(180, 60, 20, 710, "Definir regla B/S", 17, 22, 18),

        createRectangle(80, 50, 20, 800, "Guardar", 14, 15, 17),
        createRectangle(80, 50, 110, 800, "Abrir", 14, 25  , 17),


        createRectangle(180, 50, 1450, 810, "Mostrar Graficas", 17, 18, 14)
    };

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Creamos un arreglo para poder tener las etiquetas necesarias en un solo lugar
    vector <std::pair<sf::RectangleShape, sf::Text>> etiquetas = {
        createRectangle(200, 50, 400, 810, "Iteracion : " + to_string(total_iteraciones), 20, 5, 13),
        createRectangle(200, 50, 650, 810, "Celdas vivas : " + to_string(total_celdas_vivas), 18, 5, 13)
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
                            actionHandler(action);
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
