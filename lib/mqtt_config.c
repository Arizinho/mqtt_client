#include "mqtt_client.h"
#include "mqtt_config.h"
#include <stdio.h>
#include <string.h>
#include "pico/cyw43_arch.h"

extern char message_buffer[50];

// Callback chamada após tentativa de publicação MQTT
static void pub_request_cb(void *arg, err_t err)
{
    if (err != ERR_OK)
    {
        printf("Publish failed: %d\n", err);
    }
}

// Callback chamada após tentativa de inscrição (subscribe) em tópico MQTT
static void sub_request_cb(void *arg, err_t err)
{
    if (err != ERR_OK)
    {
        printf("Subscribe failed: %d\n", err);
    }
}

// Callback chamada quando um novo tópico é recebido
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)arg;
    strncpy(state->topic, topic, sizeof(state->topic));
}

// Callback chamada quando uma nova mensagem é recebida em um tópico MQTT
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)arg;
    strncpy(state->data, (const char *)data, len);
    state->data[len] = '\0';
    printf("MQTT Topic: %s | Message: %s\n", state->topic, state->data);
}

// Inicia a conexão MQTT e configura os callbacks de entrada
void mqtt_start(MQTT_CLIENT_DATA_T *state)
{
    const int port = 1883;
    state->mqtt_client_inst = mqtt_client_new();
    if (!state->mqtt_client_inst)
    {
        panic("Failed to create MQTT client");
    }

    printf("Connecting to MQTT broker at %s\n", ipaddr_ntoa(&state->mqtt_server_address));

    cyw43_arch_lwip_begin();
    if (mqtt_client_connect(state->mqtt_client_inst, &state->mqtt_server_address, port,
                            NULL, state, &state->mqtt_client_info) != ERR_OK)
    {
        panic("MQTT broker connection failed");
    }

    mqtt_set_inpub_callback(state->mqtt_client_inst,
                            mqtt_incoming_publish_cb,
                            mqtt_incoming_data_cb,
                            state);
    cyw43_arch_lwip_end();
}

// Publica uma mensagem em um tópico específico
void mqtt_publish_message(MQTT_CLIENT_DATA_T *state, const char *topic, const char *message)
{
    mqtt_publish(state->mqtt_client_inst, topic, message, strlen(message), 1, 0, pub_request_cb, state);
}

// Realiza a inscrição (subscribe) nos tópicos MQTT utilizados
void mqtt_subscribe_topics(MQTT_CLIENT_DATA_T *state)
{
}
