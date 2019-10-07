/*
 * Два потока по очереди выводят в консоль сообщение.
 * Первый выводит ping, второй выводит pong.
 * Количество ping и pong - по 500 000 каждый.
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable printed;
bool pingPrinted = false;
const int N = 500000;


void printPing()
{
    for (int i = 0; i < N; i++) {
        std::unique_lock<std::mutex> lock(m);
        printed.wait(lock, [] { return !pingPrinted; });
        std::cout << "ping\n";
        pingPrinted = true;
        printed.notify_one();
    }
}


void printPong()
{
    for (int i = 0; i < N; i++) {
        std::unique_lock<std::mutex> lock(m);
        printed.wait(lock, [] { return pingPrinted; } );
        std::cout << "pong\n";
        pingPrinted = false;
        printed.notify_one();
    }
}


int main()
{
    std::thread ping(printPing);
    std::thread pong(printPong);

    ping.join();
    pong.join();

    return 0;
}
