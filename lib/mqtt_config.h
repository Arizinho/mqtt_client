#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
#include <stdbool.h>
#include "pico/cyw43_arch.h"

#define MQTT_TOPIC_LEN 100 // Tamanho máximo do tópico MQTT
#define WIFI_SSID "WIFI_NAME"       // Nome da rede Wi-Fi
#define WIFI_PASSWORD "WIFI_SENHA"   // Senha da rede Wi-Fi

#define MQTT_SERVER "IP_MQTT_SERVER" // Endereço do servidor MQTT
#define MQTT_USERNAME "NOME_USUARIO"      // Nome de usuário para autenticação MQTT
#define MQTT_PASSWORD "SENHA_USUARIO"      // Senha para autenticação MQTT

typedef struct // Estrutura de dados para o cliente MQTT
{
    mqtt_client_t *mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    char data[MQTT_OUTPUT_RINGBUF_SIZE];
    char topic[MQTT_TOPIC_LEN];
    uint32_t len;
    ip_addr_t mqtt_server_address;
    bool connect_done;
    int subscribe_count;
    bool stop_client;
} MQTT_CLIENT_DATA_T;

void mqtt_start(MQTT_CLIENT_DATA_T *state);
void mqtt_publish_message(MQTT_CLIENT_DATA_T *state, const char *topic, const char *message);
void mqtt_subscribe_topics(MQTT_CLIENT_DATA_T *state);

#endif // MQTT_CLIENT_H
