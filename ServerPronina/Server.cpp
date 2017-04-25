#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#include <iostream>
#include <vector>
#include <thread>
#pragma comment (lib,"Ws2_32.lib")

struct SEND
{
	SEND()
	{
		typemessage = 0;
		countpeople = 0;
		stopchat = false;
		ZeroMemory(name, sizeof(char) * 14);
		ZeroMemory(buffer, sizeof(char) * 202);
	}

	int typemessage;
	int countpeople;
	bool stopchat;
	char name[14];
	char buffer[202];
};

//Сокет для подключения, для хранения подключенных и для "прослушка" + подсчет количества подключенных 

SOCKET Connect;
SOCKET Listen;
int Count = 0;

std::vector<SOCKET> Connection(64);

//Функция для отправки-приема сообщений... Принимает сообщение - рассылает всем подключенным

void decompres(int n)
{
	for (int i = n;i < Count - 1;i++)
		Connection[i] = Connection[i + 1];
	Connection[Count] = NULL;
}

void SendM(int ID)
{
	for (;;Sleep(75))
	{
		SEND s;
		int iResult = recv(Connection[ID], (char*)&s, sizeof(s), NULL);
		std::cout << s.countpeople << ' ' << s.name << ' ' << s.typemessage << std::endl;
		if (s.typemessage == 3)
		{
			for (int i = 0;i <= Count;i++)
				send(Connection[i], (char*)&s, sizeof(s), NULL);
			closesocket(Connection[ID]);
			decompres(ID);
			Count--;
			break;
		}
		if (iResult>0)
		{
			for (int i = 0;i <= Count;i++)
				send(Connection[i], (char*)&s, sizeof(s), NULL);
		}
	}
	std::cout << ID << " off\n";
}

int main()
{
	setlocale(LC_ALL, "russian");

	std::cout << "Подключение сервера...\n";
	WSAData  data;
	WORD version = MAKEWORD(2, 2);
	int res = WSAStartup(version, &data);
		if (res != 0)
		{
			return 0;
		}
	struct addrinfo hints;
	struct addrinfo * result;

	//Connection = new SOCKET[64];

	ZeroMemory(&hints, sizeof(hints));

	//Задание сокетов

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//установка ip и порта

	std::cout << "Введите IP адрес сервера в формате 000.000.000.000:\n";
	std::string iport; std::cin >> iport;
	std::cout << "Введите port:\n";
	std::string port; std::cin >> port;
	getaddrinfo(iport.c_str(), port.c_str(), &hints, &result);

	//Заполнение сокета listen

	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen, result->ai_addr, result->ai_addrlen);
	listen(Listen, SOMAXCONN);
	freeaddrinfo(result);

	//Начало работы сервера

	std::cout << "Ожидание клиентов..." << std::endl;
	char c_connect[] = "Connect";
	for (;;Sleep(75))
	{
	//проверка на получение сигнала от кого-нибудь

		if (Connect = accept(Listen, NULL, NULL))
		{
			std::cout << c_connect << ' ' << Count << std::endl;
			Connection[Count] = Connect;
			send(Connection[Count], c_connect, strlen(c_connect), NULL);
			Count++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendM, (LPVOID)(Count - 1), NULL, NULL);
		}
	}
	return 0;
}