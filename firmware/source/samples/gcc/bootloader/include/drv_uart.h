/**
 * @file drv_uart.h
 * @brief Interface to the UART communication driver of the BootLoader.
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */
#ifndef _DRV_UART_H    /* avoids multiple inclusion */
#define _DRV_UART_H

#include <stdint.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

#define UART_WITH_FCS                   true
#define UART_WITHOUT_FCS                false

#define UART_RX_DMA_NUM                 0
#define UART_TX_DMA_NUM                 1

#define UART_CLK_HZ                     8000000
#define UART_TX_GPIO                    6
#define UART_RX_GPIO                    5
#define BAUD_RATE                       1000000

#define CFG_TIMEOUT                     30  /* in seconds, 0 = no timeout */
#define CFG_READ_SUPPORT                0

#define NXT_TYPE                '\x55'
#define END_TYPE                '\xAA'

#define CRC32_SIZE              sizeof(crc32_t)
#define CRC32_GOOD              0x2144DF1C
#define CRC32_CONFIG            (CRC_32 | CRC_LITTLE_ENDIAN)

#define DMA_ALIGN               ALIGN(sizeof(uint32_t))

#define MIN(a, b)               ((a) < (b) ? (a) : (b))
#define ALIGN(x)                __attribute__ ((aligned(x)))


/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

typedef uint16_t Drv_Uart_fcs_t;

typedef uint32_t crc32_t;

typedef enum
{
    NO_ERROR,
    BAD_MSG,
    UNKNOWN_CMD,
    INVALID_CMD,
    GENERAL_FLASH_FAILURE,
    WRITE_FLASH_NOT_ENABLED,
    BAD_FLASH_ADDRESS,
    ERASE_FLASH_FAILED,
    BAD_FLASH_LENGTH,
    INACCESSIBLE_FLASH,
    FLASH_COPIER_BUSY,
    PROG_FLASH_FAILED,
    VERIFY_FLASH_FAILED,
    VERIFY_IMAGE_FAILED,
    NO_VALID_BOOTLOADER
} err_t;

typedef enum
{
    HELLO,
    PROG,
    READ,
    RESTART
} cmd_type_t;

typedef struct
{
    uint32_t adr;                   /* start address of image
                                     * (must by a multiple of sector size) */
    uint32_t length;                /* image length in octets
                                     * (must by a multiple of 2) */
    uint32_t hash;                  /* image hash (CRC32) */
} prog_cmd_arg_t;

typedef struct
{
    uint32_t adr;                   /* start address to read from */
    uint32_t length;                /* read length in octets
                                     * (max sector size) */
} read_cmd_arg_t;

typedef union
{
    /* HELLO cmd has no arguments */
    prog_cmd_arg_t prog;
    read_cmd_arg_t read;

    /* RESTART cmd has no arguments */
} cmd_arg_t;

typedef struct
{
    cmd_type_t type;
    cmd_arg_t arg;
} cmd_msg_t;

typedef DMA_ALIGN struct
{
    Sys_Boot_app_version_t boot_ver;    /* version of the BootLoader */
    Sys_Boot_app_version_t app1_ver;    /* version of the installed primary application,
                                         * or 0 if no primary application is installed */
    uint16_t sector_size;               /* Flash sector size in octets */
    Sys_Boot_app_version_t app2_ver;    /* version of the installed secondary application,
                                         * (if no secondary application is installed,
                                         * this field is not included) */
    Drv_Uart_fcs_t fcs;                 /* calculated by drv_uart */
} hello_resp_msg_t;

typedef DMA_ALIGN struct
{
    uint8_t type;                       /* NXT_TYPE or END_TYPE */
    uint8_t code;                       /* one of error_t */
} resp_msg_t;

typedef struct
{
    prog_cmd_arg_t prop;
    crc32_t crc;
    uint32_t header_a[8];
} image_dscr_t;

/* ----------------------------------------------------------------------------
 * Function prototypes
 * ------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Uart_Init(void)
 * ----------------------------------------------------------------------------
 * Description   : Initializes the UART HW.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Uart_Init(void);

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Uart_StartSend(void *msg_p, uint_fast16_t length,
 *                                         bool fcs_b)
 * ----------------------------------------------------------------------------
 * Description   : Starts sending a message.
 * Inputs        : msg_p            - pointer to message
 *                                    (must have an alignment of 4)
 *                 length           - length of message in octets
 *                                    (including optional FCS)
 *                 fcs_b            - UART_WITH_FCS
 *                                    appends a FCS to the message
 *                                  - UART_WITHOUT_FCS
 *                                    no FCS is appended
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Uart_StartSend(void *msg_p, uint_fast16_t length, bool fcs_b);

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Uart_FinishSend(void)
 * ----------------------------------------------------------------------------
 * Description   : Waits for the complete transmission of a message.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Uart_FinishSend(void);

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Uart_StartRecv(uint_fast16_t length)
 * ----------------------------------------------------------------------------
 * Description   : Starts receiving a message.
 * Inputs        : length           - length of message in octets
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Uart_StartRecv(uint_fast16_t length);

/* ----------------------------------------------------------------------------
 * Function      : void * Drv_Uart_FinishRecv(void)
 * ----------------------------------------------------------------------------
 * Description   : Waits for the complete reception of a message
 *                 and returns it.
 * Inputs        : None
 * Outputs       : return value     - pointer to message
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void * Drv_Uart_FinishRecv(void);

/* ----------------------------------------------------------------------------
 * Function      : cmd_msg_t * RecvCmd(void)
 * ----------------------------------------------------------------------------
 * Description   : Receives a command message.
 * Inputs        : None
 * Outputs       : return value     - pointer to message
 * Assumptions   :
 * ------------------------------------------------------------------------- */
cmd_msg_t * RecvCmd(void);

/* ----------------------------------------------------------------------------
 * Function      : void SendResp(uint_fast8_t  type, err_t code)
 * ----------------------------------------------------------------------------
 * Description   : Sends the RESP message.
 * Inputs        : type             - NXT_TYPE or END_TYPE
 *                 code             - one of err_t
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void SendResp(uint_fast8_t type, err_t code);

/* ----------------------------------------------------------------------------
 * Function      : void SendError(err_t error)
 * ----------------------------------------------------------------------------
 * Description   : Sends an error message.
 * Inputs        : error            - one of error_t
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void SendError(err_t error);

/* ----------------------------------------------------------------------------
 * Function      : uint32_t * RecvSector(uint_fast32_t remaining_len,
 *                                              err_t         error)
 * ----------------------------------------------------------------------------
 * Description   : Reads a sector.
 * Inputs        : remaining_len    - remaining length of FW image in octets
 *                 error            - one of error_t
 * Outputs       : return value     - pointer to message buffer or NULL
 * Assumptions   :
 * ------------------------------------------------------------------------- */
uint32_t * RecvSector(uint_fast32_t remaining_len, err_t error);

/* ----------------------------------------------------------------------------
 * Function      : err_t Verify(uint_fast32_t   adr,
 *                                     const uint32_t *data_p,
 *                                     uint_fast32_t   len)
 * ----------------------------------------------------------------------------
 * Description   : Verifies the programmed data and continues calculating hash.
 * Inputs        : adr              - flash start address
 *                 data_p           - pointer to image data to verify against
 *                 len              - length of image data to verify
 * Outputs       : return value     - NO_ERROR            if verify is OK
 *                                  - VERIFY_FLASH_FAILED if verify failed
 * Assumptions   :
 * ------------------------------------------------------------------------- */
err_t Verify(uint_fast32_t adr, const uint32_t *data_p, uint_fast32_t len);

/* ----------------------------------------------------------------------------
 * Function      : err_t ProgFlash(uint_fast32_t   adr,
 *                                        const uint32_t *data_p,
 *                                        uint_fast32_t   len)
 * ----------------------------------------------------------------------------
 * Description   : Programs data to the Flash and verifies it afterwards.
 * Inputs        : adr              - flash start address
 *                 data_p           - pointer to image data to program
 *                 len              - length of image data to program
 * Outputs       : return value     - NO_ERROR            if programming is OK
 *                                  - VERIFY_FLASH_FAILED if verify failed
 *                                  - or a flash HW error
 * Assumptions   :
 * ------------------------------------------------------------------------- */
err_t ProgFlash(uint_fast32_t adr, const uint32_t *data_p, uint_fast32_t len);

/* ----------------------------------------------------------------------------
 * Function      : err_t ProgSector(uint_fast32_t   adr,
 *                                         const uint32_t *data_p,
 *                                         uint_fast16_t   sector_len)
 * ----------------------------------------------------------------------------
 * Description   : Erases a sector and programs it with new data.
 * Inputs        : adr              - flash start address
 *                 data_p           - pointer to image data to program
 *                 sector_len       - length of sector
 * Outputs       : return value     - NO_ERROR            if programming is OK
 *                                  - VERIFY_FLASH_FAILED if verify failed
 *                                  - or a flash HW error
 * Assumptions   :
 * ------------------------------------------------------------------------- */
err_t ProgSector(uint_fast32_t adr, const uint32_t *data_p, uint_fast16_t sector_len);

/* ----------------------------------------------------------------------------
 * Function      : bool CopyVersionInfo(Sys_Boot_app_version_t *buffer_p,
 *                                             uint_fast32_t           image_adr)
 * ----------------------------------------------------------------------------
 * Description   : Copies one image version to the HELLO response.
 * Inputs        : image_adr        - image start address
 * Outputs       : return value     - true  if version info valid
 *                                  - false if version info invalid
 * Assumptions   :
 * ------------------------------------------------------------------------- */
bool CopyVersionInfo(Sys_Boot_app_version_t *buffer_p, uint_fast32_t image_adr);

/* ----------------------------------------------------------------------------
 * Function      : void ProcessHello(void)
 * ----------------------------------------------------------------------------
 * Description   : Processes the HELLO command.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessHello(void);

/* ----------------------------------------------------------------------------
 * Function      : void ProcessProg(prog_cmd_arg_t *arg_p)
 * ----------------------------------------------------------------------------
 * Description   : Processes the PROG command.
 * Inputs        : arg_p            - pointer to PROG command message
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessProg(prog_cmd_arg_t *arg_p);

/* ----------------------------------------------------------------------------
 * Function      : void ProcessRead(cmd_msg_t *cmd_p)
 * ----------------------------------------------------------------------------
 * Description   : Processes the READ command.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessRead(cmd_msg_t *cmd_p);

/* ----------------------------------------------------------------------------
 * Function      : void ProcessRestart(void)
 * ----------------------------------------------------------------------------
 * Description   : Processes the RESTART command.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessRestart(void);

/* ----------------------------------------------------------------------------
 * Function      : void ProcessCmd(void)
 * ----------------------------------------------------------------------------
 * Description   : Processes a CMD message.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessCmd(void);

#endif    /* _DRV_UART_H */
