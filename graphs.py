import tkinter as tk
import math
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

def create_graph(ax, canvas, option):
    # Limpiamos el canvas para poder poner la nueva grafica
    ax.clear()
    
    # Declaración de variables necesarias para poder controlar el nombre de archivo
    # y etiqueta de la grafica
    label_Y = "# Population"
    nombre_archivo = "normal.txt"

    # Creamos las etiquetas en X y Y para las graficas
    ax.set_xlabel("Iteration")
    ax.set_ylabel(label_Y)

    # Declaramos la variable y estructura de datos para mostrar la grafica
    index = 0
    data = []

    # Abrimos el archivo de texto para leer los valores de la gráfica
    with open(nombre_archivo, "r") as file:
        # Recorremos todo el archivo linea por linea
        for line in file:
            # Transformamos el numero de la linea a flotante
            line = float(line.strip())

            # Si es el caso, aplicamos logaritmo base 10 al numero anterior
            if option == "Population Log10" and line > 0:
                line = math.log10(line)

            # Guardamos el numero de iteracion y el valor para la grafica en ese punto
            data.append(tuple([index, line]))
            # Aumentamos el numero de iteraciones dependiendo el numero de lineas
            index += 1

    # Asignamos los valores en X y Y de la estructura de datos anteriormente declarada
    x, y = zip(*data) 

    # Colocamos todos los puntos de datos en la grafica
    ax.scatter(x, y, color="blue")

    # Colocamos la figura final en el canva
    canvas.draw()

def main():
    root = tk.Tk()

    # Definimos las dimensiones que va a tomar la pantalla donde mostraremos las graficas
    root.geometry("1250x700")

    # Creamos una etiqueta mara mostrar unicamente lo que estamos mostrando
    label = tk.Label(root, text="Population", font=("Arial", 18))
    label.pack(pady=(5,5))

    # Creamos la figura junto con la grafica que mostraremos primero
    fig = Figure(figsize=(11, 6))
    ax = fig.add_subplot(111)

    # Ponemos la figura de la grafica en un canvasde python 
    canvas = FigureCanvasTkAgg(fig, master=root)
    canvas.draw()
    canvas.get_tk_widget().pack()
    
    # Mostramos la primera grafica
    create_graph(ax, canvas,  "Population")

    # Creamos una variable para poder almacenar la opcion de la grafica a observar
    var = tk.StringVar(root)
    var.set("Population")

    # Creamos un menu desplegable
    option_menu = tk.OptionMenu(root, var, "Population", "Population Log10", command=lambda choice: create_graph(ax, canvas, choice))
    option_menu.config(width=15, heigh=3, font=("Arial", 14))
    option_menu.pack(pady=(20,0))

    root.mainloop()

if __name__ == "__main__":
    main()
