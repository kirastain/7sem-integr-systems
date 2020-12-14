// CppClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "CppClient.h"
#include "../MsgServer/Msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ProcessMessages()
{
    while (true)
    {
        Message m = Message::Send(M_BROKER, M_GETDATA);
        switch (m.m_Header.m_Type)
        {
        case M_DATA:
        {
            cout << "\nMessage from: " << m.m_ClientID << endl;
            cout << m.m_Data << endl;
            break;
        }
        case M_EXIT:
        {
            cout << "You've been disconnected due to inactivity";
            Message::m_ClientID = 0;
            break;
        }
        case M_CONFIRM:
        {
            cout << "You've successfully disconnected from server;";
            Message::m_ClientID = 0;
        }
        default:
        {
            Sleep(100);
            break;
        }
        }
    }
}

void Menu()
{
    cout << "Write action:" << endl;
    cout << "\n1.Connect to Server \n2.Send global message(only if connected) \n3.Send message to certain user(only if connected) \n0. Exit program " << endl;
    int actionId;
    cin >> actionId;
    switch (actionId)
    {
    case 1:
    {
        AfxSocketInit();
        Message m = Message::Send(M_BROKER, M_INIT);
        thread t(ProcessMessages);
        t.detach();
        cout << "Your ID is: " << m.m_Header.m_To << endl;
        break;
    }
    case 2:
    {
        if (!Message::m_ClientID) {
            cout << "Please, connect to server" << endl;
            break;
        }
        cout << "\nWrite message: ";
        string s;
        cin.ignore(256, '\n');
        getline(cin, s, '\n');
        Message::Send(M_ALL, M_DATA, s);
        break;
    }
    case 3:
    {
        if (!Message::m_ClientID) {
            cout << "Please, connect to server" << endl;
            break;
        }
        cout << "\nWrite reñiever ID: ";
        int recieverId;
        cin.ignore(256, '\n');
        cin >> recieverId;
        cin.ignore(256, '\n');
        cout << "\nWrite message: ";
        string s;
        getline(cin, s, '\n');
        Message::Send(M_ALL, M_DATA, s);
        break;
    }
    case 0:
    {
        Message::Send(M_BROKER, M_EXIT);
        break;
    }
    default:
        cout << "Incorrect action";
        break;
    }
}

// The one and only application object
void Client()
{
    while (true)
    {
        Menu();
    }
}

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            Client();
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
