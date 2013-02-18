#ifndef CINTELHEX_H
#define CINTELHEX_H

#define IHEX_DEBUG_OFF

#include <stdlib.h>
#include <stdint.h>

// CONSTANT DEFINITIONS

#define IHEX_ERR_INCORRECT_CHECKSUM   0x01
#define IHEX_ERR_NO_EOF               0x02
#define IHEX_ERR_PARSE_ERROR          0x03
#define IHEX_ERR_WRONG_RECORD_LENGTH  0x04
#define IHEX_ERR_NO_INPUT             0x05
#define IHEX_ERR_UNKNOWN_RECORD_TYPE  0x06
#define IHEX_ERR_PREMATURE_EOF        0x07
#define IHEX_ERR_ADDRESS_OUT_OF_RANGE 0x08
#define IHEX_ERR_MMAP_FAILED          0x09

// TYPE DEFINITIONS

typedef unsigned int  uint_t;
typedef unsigned long ulong_t;

typedef enum { IHEX_DATA = 0x00, IHEX_EOF = 0x01, IHEX_ESA = 0x02,
               IHEX_SSA = 0x03, IHEX_ELA = 0x04, IHEX_SLA = 0x05 } ihex_rtype_t;
typedef enum { IHEX_WIDTH_8BIT = 1, IHEX_WIDTH_16BIT = 2,
               IHEX_WIDTH_32BIT = 4, IHEX_WIDTH_64BIT = 8 } ihex_width_t;
typedef enum { IHEX_ORDER_BIGENDIAN, IHEX_ORDER_LITTLEENDIAN } ihex_byteorder_t;

typedef uint8_t* ihex_rdata_t;
typedef uint8_t  ihex_rlen_t;
typedef uint8_t  ihex_rchks_t;
typedef uint_t   ihex_error_t;
typedef uint16_t ihex_addr_t;

// STRUCTURE DEFINITIONS

/// Models a single Intel HEX record.
/** This structure models a single Intel HEX record, i.e. a single line
 *  in an Intel HEX file. Each record basically consists of a type, a
 *  base address and a data segment of variable length. */
typedef struct ihex_record {
	unsigned int ihr_length;   //!< Length of the record in bytes.
	ihex_rtype_t ihr_type;     //!< Record type (see ihex_rtype_t).
	ihex_addr_t  ihr_address;  //!< Base address offset.
	ihex_rdata_t ihr_data;     //!< Record data.
	ihex_rchks_t ihr_checksum; //!< The record's checksum.
} ihex_record_t;

/// Models a set of Intel HEX records.
/** This structure models an entire set of Intel HEX record, i.e. a
 *  complete Intel HEX input file. Basically, it just consists of a list
 *  of ihex_record_t structures.
 *  The last record must be a special EOF record. */
typedef struct ihex_recordset {
	uint_t         ihrs_count;   //!< Amount of records.
	ihex_record_t *ihrs_records; //!< A list of record (with ihrs_count elements).
} ihex_recordset_t;

// GLOBAL VARIABLES

#ifdef IHEX_PARSE_C
static ihex_error_t ihex_last_errno = 0;
static char*        ihex_last_error = NULL;
#else
extern ihex_error_t ihex_last_errno; //!< Error code of last error.
extern char*        ihex_last_error; //!< Description of last error.
#endif

// METHOD DECLARATIONS

/// Parse Intel HEX string from file.
/** This method parses an Intel HEX string from a file. The file will be
 *  mapped into memory and then parsed. This method returns a pointer to
 *  a newly generated ihex_recordset_t object.
 * 
 *  @param filename The filename of the input file.
 *  @return         A pointer to a newly generated recordset object. */
ihex_recordset_t* ihex_rs_from_file(char* filename);

/// Parse Intel HEX string from string input.
/** This method parses an Intel HEX string from a string input. This
 *  method returns a pointer to a newly generated ihex_recordset_t
 *  object.
 * 
 *  @param data The input string.
 *  @return         A pointer to a newly generated recordset object. */
ihex_recordset_t* ihex_rs_from_string(char* data);

/// Gets a record set's size.
/** This method determines a record set's size. This is done by adding
 *  the length of all records, however without regard to address offsets
 *  etc.
 * 
 *  @param rs A pointer to the record set.
 *  @return   The record set's size. */
ulong_t ihex_rs_get_size(ihex_recordset_t* rs);

/// Return error code, or 0 if no error occurred.
/** This method returns the error code of the latest error.
 *  @return The error code of the latest error. */
ihex_error_t ihex_errno();

/// Checks if a record's checksum is valid.
/** Validate the record by adding up all bytes of a record.
 *  Including the checksum, the lower 8 bytes of the sum of all
 *  bytes must be 0x00.
 * 
 *  @param r The record that is to be validated.
 *  @return  0 if the record is correct, otherwise 1. */
int ihex_check_record(ihex_record_t *r);

/// Copy the content of a record set.
/** This method copies the content of a record set to a certain
 *  location in memory.
 * 
 *  @param rs  The record set that is to be copied.
 *  @param dst A pointer to the destination address.
 *  @param n   The size of the allocated target area.
 *  @param w   The width of data words to be copied.
 *  @param o   Defines whether data words are big or little endian.
 *  @return    0 on success, an error code otherwise. */
int ihex_mem_copy(ihex_recordset_t *rs, void* dst, ulong_t n, ihex_width_t w, ihex_byteorder_t o);

/// Fill a memory area with zeroes.
/** This method fills a whole memory area with zeros.
 * 
 *  @param dst Target area.
 *  @param n   The size of the target area.
 *  @return    0 on success, an error code otherwise. */
int ihex_mem_zero(void* dst, ulong_t n);

/// Return error string, or NULL if no error occurred.
char* ihex_error();

/// Parse 8-bit hex input.
uint8_t ihex_fromhex8(uint8_t *input);

/// Parse 16-bit hex input.
uint16_t ihex_fromhex16(uint8_t *input);
#endif
