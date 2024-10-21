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
    global color_objetivo
    color_objetivo = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
    objetivo_label.config(bg=color_hex(color_objetivo))
    enviar_color(*color_objetivo)
    print(f"Color objetivo actualizado: {color_objetivo}")

# Convertir el color RGB a formato hexadecimal para tkinter
def color_hex(rgb):
    return f'#{rgb[0]:02x}{rgb[1]:02x}{rgb[2]:02x}'

# Verificar si el jugador ha ganado
def verificar_ganador():
    if comparar_colores(color_actual, color_objetivo):
        messagebox.showinfo("¡Ganaste!", "El color coincide con el objetivo.")
    else:
        messagebox.showwarning("Sigue intentando", "El color no coincide, ¡sigue jugando!")

# Cambiar el color del jugador
def cambiar_color_actual():
    global color_actual
    color_actual = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
    actual_label.config(bg=color_hex(color_actual))

# Crear la ventana principal
root = tk.Tk()
root.title("Juego de Colores")

# Etiquetas para mostrar los colores
objetivo_label = tk.Label(root, text="Color Objetivo", bg=color_hex(color_objetivo), width=30, height=10)
objetivo_label.pack(pady=10)

actual_label = tk.Label(root, text="Color Actual", bg=color_hex(color_actual), width=30, height=10)
actual_label.pack(pady=10)

# Botones para cambiar el color actual y verificar si coincide
btn_actualizar = tk.Button(root, text="Cambiar Color", command=cambiar_color_actual)
btn_actualizar.pack(pady=10)

btn_verificar = tk.Button(root, text="Verificar", command=verificar_ganador)
btn_verificar.pack(pady=10)

btn_nuevo_color = tk.Button(root, text="Nuevo Color Objetivo", command=actualizar_color_objetivo)
btn_nuevo_color.pack(pady=10)

# Inicializar el primer color objetivo
actualizar_color_objetivo()

# Iniciar la ventana de la aplicación
root.mainloop()

# Cerrar la conexión al ESP32 cuando la ventana se cierre
if esp32:
    esp32.close()
