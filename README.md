Este proyecto implementa un reloj digital que utiliza un ESP8266 para conectarse a internet y sincronizarse con un servidor NTP (Network Time Protocol), mientras muestra la hora en un conjunto de matrices LED RGB WS2812B organizadas en una cuadrícula de 256 LEDs (4 matrices de 8x8). El código combina múltiples tecnologías para lograr un sistema funcional y visualmente atractivo. A continuación, se detallan los principales componentes y su funcionamiento:

1. Funcionalidad Principal
Sincronización de Tiempo: Usa la librería NTPClient para obtener la hora exacta de un servidor NTP, ajustando la zona horaria a UTC-3 (Argentina).
Display LED: Muestra la hora en formato digital utilizando LEDs RGB controlados por la librería FastLED. Los números se representan como patrones de encendido y apagado en las matrices.
WiFiManager: Configura la conexión WiFi, permitiendo al usuario acceder a un portal de configuración en caso de que no haya credenciales almacenadas.
2. Hardware Utilizado
ESP8266: Microcontrolador que gestiona la conexión WiFi, el tiempo sincronizado y el control de los LEDs.
Matriz WS2812B: Conjunto de LEDs RGB direccionables controlados por un solo pin de datos (DATA_PIN).
WiFiManager: Permite gestionar las credenciales WiFi mediante un portal de configuración.
3. Características Clave
Sincronización NTP:

Se conecta al servidor pool.ntp.org para obtener la hora actual.
Sincroniza cada 60 segundos si hay conexión WiFi.
En caso de pérdida de conexión, continúa calculando el tiempo local basado en la función millis().
Pantalla LED:

Se utiliza la librería FastLED para manejar los LEDs WS2812B.
Cada número se representa en una cuadrícula de 8x8 LEDs, con patrones predefinidos almacenados en una tabla.
Los dígitos de la hora (HH:MM:SS) se despliegan en las cuatro matrices, con dos LEDs adicionales que simulan los puntos ":".
Portal de Configuración WiFi:

Si el dispositivo no puede conectarse a una red WiFi existente, inicia un punto de acceso (RelojAP) que permite configurar las credenciales.
4. Estructura del Código
Configuración Inicial (setup)
Configura la conexión WiFi mediante WiFiManager.
Inicializa el cliente NTP para sincronizar el tiempo.
Configura los LEDs utilizando FastLED.
Bucle Principal (loop)
Actualiza el tiempo local utilizando millis() para mantener la continuidad incluso sin conexión WiFi.
Intenta reconectar el WiFi si la conexión se pierde.
Muestra la hora actual en las matrices LED.
Funciones Auxiliares
updateLocalTime(): Incrementa el tiempo local en segundos utilizando millis().
reconnectWiFi(): Gestiona la reconexión a WiFi en caso de desconexión.
updateDisplay(): Actualiza los LEDs para mostrar los dígitos de la hora, los minutos y los segundos.
drawDigit(): Dibuja cada dígito en la matriz LED correspondiente utilizando una tabla de patrones numéricos.
drawColon(): Controla el encendido de los puntos ":" entre los dígitos de la hora y los minutos.
padZero(): Añade ceros a la izquierda para asegurarse de que los números tengan dos dígitos.
