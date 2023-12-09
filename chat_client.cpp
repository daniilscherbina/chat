#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <iostream>
#include <queue>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;
#define SRV_HOST "localhost"  //адрес сервера
#define SRV_PORT 1234 //порт сервера
#define CLNT_PORT 1235  //порт клиента
#define BUF_SIZE 64  //размер сообщений

struct Message {
    char name[20];
    char text[120];
} get_mess, send_mess;

SOCKET Connection;
char buff[10 * 1014];

void getMessage() {
    while (1) {
        int size = recv(Connection, (char*)&get_mess, sizeof(get_mess), 0);
        if (size < 0) {
            // тут должен быть обработчик ошибок, но с ним все ломается
        }
        else
        {
            // отсекаем сообщения которые отправляли мы
            if (std::string(get_mess.name).compare(std::string(send_mess.name)))
                std::cout << "\033[0;35m  [" << get_mess.name << "]:\033[0m \033[0;33m" << get_mess.text << "\033[0m" << std::endl;
        }
    }
    closesocket(Connection);
    Connection = INVALID_SOCKET;
    return;
}

int connectToServer() {
    cout << "TCP Demo Chat Client\nMax size name 20, max size message 120\nType '!exit' to quit \n";
    // Шаг 1 - иницилизация библиотеки Winsocks
    if (WSAStartup(0x202, (WSADATA*)&buff))
    {
        cout << "WSASTARTUP ERROR: " << WSAGetLastError() << "\n";
        return 1;
    }
    int from_len;
    char buf[BUF_SIZE] = { 0 };
    hostent* hp;
    sockaddr_in clnt_sin, srv_sin;
    Connection = socket(AF_INET, SOCK_STREAM, 0);
    clnt_sin.sin_family = AF_INET;
    clnt_sin.sin_addr.s_addr = 0;
    clnt_sin.sin_port = htons(CLNT_PORT);
    bind(Connection, (sockaddr*)&clnt_sin, sizeof(clnt_sin));
    hp = gethostbyname(SRV_HOST);
    srv_sin.sin_port = htons(SRV_PORT);
    srv_sin.sin_family = AF_INET;
    ((unsigned long*)&srv_sin.sin_addr)[0] = ((unsigned long**)hp->h_addr_list)[0][0];
    connect(Connection, (sockaddr*)&srv_sin, sizeof(srv_sin));

    return 0;
}

int main()
{
    // подключаемся к серверу
    if (connectToServer())
        return -1;
    cout << "you name: ";
    cin >> send_mess.name;

    // создаем поток на получение сообщений от сервера
    std::thread tread(getMessage);
    tread.detach();

    while (1)
    {
        // вводим сообщение
        cin >> send_mess.text;
        // отправляем
        send(Connection, (char*)&send_mess, sizeof(send_mess), 0);
        // если введена команда для выхода, выходим
        if (!std::string(send_mess.text).compare(std::string("!exit")))
        {
            break;
        }
    }
    // шаг последний - выход
    closesocket(Connection);
    WSACleanup();
    return 0;
}