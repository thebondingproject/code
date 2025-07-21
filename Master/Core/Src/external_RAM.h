/*
 * external_RAM.h
 *
 *  Created on: May 9, 2025
 *      Author: snslu
 */

#ifndef SRC_EXTERNAL_RAM_H_
#define SRC_EXTERNAL_RAM_H_

/*APS6408L-3OB PSRAM APmemory*/
#define LINEAR_BURST_READ 0x20
#define LINEAR_BURST_WRITE 0xA0
#define DUMMY_CLOCK_CYCLES_SRAM_READ 5
#define DUMMY_CLOCK_CYCLES_SRAM_WRITE 4
/* Exported macro -----------------------------------------------------*/
#define BUFFERSIZE (COUNTOF(aTxBuffer) - 1)
#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define DLYB_BUFFERSIZE (COUNTOF(Cal_buffer) - 1)
#define EXTENDEDBUFFERSIZE (1048576)
/* USER CODE END Private defines */

#endif /* SRC_EXTERNAL_RAM_H_ */
