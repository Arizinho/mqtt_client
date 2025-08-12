#include <stdio.h>           // Biblioteca padrão de entrada e saída
#include <math.h>
#include "pico/stdlib.h"     // Biblioteca padrão do Raspberry Pi Pico
#include "pico/cyw43_arch.h" // Biblioteca que fornece funções e recursos para o chip CYW43
#include "lib/mqtt_config.h" // Configurações do cliente MQTT, incluindo SSID, senha, endereço do servidor e credenciais
#include "lib/mqtt_client.h" // Biblioteca que fornece funções e recursos para o cliente MQTT
#include "lwip/dns.h"        // Biblioteca que fornece funções e recursos suporte DNS:
#include "lwip/altcp_tls.h"  // Biblioteca que fornece funções e recursos para conexões TLS
#include "hardware/adc.h"

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_RX_PIN 9
#define UART_TX_PIN 8

#define BUFFER_SIZE 64

// variaveis globais
static MQTT_CLIENT_DATA_T mqtt_state; // Estrutura de dados para o cliente MQTT
char message_buffer[50];              // Buffer para armazenar mensagens recebidas do MQTT

// Prototipagem das funções
void init_mqtt(); //    Inicializa o cliente MQTT

int main()
{
    stdio_init_all(); // Inicializa a entrada e saída padrão
    init_mqtt();      // Inicializa o cliente MQTT

    char buffer[BUFFER_SIZE];
    int idx = 0;

    // Inicializa UART1
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    float tensao, corrente;


    while (true)
    {  
        // Lê caractere por caractere da UART
        while (uart_is_readable(UART_ID)) {
            
            char c = uart_getc(UART_ID);

            if (c == '\n') {
                buffer[idx] = '\0';  // Finaliza string
                idx = 0;
                while (uart_is_readable(UART_ID)){
                    uart_getc(UART_ID);
                }

                // Verifica se está no formato esperado
                if (buffer[0] == 'T') {
                    float v, a;
                    if (sscanf(buffer, "T%f C%f ", &v, &a) == 2) {
                        tensao = v;
                        corrente = a;

                        printf("Recebido -> Tensão: %.2f V, Corrente: %.2f A\n", tensao, corrente);
                    } else {
                        printf("Formato inválido: %s\n", buffer);
                    }
                }
            } else {
                buffer[idx++] = c;
            }
        }
       

        sleep_ms(500);


        char buffer_tensao[32];                                          // Buffer para armazenar o valor de tensão
        char buffer_corrente[32];                                        // Buffer para armazenar o valor de corrente
        snprintf(buffer_tensao, sizeof(buffer_tensao), "%.2f", tensao); // Formata o valor de tensão com duas casas decimais
        snprintf(buffer_corrente, sizeof(buffer_corrente), "%.2f", corrente);

        // Envia os valores corretos aos tópicos
        mqtt_publish_message(&mqtt_state, "/tensao", buffer_tensao);     // Publica o valor de VX no tópico "/tensao"
        mqtt_publish_message(&mqtt_state, "/corrente", buffer_corrente); // Publica o valor de VY no tópico "/corrente"

    }
}

// Inicializa o cliente MQTT e configura as informações de conexão
void init_mqtt()
{
    mqtt_state.mqtt_client_info.client_id = "pico_mqtt_client";
    mqtt_state.mqtt_client_info.client_user = MQTT_USERNAME;
    mqtt_state.mqtt_client_info.client_pass = MQTT_PASSWORD;
    mqtt_state.mqtt_client_info.keep_alive = 60;

    if (cyw43_arch_init())
        panic("CYW43 init failed");
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
        panic("Wi-Fi connection failed");

    cyw43_arch_lwip_begin();
    int err = dns_gethostbyname(MQTT_SERVER, &mqtt_state.mqtt_server_address, NULL, NULL);
    cyw43_arch_lwip_end();
    if (err != ERR_OK)
        panic("DNS failed");

    mqtt_start(&mqtt_state);
    mqtt_subscribe_topics(&mqtt_state);
}