# Simulador de Matching Engine Financeiro em C++

![Status](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)
![Linguagem](https://img.shields.io/badge/language-C%2B%2B17-blue)
![Licença](https://img.shields.io/badge/license-MIT-green)

## Sobre o Projeto

Este projeto é um simulador de um motor de cruzamento de ordens financeiras (Matching Engine) e seu respectivo livro de ofertas (Order Book), desenvolvido em C++ moderno com foco em design, performance e concorrência. O objetivo é criar uma base sólida que simula o núcleo de uma bolsa de valores, seguindo as melhores práticas de engenharia de software.

## Visão Geral da Arquitetura

O sistema é modelado como uma pipeline de processamento de eventos. As requisições externas (comandos) são recebidas por um `Inbound Gateway`, traduzidas para um formato interno e colocadas em uma fila de comandos. Um `Matching Engine` dedicado consome esses comandos, aplica a lógica de negócio contra o `Order Book` e gera eventos de resultado.

Esses eventos são publicados em um `Event Bus` central, que atua como um roteador, direcionando eventos transacionais e de dados de mercado para canais de saída distintos e especializados. Isso garante que notificações críticas de execução não sejam atrasadas por atualizações de mercado de alto volume.

## Documentação Completa do Projeto

Toda a fase de planejamento e design foi documentada em detalhes, consulte os seguintes documentos:

* 📄 **[Requisitos do Sistema](documentation/requirements.md):** Define o "quê" o sistema deve fazer (Requisitos Funcionais) e o "quão bem" ele deve operar (Requisitos Não-Funcionais e SLOs).
* 🏛️ **[Arquitetura do Sistema](documentation/architecture.md):** Apresenta o "como", com o design de alto nível dos componentes, o fluxograma completo e a responsabilidade de cada parte da arquitetura.
* 📦 **[Modelo de Dados e Entidades](documentation/entities.md):** Detalha as "peças" do sistema, especificando cada entidade de domínio (`Order`, `Trade`) e de mensageria (`Command`, `Event`).
