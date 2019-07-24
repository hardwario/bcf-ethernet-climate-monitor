
#include "app_relay_module.h"

void app_relay_module_start(void)
{
    bc_module_relay_init(&relay, 0x3B);
    bc_module_relay_set_state(&relay, false);
}
