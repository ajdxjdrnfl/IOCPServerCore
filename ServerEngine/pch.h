#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <thread>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <Windows.h>
#include <iostream>

#include <assert.h>

#include "Types.h"
#include "TLS.h"
#include "SocketManager.h"
#include "ThreadManager.h"

using namespace std;


#pragma comment(lib, "ws2_32.lib")
