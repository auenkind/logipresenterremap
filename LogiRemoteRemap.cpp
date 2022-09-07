#include <iostream>
#include "interception.h"
#include "utils.h"

enum ScanCode
{
    SCANCODE_POINT = 52,
    SCANCODE_PLAY = 63,
    SCANCODE_A = 30,
    SCANCODE_B = 48,
    SCANCODE_P = 25,
    SCANCODE_N = 49,
    SCANCODE_ESC = 0x01,
    SCANCODE_PGDOWN = 81,
    SCANCODE_PGUP = 73
};

int main()
{
    InterceptionContext context;
    InterceptionDevice device;
    InterceptionKeyStroke stroke;

    raise_process_priority();

    wchar_t hardware_id[500];
    wchar_t hardware_match_id[] = L"HID\\VID_046D&PID_C538&REV_3601&MI_00";

    context = interception_create_context();

    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_UP | INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_E0 | INTERCEPTION_FILTER_KEY_E1);

    while (interception_receive(context, device = interception_wait(context), (InterceptionStroke*)&stroke, 1) > 0)
    {
        size_t length = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));

        int res = wcscmp(hardware_id, hardware_match_id);

       if (res == 0) {
            if(stroke.code == SCANCODE_PLAY || stroke.code == SCANCODE_ESC) stroke.code = SCANCODE_A;
            if(stroke.code == SCANCODE_POINT) stroke.code = SCANCODE_B;

            if (stroke.code == SCANCODE_PGUP) stroke.code = SCANCODE_P;
            if (stroke.code == SCANCODE_PGDOWN) stroke.code = SCANCODE_N;

            if (stroke.state == INTERCEPTION_KEY_E0) stroke.state = INTERCEPTION_KEY_DOWN;
            if (stroke.state == INTERCEPTION_KEY_E1) stroke.state = INTERCEPTION_KEY_UP;

            interception_send(context, device, (InterceptionStroke*)&stroke, 1);
            continue;
        }
        else {
            interception_send(context, device, (InterceptionStroke*)&stroke, 1);
        }

    }

    interception_destroy_context(context);

    return 0;
}