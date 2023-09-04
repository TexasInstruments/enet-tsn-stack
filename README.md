<div align="center">

<img src="https://upload.wikimedia.org/wikipedia/commons/b/ba/TexasInstruments-Logo.svg" width="150"><br/>
# Ethernet TSN Stack

[Introduction](#introduction) | [Features](#features) | [Overview](#overview) | [Usage](#usage) | [Contribute](#contributing-to-the-project)

</div>

## Introduction

Ethernet TSN Stack is a generic TSN stack that aims to provide a unified TSN layer for the different Ethernet peripherals found in Sitara MCU+ and J7 class of devices from Texas Instruments. These devices currently include

- [AM2431](https://www.ti.com/product/AM2431), [AM2432](https://www.ti.com/product/AM2432), [AM2434](https://www.ti.com/product/AM2434)
- [AM6411](https://www.ti.com/product/AM6411), [AM6412](https://www.ti.com/product/AM6412), [AM6421](https://www.ti.com/product/AM6421), [AM6422](https://www.ti.com/product/AM6422), [AM6441](https://www.ti.com/product/AM6441), [AM6442](https://www.ti.com/product/AM6442)
- [J7200](https://www.ti.com/product/DRA821U), [J721E](https://www.ti.com/product/DRA829V)

## Features

- IEEE 802.1AS-2020 gPTP
  - End Instance Master and Slave mode
  - Relay Instance for switch mode

- Supports Yang Configuration

- Currently supports FreeRTOS

- Example application as part of corresponding core sdk.


## Overview

### TSN source modules

- tsn_unibase: Universal utility libraries that are platform-independent.
- tsn_combase: Communication utility libraries that provide support for functions
  like sockets, mutexes, and semaphores.
- tsn_gptp: Implementation of the IEEE 802.1 AS gptp protocol.
- tsn_uniconf: Uniconf is a universal configuration daemon for yang. Provides APIs for
  developing a client application which retreives/writes yang parameters from/to database
  and ask the uniconf for configurating HW.
- enet_tsn_example: Example application for Time-Sensitive Networking (TSN).

## Usage
Even though TSN-STACK is a seperate repository, it is not independent. It has dependencies to other repositories such as [MCU_PLUS_SDK](https://github.com/TexasInstruments/mcupsdk-core), [Ethernet Low Level Driver](https://github.com/TexasInstruments/mcupsdk-enet-lld), [FreeRTOS](https://github.com/TexasInstruments/mcupsdk-FreeRTOS-Kernel). It uses the build system from MCU_PLUS_SDK and hence please refer to MCU_PLUS_SDK [documentation](https://software-dl.ti.com/mcu-plus-sdk/esd/AM243X/latest/exports/docs/api_guide_am243x/index.html) about compilation, documentation generation and usage.

## Reference
- [MCU PLUS SDK documentation for AM243x](https://software-dl.ti.com/mcu-plus-sdk/esd/AM243X/latest/exports/docs/api_guide_am243x/index.html)
- [MCU PLUS SDK documentation for AM64x](https://software-dl.ti.com/mcu-plus-sdk/esd/AM64X/latest/exports/docs/api_guide_am64x/index.html)
- [MCU PLUS SDK documentation for AM263x](https://software-dl.ti.com/mcu-plus-sdk/esd/AM263X/latest/exports/docs/api_guide_am263x/index.html)
- [PDK documentation for Jacinto platforms](https://software-dl.ti.com/jacinto7/esd/processor-sdk-rtos-jacinto7/latest/exports/docs/psdk_rtos/docs/user_guide/index.html)

## Contributing to the project

This project is currently not accepting contributions.
