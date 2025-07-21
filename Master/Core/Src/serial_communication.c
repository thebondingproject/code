/**
 * @file serial_communication.c
 * @brief Functions and variables for I2C serial communication between master and slave boards
 */

#include "serial_communication.h"

/**********************************************************************************************
 *  Global variables
 **********************************************************************************************/
uint8_t tx_buffer[MAX_COMM_BUFFER_SIZE];
uint8_t rx_buffer[MAX_COMM_BUFFER_SIZE];

/**********************************************************************************************
 *  Function declarations
 **********************************************************************************************/
/**
 * Zeroes the tx and rx buffers
 */
void serial_communication_variable_init(void)
{
	memset(tx_buffer, 0x00, MAX_COMM_BUFFER_SIZE);
	memset(rx_buffer, 0x00, MAX_COMM_BUFFER_SIZE);
}

/**
 * Prepares the tx buffer for the transmission of an ACK command
 *
 * @param buffer tx buffer pointer
 * @return total message length
 */
uint16_t prepare_ACK_packet(uint8_t * buffer)
{
	uint16_t packet_size = 2;
	buffer[0] = COMMAND_ACK;
	buffer[1] = packet_size;
	return packet_size;
}

/**
 * Prepares the tx buffer for the transmission of a NACK command
 *
 * @param buffer tx buffer pointer
 * @return total message length
 */
uint16_t prepare_NACK_packet(uint8_t * buffer)
{
	uint16_t packet_size = 2;
	buffer[0] = COMMAND_NACK;
	buffer[1] = packet_size;
	return packet_size;
}

/**
 * Prepares the tx buffer for the transmission of a motor START command
 *
 * @param buffer tx buffer pointer
 * @return total message length
 */
uint16_t prepare_START_packet(uint8_t * buffer)
{
	uint16_t packet_size = 2;
	buffer[0] = COMMAND_START;
	buffer[1] = packet_size;
	return packet_size;
}

/**
 * Prepares the tx buffer for the transmission of a motor STOP command
 *
 * @param buffer tx buffer pointer
 * @return total message length
 */
uint16_t prepare_STOP_packet(uint8_t * buffer)
{
	uint16_t packet_size = 2;
	buffer[0] = COMMAND_STOP;
	buffer[1] = packet_size;
	return packet_size;
}

/**
 * Prepares the tx buffer for the transmission of a data WRITE command
 *
 * @param buffer tx buffer pointer
 * @return total message length
 */
uint16_t prepare_WRITE_packet(uint8_t * buffer, uint8_t virtual_memory_address, uint32_t value)
{
	uint16_t packet_size = 7;
	ui32_to_ui8 conversion_union;

	conversion_union.ui32 = value;

	buffer[0] = COMMAND_WRITE;
	buffer[1] = packet_size;
	buffer[2] = virtual_memory_address;
	buffer[3] = conversion_union.ui8[0];
	buffer[4] = conversion_union.ui8[1];
	buffer[5] = conversion_union.ui8[2];
	buffer[6] = conversion_union.ui8[3];
	return packet_size;
}

/**
 * Prepares the tx buffer for the transmission of a data READ command
 *
 * @param buffer tx buffer pointer
 * @return total message length
 */
uint16_t prepare_READ_packet(uint8_t * buffer, uint8_t read_address)
{
	uint16_t packet_size = 3;
	buffer[0] = COMMAND_READ;
	buffer[1] = packet_size;
	buffer[2] = read_address;
	return packet_size;
}

/**
 * Reads a uint32_t word (4 bytes) received from a WRITE command
 *
 * @param buffer pointer to the input buffer (it contains the received WRITE command)
 * @param read_address address at which the WRITE command is directed
 * @param data received uint32_t word
 */
void read_data_from_WRITE_command(uint8_t * buffer, uint8_t * read_address, uint32_t * data)
{
	ui32_to_ui8 conversion_union;
	(*read_address) = buffer[2];
	conversion_union.ui8[0] = buffer[3];
	conversion_union.ui8[1] = buffer[4];
	conversion_union.ui8[2] = buffer[5];
	conversion_union.ui8[3] = buffer[6];
	(*data) = conversion_union.ui32;

	return;
}
