#include <windows.h>
#include <math.h>

#include "task.h"
#include "utils.h"

int moveMouseImpl(long x, long y) {
    INPUT in = {0};
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    in.mi.dx = (65535 * x) / GetSystemMetrics(SM_CXSCREEN);
    in.mi.dy = (65535 * y) / GetSystemMetrics(SM_CYSCREEN);
    SendInput(1, &in, sizeof(INPUT));
    return 0;
}

int moveMouseToImpl(long x, long y, long totalMs) {
    POINT p;
    if (!GetCursorPos(&p)) return 1;
    const int startX = p.x;
    const int startY = p.y;

    if (totalMs <= 0) {
        return moveMouseImpl(x, y);
    }

    LARGE_INTEGER freq, t0, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);

    timeBeginPeriod(1);

    int lastX = startX, lastY = startY;

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsedMs = (now.QuadPart - t0.QuadPart) * 1000.0 / (double)freq.QuadPart;

        double t = clamp01(elapsedMs / (double)totalMs);
        double s = t;

        int newX = (int)llround(startX + (x - startX) * s);
        int newY = (int)llround(startY + (y - startY) * s);

        if (newX != lastX || newY != lastY) {
            INPUT in = {0};
            in.type = INPUT_MOUSE;
            in.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
            int w = GetSystemMetrics(SM_CXSCREEN) - 1;
            int h = GetSystemMetrics(SM_CYSCREEN) - 1;
            in.mi.dx = (int)((65535.0 * newX) / w);
            in.mi.dy = (int)((65535.0 * newY) / h);
            SendInput(1, &in, sizeof(INPUT));
            lastX = newX;
            lastY = newY;
        }

        if (t >= 1.0) break;
        Sleep(1);
    }
    timeEndPeriod(1);

    return moveMouseImpl(x, y);
}

int sleepImpl(long totalMs) {
    Sleep(totalMs);
    return 0;
}
