# Cliente MQTT para Raspberry Pi Pico W

Este programa lê valores de tensão e corrente recebidos via UART e os publica em tópicos MQTT, permitindo integração com sistemas de monitoramento remoto.

## Funcionalidades

- Recebe dados formatados no padrão `T<valor_tensao> C<valor_corrente>` pela UART1.
- Conecta-se à rede Wi-Fi configurada no arquivo `mqtt_config.h`.
- Publica os valores lidos nos tópicos `/tensao` e `/corrente`.
- Implementa reconexão automática com o broker MQTT.

## Requisitos

- Raspberry Pi Pico W
- Broker MQTT ativo (por exemplo, Mosquitto)
- Arquivo `lib/mqtt_config.h` configurado com:
  - SSID e senha do Wi-Fi
  - Endereço do servidor MQTT
  - Usuário e senha do MQTT
- Biblioteca MQTT para Raspberry Pi Pico (baseada em lwIP)
