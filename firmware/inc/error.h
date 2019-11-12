#pragma once

enum mainError_e
{
	ERR_NOERROR = 0,
	ERR_CRYSTAL = 1,
	ERR_PLL = 2,
	ERR_OVERRUN = 3
};

void write_error(enum mainError_e error);
