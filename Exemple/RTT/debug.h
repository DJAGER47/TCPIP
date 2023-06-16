#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define RTT
// #define SWO


#ifdef RTT
	#include "SEGGER_RTT.h"
	#define _printf(...) SEGGER_RTT_printf(0, __VA_ARGS__)
    #define OFFSET_BUF_RTT __attribute__((section(".rtt")))
#endif
#ifdef SWO
	#define _printf(...) printf(__VA_ARGS__)
#endif

#ifndef _printf
  #define _printf(...)
#endif
