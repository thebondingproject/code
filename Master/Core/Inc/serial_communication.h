/**
 * @file serial_communication.h
 * @brief header file for serial_communication.c
 * @details
 *
 * YY: Command code
 * - 0x01: ACK
 * - 0x02: NACK
 * - 0x03: START
 * - 0x04: STOP
 * - 0x05: WRITE
 * - 0x06: READ
 * - 0x07: Enter in "position control" mode
 * - 0x08: Enter in "speed control" mode
 *
 * 0xZZ: Virtual memory addresses
 * - 0x00 = Setpoint position
 * - 0x01 = Setpoint speed
 * - 0x02 = Setpoint acceleration
 * - 0x03 = Measured position
 * - 0x04 = Measured speed
 * - 0x05 = Measured current
 *
 ***********************************************************************************************
 *
 * Generic example:
 * 0xYY 0xKK 0xZZ 0xUU 0xUU 0xUU 0xUU
 * where 0xKK is the packet length (0xYY included, which in this example is 0x07)
 *
 * Communication example for reading:
 * Master says: 0x06 0x03 0x01
 * Slave says:  0x05 0x07 0x01 0xUU 0xUU 0xUU 0xUU
 *
 * Communication example for writing:
 * Master says: 0x05 0x07 0x00 0xUU 0xUU 0xUU 0xUU
 * Slave says:  0x01 0x02 if the write operation did succeed
 *              0x02 0x02 if the write operation did NOT succeed
 *
 * Communication example for motor start:
 * Master says: 0x03 0x02
 * Slave says:  0x01 0x02 if the motor start did succeed
 *              0x02 0x02 if the motor start did NOT succeed
 * 			    NB: ACK or NACK get sent to Master after a random delay time betwwn 0 and 5ms to avoid bus conflicts with other slaves.
 */

#ifndef INC_SERIAL_COMMUNICATION_H_
#define INC_SERIAL_COMMUNICATION_H_

#include <stdint.h>
#include <string.h>


/**********************************************************************************************
 *  Function primitives
 **********************************************************************************************/
void serial_communication_variable_init(void);
uint16_t prepare_ACK_packet(uint8_t * buffer);
uint16_t prepare_NACK_packet(uint8_t * buffer);
uint16_t prepare_START_packet(uint8_t * buffer);
uint16_t prepare_STOP_packet(uint8_t * buffer);
uint16_t prepare_WRITE_packet(uint8_t * buffer, uint8_t virtual_memory_address, uint32_t value);
uint16_t prepare_READ_packet(uint8_t * buffer, uint8_t read_address);
void read_data_from_WRITE_command(uint8_t * buffer, uint8_t * read_address, uint32_t * data);

/**********************************************************************************************
 *  Unions
 **********************************************************************************************/
/** Union to convert an uint32_t value to a 4 byte array of uint8_t values (and vice versa)*/
typedef union {
    uint32_t ui32;  /**< uint32_t value */
    uint8_t ui8[4]; /**< Equivalent uint8_t array */
} ui32_to_ui8;

/** Union to convert an uint32_t value to a float, byte per byte (and vice versa)*/
typedef union {
    uint32_t ui32;  /**< uint32_t value */
    float f; /**< Equivalent float, converted byte per byte */
} ui32_to_f;

/**********************************************************************************************
 *  Defines
 **********************************************************************************************/
/** Command execution finished correctly */
#define COMM_OK   0
/** Command execution encountered an error */
#define COMM_ERR -1

/** X axis slave board I2C adress */
#define ADDRESS_MOTOR_X 0x14<<1
/** Y axis slave board I2C adress */
#define ADDRESS_MOTOR_Y 0x15<<1
/** Z axis slave board I2C adress */
#define ADDRESS_MOTOR_Z 0x16<<1
/** Theta axis slave board I2C adress */
#define ADDRESS_MOTOR_T 0x17<<1

/** ACK command code */
#define COMMAND_ACK         0x01
/** NACK command code */
#define COMMAND_NACK        0x02
/** Motor START command code */
#define COMMAND_START       0x03
/** Motor STOP  command code */
#define COMMAND_STOP        0x04
/** Data WRITE command code */
#define COMMAND_WRITE       0x05
/** Data READ command code */
#define COMMAND_READ        0x06

/** Virtual memory address for POSITION*/
#define VIRTUAL_MEM_POSITION      0x00
/** Virtual memory address for SPEED*/
#define VIRTUAL_MEM_SPEED         0x01
/** Virtual memory address for ACCELERATION*/
#define VIRTUAL_MEM_ACCELERATION  0x02
/** Virtual memory address for Measured POSITION*/
#define VIRTUAL_MEM_POSITION_MEAS 0x03
/** Virtual memory address for Measured SPEED*/
#define VIRTUAL_MEM_SPEED_MEAS    0x04
/** Virtual memory address for MODE */
#define VIRTUAL_MEM_MODE          0x05

/** Maximum message size (32 bytes) */
#define MAX_COMM_BUFFER_SIZE 32
/** Timeout for the HAL I2C transmit function */
#define MAX_TRANSMISSION_TIME 100

/** Slave POSITION control mode */
#define POS_MODE 0
/** Slave SPEED    control mode */
#define SPD_MODE 1
/** Slave CURRENT  control mode */
#define CUR_MODE 2

/**********************************************************************************************
 *  Global variables
 **********************************************************************************************/
/** Transmit buffer */
extern uint8_t tx_buffer[MAX_COMM_BUFFER_SIZE];
/** Receive buffer */
extern uint8_t rx_buffer[MAX_COMM_BUFFER_SIZE];

#endif /* INC_SERIAL_COMMUNICATION_H_ */
