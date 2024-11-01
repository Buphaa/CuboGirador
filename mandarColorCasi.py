import tkinter as tk
from tkinter import messagebox
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
                actualizar_color_objetivo()
    else:
        inicio_verificacion = None  # Reinicia si el color cambia
        timer_label.config(text="")  # Limpia el temporizador si no coinciden
    # Repite verificación cada 100 ms
    root.after(100, verificacion_automatica)

# Crear la ventana principal
root = tk.Tk()
root.title("Juego de Colores")
root.geometry("400x350")

# Crear etiquetas para mostrar el color actual y el objetivo
tk.Label(root, text="Color Actual").pack(pady=10)
actual_label = tk.Label(root, bg=color_hex(color_actual), width=20, height=10)
actual_label.pack()
tk.Label(root, text="Color Objetivo").pack(pady=10)
objetivo_label = tk.Label(root, bg=color_hex(color_objetivo), width=20, height=10)
objetivo_label.pack()

# Etiqueta para el temporizador
timer_label = tk.Label(root, text="", font=("Helvetica", 14))
timer_label.pack(pady=10)

# Botón para cambiar el color objetivo
boton_cambiar = tk.Button(root, text="Nuevo Color Objetivo", command=actualizar_color_objetivo)
boton_cambiar.pack(pady=20)

# Iniciar el ciclo de verificación automática
verificacion_automatica()

# Ejecutar la aplicación
root.mainloop()
