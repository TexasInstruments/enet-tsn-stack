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
Enen though TSN-STACK is a seperate repository, it is not independent. It has dependencies to other repositories such as MCU_PLUS_SDK, FreeRTOS. It uses the build system from MCU_PLUS_SDK and hence please refer to MCU_PLUS_SDK for instructions about compilation, documentation generation and usage.

## Contributing to the project

This project is currently not accepting contributions.
