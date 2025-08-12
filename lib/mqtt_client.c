#include "mqtt_client.h"
#include "mqtt_config.h"
#include <string.h>
#include <stdio.h>
#include "pico/cyw43_arch.h"

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)arg;

    if (status == MQTT_CONNECT_ACCEPTED)
    {
        printf("✅ MQTT conectado com sucesso ao broker!\n");
        state->connect_done = true;
        mqtt_subscribe_topics(state);
    }
    else
    {
        printf("❌ Falha na conexão MQTT: %d\n", status);
        state->connect_done = false;
    }
}

// Callback chamado após uma tentativa de publicação MQTT.
// Caso ocorra um erro, imprime uma mensagem indicando a falha.
static void pub_request_cb(void *arg, err_t err)
{
    if (err != ERR_OK)
    {
        printf("Publish failed: %d\n", err);
    }
}

// Callback chamado após uma tentativa de inscrição (subscribe) em um tópico MQTT.
// Caso ocorra um erro, imprime uma mensagem indicando a falha.
static void sub_request_cb(void *arg, err_t err)
{
    if (err != ERR_OK)
    {
        printf("Subscribe failed: %d\n", err);
    }
}

// Callback chamado quando um novo tópico é publicado para este cliente MQTT.
// Armazena o nome do tópico recebido na estrutura de estado.
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)arg;
    strncpy(state->topic, topic, sizeof(state->topic));
}

// Callback chamado quando dados (mensagem) são recebidos de um tópico MQTT.
// Armazena os dados recebidos na estrutura de estado e imprime a mensagem recebida.

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T *)arg;
    strncpy(state->data, (const char *)data, len);
    state->data[len] = '\0';
    printf("Received topic: %s | Message: %s\n", state->topic, state->data);
}

// Publica uma mensagem em um tópico específico utilizando o cliente MQTT.
// A função usa QoS 1 e sem retenção da mensagem.
void mqtt_publish_message(MQTT_CLIENT_DATA_T *state, const char *topic, const char *message)
{
    mqtt_publish(state->mqtt_client_inst, topic, message, strlen(message), 1, 0, pub_request_cb, state);
}
// Realiza a inscrição (subscribe) em diversos tópicos utilizados pelo sistema MQTT.
// A inscrição permite que o cliente receba mensagens desses tópicos.
void mqtt_subscribe_topics(MQTT_CLIENT_DATA_T *state)
{
    // Inscreve-se nos tópicos relevantes
}

// Inicia a conexão do cliente MQTT com o broker.
// Configura os callbacks para recebimento de mensagens e trata possíveis erros de conexão.

void mqtt_start(MQTT_CLIENT_DATA_T *state)
{
    const int port = 1883;

    state->mqtt_client_inst = mqtt_client_new();
    if (!state->mqtt_client_inst)
    {
        panic("MQTT client instance creation error");
    }

    printf("Connecting to MQTT broker at %s\n", ipaddr_ntoa(&state->mqtt_server_address));

    cyw43_arch_lwip_begin();
    if (mqtt_client_connect(state->mqtt_client_inst,
                            &state->mqtt_server_address,
                            port,
                            mqtt_connection_cb,
                            state,
                            &state->mqtt_client_info) != ERR_OK)
    {
        panic("MQTT broker connection error");
    }

    mqtt_set_inpub_callback(state->mqtt_client_inst,
                            mqtt_incoming_publish_cb,
                            mqtt_incoming_data_cb,
                            state);
    cyw43_arch_lwip_end();
}
