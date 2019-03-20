#ifndef HEX2BIN_H
#define HEX2BIN_H

#define DATA_RECORD                      0
#define END_OF_FILE_RECORD               1
#define EXTEND_SEGMENT_ADDRESS_RECORD    2
#define START_SEGMENT_ADDRESS_RECORD     3
#define EXTEND_LINEAR_ADDRESS_RECORD     4
#define START_LINEAR_ADDRESS_RECORD      5


int analysis_hex(char hex_data[], char bin_data[],int *absolute_addr,int *start_addr, int *bin_length);

#endif