#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "utils/thread_safe_queue.hpp" 
#include "messaging/inbound_gateway.hpp"
#include "utils/fix_generator.hpp"
#include "utils/timestamp_formatter.hpp"
#include "domain/engine.hpp"
#include <iomanip>
#include "domain/order.hpp"
#include <vector>
#include <random>
#include <fstream>
#include <sstream>

// Função que cada thread vai executar
void fix_producer(InboundGateway& gateway, int thread_id, ThreadSafeQueue<std::unique_ptr<Command>>& queue) 
{
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    static thread_local std::uniform_real_distribution<float> dis(0.1, 1.0);

    while (queue.size() < 1000) 
    {
        auto [fixMessage, receivedFixTime] = FixGenerator::generateFIXMessageForThread(); // Recebe mensagem e timestamp
		std::unique_ptr<Command> commandPtr = gateway.parseAndCreateCommand(fixMessage, std::to_string(thread_id), receivedFixTime);

        if (!commandPtr) 
        {
            std::cerr << "Failed to create command from FIX message in thread " << thread_id << ".\n";
			continue;
        }

        gateway.pushToQueue(std::move(commandPtr));

        double sleep_time = dis(gen);
		//std::cout << "Thread " << thread_id << " FIX: " << fixMessage << " - will Sleep for " << sleep_time << " sec\n";
        std::this_thread::sleep_for(std::chrono::duration<double>(sleep_time));
    }
}

int main() {

    ThreadSafeQueue<std::unique_ptr<Command>> commandQueue;
    InboundGateway inboundGateway(commandQueue, "src/logs/write_ahead_log.txt");

	// Inicializa a engine que consome os comandos da fila
	// A engine vai processar os comandos e executar as ações necessárias e para isso criamos uma thread que chama o método consumeQueue()
	// A thread vai ficar rodando em segundo plano, consumindo os comandos da fila e executando-os
	Engine engine(commandQueue);
	std::thread engineThread(&Engine::consumeQueue, &engine);  

    int clientNumber = 3;
    std::vector<std::thread> clients;
    for (int i = 0; i < clientNumber; ++i) 
	{
		std::cout << "Starting thread " << i << "...\n";

		// Pra cada thread, a função fix_producer é passada como callback e tanto a fila quando o id são passados como argumento
		// É equivalente a, dentro da thread, chamar:
		// std::thread t(fix_producer, std::ref(inboundGateway), i, std::ref(commandQueue));
		// fix_producer(inboundGateway, i, std::ref(commandQueue));

		// Depois do emplace_back a thread já começa a rodar, ou seja, a função fix_producer já está sendo executada
        clients.emplace_back(fix_producer, std::ref(inboundGateway), i, std::ref(commandQueue));
    }

    // Serve para que a thread principal (main) aguarde o término de todas as threads criadas
	// Isso é importante para evitar que o programa termine antes das threads concluírem suas execuções
    for (std::thread& t : clients) 
	{
        t.join();
    }

	commandQueue.print_and_clear();

    return 0;
}
