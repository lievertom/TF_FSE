# Programa

## Proposta

Este trabalho tem por objetivo a realização do controle de temperatura de um sistema. O sistema possui internamente dois dispositivos para alterar sua temperatura. O primeiro é um resistor de potência de 15 Watts utilziado para aumentar temperatura e o segundo, uma ventoínha que puxa o ar externo (à temperatura ambiente) para reduzir a temperatura.

A temperatura de referência (TR) pode ser definida pelo usuário do sistema de duas maneiras:

1. Através de um Potenciômetro;
2. Através de entrada de teclado no terminal.

Maiores informações e requisitos do trabalho disponíveis em:
*   [Gitlab - FSE - Projeto 1](https://gitlab.com/fse_fga/projetos/projeto-1).

## Demonstração

![Demonstração do programa](./ass/demonstration.gif)

*Demonstração realizada as 20h53m em 15 de outubro de 2020 durante uma live da disciplina.*

## Execução

1. Clonar repositório:

    ```bash
    git clone https://github.com/lievertom/Fundamentos-de-Sistemas-Embarcados.git
    ```

2. Entrar no diretório programa:

    ```bash
    cd Fundamentos-de-Sistemas-Embarcados/projeto-1/programa/
    ```

3. Compilar:

    ```bash
    make
    ```

4. Aumentar Tela:

    `F11`

5. Executar:

    ```bash
    bin/bin
    ```

## Comandos

    F2 : pegar valor do potenciômetro para temperatura de referência
    F3 : escolher valor da temperatura de referência 
    F4 : escolher valor da histerese
     ↑ : para cima
     ↓ : para baixo
     ↵ : selecionar
    Esc: fechar

## Autor

|Matrícula | Estudante |
| -- | -- |
| 17/0039251 | Lieverton Santos Silva |

## Referências

*   [Driver da Bosh para o sensor BME280](https://github.com/BoschSensortec/BME280_driver)
*   [Driver do controle do LCD 16x2 em C](http://www.bristolwatch.com/rpi/i2clcd.htm)
*   [Descrição do trabalho prático 01](https://gitlab.com/fse_fga/projetos/projeto-1)