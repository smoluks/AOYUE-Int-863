#include "error.h"
#include "stm32f1xx.h"

static error_t error = 0;
extern bool need_update_display;

inline void setError(error_t err)
{
    error = err;
    need_update_display = true;
}

inline error_t getError()
{
    return error;
}
