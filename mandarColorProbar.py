import tkinter as tk
from tkinter import messagebox
from PIL import Image, ImageTk
import time
import random
import serial

# Conectar al puerto Bluetooth
try:
    bt_port = '/dev/rfcomm0'
    baud_rate = 115200
    esp32 = serial.Serial(bt_port, baud_rate)
    print("Conexión establecida con ESP32")
except serial.SerialException as e:
    print(f"No se pudo conectar al ESP32: {e}")
    esp32 = None

# Variables para los colores
color_actual = (255, 0, 0)
color_objetivo = (0, 0, 255)
# Tiempo de verificación
inicio_verificacion = None
esperar_ganador = 3  # Segundos necesarios para ganar

# enviar color al ESP32
def enviar_color(r, g, b):
    if esp32:
        color_data = f"{r},{g},{b}\n"
        esp32.write(color_data.encode())
        print(f"Enviado: {color_data.strip()}")

# verificar si los colores son similares
def comparar_colores(c1, c2):
    return all(abs(c1[i] - c2[i]) < 20 for i in range(3))

# Actualizar el color objetivo aleatoriamente
def actualizar_color_objetivo():
    global color_objetivo, inicio_verificacion
    color_objetivo = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
    objetivo_label.config(bg=color_hex(color_objetivo))
    enviar_color(*color_objetivo)
    inicio_verificacion = None  # Reinicia el temporizador
    timer_label.config(text="")  # Limpia el temporizador
    print(f"Color objetivo actualizado: {color_objetivo}")

# Convertir el color RGB a formato hexadecimal para tkinter
def color_hex(rgb):
    return f'#{rgb[0]:02x}{rgb[1]:02x}{rgb[2]:02x}'

# Leer y actualizar el color actual desde el ESP32
def cambiar_color_actual():
    global color_actual
    if esp32 and esp32.in_waiting > 0:
        color_data = esp32.readline().decode().strip()
        try:
            r, g, b = map(int, color_data.split(','))
            color_actual = (r, g, b)
            actual_label.config(bg=color_hex(color_actual))
            print(f"Color actual recibido: {color_actual}")
        except ValueError:
            print(f"Datos inválidos recibidos: {color_data}")

# Función para reiniciar el juego
def reiniciar_juego():
    actualizar_color_objetivo()
    inicio_verificacion = None
    timer_label.config(text="")

# Verificación automática cada 100 ms
def verificacion_automatica():
    global inicio_verificacion
    cambiar_color_actual()  # Actualiza el color actual desde el ESP32
    if comparar_colores(color_actual, color_objetivo):
        if inicio_verificacion is None:
            inicio_verificacion = time.time()  # Comienza el conteo si coincide
        else:
            # Calcula el tiempo transcurrido
            tiempo_restante = esperar_ganador - (time.time() - inicio_verificacion)
            if tiempo_restante > 0:
                timer_label.config(text=f"Tiempo para ganar: {int(tiempo_restante)} s")
            else:
                messagebox.showinfo("¡Ganaste!", "El color coincide con el objetivo.")
                reiniciar_juego()  # Llama a la función de reinicio del juego
    else:
        inicio_verificacion = None  # Reinicia si el color cambia
        timer_label.config(text="")  # Limpia el temporizador si no coinciden
    # Repite verificación cada 100 ms
    root.after(100, verificacion_automatica)

# Crear la ventana principal
root = tk.Tk()
root.title("Juego de Colores")
root.geometry("600x500")

# Añadir imagen de fondo
bg_image = Image.open("ruta/de/tu/imagen.jpg")  # Cambia esta ruta por la de tu imagen
bg_image = bg_image.resize((600, 500), Image.ANTIALIAS)
bg_photo = ImageTk.PhotoImage(bg_image)

bg_label = tk.Label(root, image=bg_photo)
bg_label.place(relwidth=1, relheight=1)

# Crear un frame transparente para los widgets
frame = tk.Frame(root, bg='white', bd=5)
frame.place(relx=0.5, rely=0.1, relwidth=0.75, relheight=0.8, anchor='n')

# Crear etiquetas para mostrar el color actual y el objetivo
tk.Label(frame, text="Color Actual", font=("Helvetica", 14)).pack(pady=10)
actual_label = tk.Label(frame, bg=color_hex(color_actual), width=20, height=10)
actual_label.pack()
tk.Label(frame, text="Color Objetivo", font=("Helvetica", 14)).pack(pady=10)
objetivo_label = tk.Label(frame, bg=color_hex(color_objetivo), width=20, height=10)
objetivo_label.pack()

# Etiqueta para el temporizador
timer_label = tk.Label(frame, text="", font=("Helvetica", 14))
timer_label.pack(pady=10)

# Botón para cambiar el color objetivo con esquinas redondeadas y fuente personalizada
boton_cambiar = tk.Button(frame, text="Nuevo Color Objetivo", command=actualizar_color_objetivo, font=("Helvetica", 12), bg='#ff9999', fg='white', bd=0, relief='solid')
boton_cambiar.pack(pady=20)

# Botón para reiniciar el juego con esquinas redondeadas y fuente personalizada
boton_reiniciar = tk.Button(frame, text="Reiniciar Juego", command=reiniciar_juego, font=("Helvetica", 12), bg='#99ccff', fg='white', bd=0, relief='solid')
boton_reiniciar.pack(pady=10)

# Iniciar el ciclo de verificación automática
verificacion_automatica()

# Ejecutar la aplicación
root.mainloop()
