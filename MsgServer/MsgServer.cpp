// MsgServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "MsgServer.h"
#include "Msg.h"
#include "Session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

int gMaxID = M_USER;
map<int, shared_ptr<Session>> gSessions;
std::chrono::system_clock::time_point minTime = std::chrono::system_clock::now();
CCriticalSection s_CS;

void Timeout()
{
    while (true)
    {
        CSingleLock(&s_CS, TRUE);
        for (map<int, shared_ptr<Session>>::iterator it = gSessions.begin(); it != gSessions.end();)
        {
     
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it->second->lastActivityTime).count() > 10000)
            {
                cout << "User " << it->first<< " left" << endl;
                it = gSessions.erase(it);
            }
            else {
                it++;
            }
        }

        Sleep(2000);
    }
}

void ProcessClient(SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);
    Message m;

    {
        int nCode = m.Receive(s);
        switch (nCode)
        {
        case M_INIT:
        {
            CSingleLock(&s_CS, TRUE);
            auto pSession = make_shared<Session>(++gMaxID, m.m_Data);
            pSession->lastActivityTime = std::chrono::system_clock::now();
            gSessions[pSession->m_ID] = pSession;
            Message::Send(s, pSession->m_ID, M_BROKER, M_INIT);
            break;
        }
        case M_EXIT:
        {
            CSingleLock(&s_CS, TRUE);
            gSessions.find(m.m_Header.m_From)->second->lastActivityTime = minTime;
            Message::Send(s, m.m_Header.m_From, M_BROKER, M_CONFIRM);
            return;
        }
        case M_GETDATA:
        {
            CSingleLock(&s_CS, TRUE);
            gSessions.find(m.m_Header.m_From)->second->lastActivityTime = std::chrono::system_clock::now();
            if (gSessions.find(m.m_Header.m_From) != gSessions.end())
            {
                gSessions[m.m_Header.m_From]->Send(s);
            }
            break;
        }
        default:
        {
            CSingleLock(&s_CS, TRUE);
            gSessions.find(m.m_Header.m_From)->second->lastActivityTime = std::chrono::system_clock::now();
            if (gSessions.find(m.m_Header.m_From) != gSessions.end())
            {
                if (gSessions.find(m.m_Header.m_To) != gSessions.end())
                {
                    cout << "Message from " << m.m_Header.m_From << " to one " << m.m_Header.m_To << endl;
                    gSessions[m.m_Header.m_To]->Add(m);
                }
                else if (m.m_Header.m_To == M_ALL)
                {
                    cout << "Message from " << m.m_Header.m_From << " to all" << endl;
                    for (auto& gSession : gSessions)
                    {
                        if (gSession.first != m.m_Header.m_From)
                        {
                            gSession.second->Add(m);
                        }
                            
                    }
                }
            }
            break;
        }
        }
    }
}


void Server()
{
    AfxSocketInit();

    CSocket Server;
    Server.Create(12345);

    thread t(Timeout);
    t.detach();

    while (true)
    {
        if (!Server.Listen())
            break;
        CSocket s;
        Server.Accept(s);
        thread t(ProcessClient, s.Detach());
        t.detach();
    }
}


// The one and only application object

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
            Server();
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
