import serial
import time

# Conectar al puerto serial Bluetooth
try:
    # Asegúrate de que /dev/rfcomm0 corresponde al puerto Bluetooth de tu ESP32
    bt_port = '/dev/rfcomm0'  
    baud_rate = 115200  # Velocidad de comunicación, debe coincidir con la configurada en el ESP32

    # Inicializar conexión serial
    esp32 = serial.Serial(bt_port, baud_rate)
    print("Conexión establecida con ESP32")

    # Función para enviar colores
    def enviar_color(r, g, b):
        color_data = f"{r},{g},{b}\n"
        esp32.write(color_data.encode())
        print(f"Enviado: {color_data.strip()}")

    # Probar enviando algunos colores
    while True:
        # Cambia los valores de los colores según necesites
        enviar_color(255, 0, 0)  # Rojo
        time.sleep(2)
        enviar_color(0, 255, 0)  # Verde
        time.sleep(2)
        enviar_color(0, 0, 255)  # Azul
        time.sleep(2)

except serial.SerialException as e:
    print(f"No se pudo conectar al ESP32: {e}")
except Exception as e:
    print(f"Error inesperado: {e}")
finally:
    if 'esp32' in locals():
        esp32.close()
