/**
 * @file drv_uart.c
 * @brief The UART driver implements the lower layers of the communication
 *        protocol of the BootLoader.
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
#include <stdint.h>
#include <string.h>
#include "bootloader.h"
#include "drv_uart.h"
#include "drv_targ.h"

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

#define CRC_CCITT_SIZE                  sizeof(Drv_Uart_fcs_t)
#define CRC_CCITT_GOOD                  0xF0B8
#define CRC_CONFIG                      (CRC_CCITT                      | \
                                         CRC_LITTLE_ENDIAN              | \
                                         CRC_BIT_ORDER_NON_STANDARD     | \
                                         CRC_FINAL_REVERSE_NON_STANDARD)

#define NUM_RX_BUF                      2
#define RX_BUF_SIZE                     (FLASH_SECTOR_SIZE + CRC_CCITT_SIZE)
#define MAX_CHAR_DELAY                  20      /* in milliseconds */

#define DIV_CEIL(n, d)                  (((n) + (d) - 1) / (d))


/* DMA config for TX */
#define DMA_TX_CONFIG                   (DMA_DEST_UART0            | \
		                                 DMA_LITTLE_ENDIAN         | \
                                         DMA_DISABLE               | \
                                         DMA_SRC_ADDR_INCR_1       | \
										 DMA_DEST_ADDR_STATIC      | \
										 DMA_PRIORITY_0            | \
                                         WORD_SIZE_8BITS_TO_8BITS)

/* DMA config for RX */
#define DMA_RX_CONFIG                   (DMA_SRC_UART0             | \
		                                 DMA_LITTLE_ENDIAN         | \
                                         DMA_DISABLE               | \
                                         DMA_DEST_ADDR_INCR_1      | \
										 DMA_SRC_ADDR_STATIC       | \
										 DMA_PRIORITY_0            | \
                                         WORD_SIZE_8BITS_TO_8BITS)


/* ----------------------------------------------------------------------------
 * Local variables and types
 * --------------------------------------------------------------------------*/

typedef struct
{
    uint32_t active;
    uint32_t data_a[NUM_RX_BUF][DIV_CEIL(RX_BUF_SIZE, sizeof(uint32_t))];
} rx_buffer_t;

rx_buffer_t Drv_Uart_rx_buffer;
static uint16_t mod_start_dma_cnt;


/* ----------------------------------------------------------------------------
 * Function      : void Drv_Uart_Init(void)
 * ----------------------------------------------------------------------------
 * Description   : Initializes the UART HW.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Uart_Init(void)
{
	/* Reset and Disable UART */
	UART->CTRL = UART_DISABLE | UART_RESET;

	/* Reset all UART GPIOs to a known state */
	GPIO->SRC_UART[0] = UART_RX_SRC_CONST_HIGH;
	SYS_GPIO_CONFIG(UART_TX_GPIO, (GPIO_MODE_DISABLE | GPIO_NO_PULL));
	SYS_GPIO_CONFIG(UART_RX_GPIO, (GPIO_MODE_DISABLE | GPIO_NO_PULL));

	/* Clear DMA interrupt status bits */
	DMA[UART_RX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;
	DMA[UART_TX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;

	/* Enable UART */
	UART->CTRL = UART_ENABLE;

	/* Configure UART, and the RX and TX GPIOs */
	Sys_UART_GPIOConfig(UART, (GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE)
	        , UART_TX_GPIO, UART_RX_GPIO);
	Sys_UART_Config(UART, UART_CLK_HZ, BAUD_RATE,  (UART_TX_DMA_ENABLE |
	        UART_RX_DMA_ENABLE));

    /* Initialize DMA for TX channel */
    Sys_DMA_ChannelConfig(&DMA[UART_TX_DMA_NUM], DMA_TX_CONFIG, 0, 0,
                          0, (uint32_t)&UART->TX_DATA);

    Drv_Uart_rx_buffer.active = 0;
}

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
void Drv_Uart_StartSend(void *msg_p, uint_fast16_t length, bool fcs_b)
{
    uint_fast16_t cnt;
    uint_fast16_t crc;
    uint8_t      *data_p;

    if (fcs_b)
    {
        /* Select correct CRC algorithm for FCS */
    	SYS_CRC_CONFIG(CRC_CONFIG | CRC_FINAL_XOR_NON_STANDARD);
        CRC->VALUE = CRC_CCITT_INIT_VALUE;
        for (data_p = msg_p, cnt = length - CRC_CCITT_SIZE; cnt > 0; cnt--)
        {
            CRC->ADD_8 = *data_p++;
        }
        crc = CRC->FINAL;
        *data_p++ = (crc >> 0);
        *data_p   = (crc >> 8);
    }

    /* Wait for completion of previous transfer */
    while (DMA[UART_RX_DMA_NUM].CTRL == DMA_ENABLE);

    /* Clear buffer and counter, and re-enable the TX DMA for next transmission*/
    DMA[UART_TX_DMA_NUM].CFG1 = length << DMA_CFG1_TRANSFER_LENGTH_Pos;
    DMA[UART_TX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
    DMA[UART_TX_DMA_NUM].SRC_ADDR = (uint32_t)msg_p;
    DMA[UART_TX_DMA_NUM].CTRL = DMA_ENABLE;
}

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Uart_FinishSend(void)
 * ----------------------------------------------------------------------------
 * Description   : Waits for the complete transmission of a message.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */

void Drv_Uart_FinishSend(void)
{
    uint32_t cycles = 20 * SystemCoreClock / BAUD_RATE;

    /* Wait for completion of the DMA transfer */
    while (DMA[UART_TX_DMA_NUM].CTRL == DMA_ENABLE);

    /* Wait for completely shifted out the last character */
    Sys_Delay(cycles);
}

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Uart_StartRecv(uint_fast16_t length)
 * ----------------------------------------------------------------------------
 * Description   : Starts receiving a message.
 * Inputs        : length           - length of message in octets
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Uart_StartRecv(uint_fast16_t length)
{
    length += CRC_CCITT_SIZE;

    /* Start new transfer */
    DMA[UART_RX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
    Sys_DMA_ChannelConfig(&DMA[UART_RX_DMA_NUM], DMA_RX_CONFIG,
                          length, 0,
                          (uint32_t)&UART->RX_DATA,
                          (uint32_t)Drv_Uart_rx_buffer.data_a[Drv_Uart_rx_buffer.active]);
    mod_start_dma_cnt = DMA[UART_RX_DMA_NUM].CNTS;
    Sys_DMA_Mode_Enable(&DMA[UART_RX_DMA_NUM], DMA_ENABLE);

}

/* ----------------------------------------------------------------------------
 * Function      : void * Drv_Uart_FinishRecv(void)
 * ----------------------------------------------------------------------------
 * Description   : Waits for the complete reception of a message and returns
 *                 it.
 * Inputs        : None
 * Outputs       : return value     - pointer to message
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void * Drv_Uart_FinishRecv(void)
{
    uint_fast32_t tick_cnt;
    uint_fast16_t dma_cnt;
    uint_fast16_t length;
    void         *result_p;
    uint8_t      *data_p;

    length   = DMA0_CFG1[UART_RX_DMA_NUM].TRANSFER_LENGTH_SHORT;
    result_p = Drv_Uart_rx_buffer.data_a[Drv_Uart_rx_buffer.active];

    /* Wait for completion of current transfer with character timeout */
    tick_cnt = Drv_Targ_GetTicks();
    dma_cnt  = mod_start_dma_cnt;
    while (DMA[UART_RX_DMA_NUM].CTRL == DMA_ENABLE)
    {
        if (dma_cnt != DMA[UART_RX_DMA_NUM].CNTS)
        {
            dma_cnt  = DMA[UART_RX_DMA_NUM].CNTS;
            tick_cnt = Drv_Targ_GetTicks();
        }
        else if (Drv_Targ_GetTicks() - tick_cnt > MAX_CHAR_DELAY)
        {
        	DMA[UART_RX_DMA_NUM].CTRL = DMA_DISABLE;
            return NULL;
        }
    }

    /* Check FCS of received message */
    SYS_CRC_CONFIG(CRC_CONFIG);
    CRC->VALUE = CRC_CCITT_INIT_VALUE;
    for (data_p  = result_p;
         length >= sizeof(uint32_t);
         length -= sizeof(uint32_t))
    {
        CRC->ADD_32 = *(uint32_t *)data_p;
        data_p += sizeof(uint32_t);
    }
    if (length >= sizeof(uint16_t))
    {
        CRC->ADD_16 = *(uint16_t *)data_p;
        data_p += sizeof(uint16_t);
        length -= sizeof(uint16_t);
    }
    if (length > 0)
    {
        CRC->ADD_8 = *data_p;
    }
    if (CRC->FINAL != CRC_CCITT_GOOD)
    {
        return NULL;
    }

    Drv_Uart_rx_buffer.active = (Drv_Uart_rx_buffer.active + 1) % NUM_RX_BUF;
    return result_p;
}


/* ----------------------------------------------------------------------------
 * Function      : cmd_msg_t * RecvCmd(void)
 * ----------------------------------------------------------------------------
 * Description   : Receives a command message.
 * Inputs        : None
 * Outputs       : return value     - pointer to message
 * Assumptions   :
 * ------------------------------------------------------------------------- */
cmd_msg_t * RecvCmd(void)
{
    cmd_msg_t    *cmd_p;
#if (CFG_TIMEOUT > 0)
    uint_fast32_t start_tick = Drv_Targ_GetTicks();
#endif
    do
    {
        /* Feed the watchdag */
        Drv_Targ_Poll();

        /* Receive command */
        Drv_Uart_StartRecv(sizeof(*cmd_p));
        cmd_p = Drv_Uart_FinishRecv();

#if (CFG_TIMEOUT > 0)
        /* Check timeout */
        if (Drv_Targ_GetTicks() - start_tick > CFG_TIMEOUT * 1000)
        {
            Drv_Targ_Reset();
        }
#endif
    }
    while (cmd_p == NULL);

    return cmd_p;
}

/* ----------------------------------------------------------------------------
 * Function      : void SendResp(uint_fast8_t  type, err_t code)
 * ----------------------------------------------------------------------------
 * Description   : Sends the RESP message.
 * Inputs        : type             - NXT_TYPE or END_TYPE
 *                 code             - one of err_t
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void SendResp(uint_fast8_t type, err_t code)
{
    static resp_msg_t resp;

    resp.type = type;
    resp.code = (uint8_t)code;
    Drv_Uart_StartSend(&resp, sizeof(resp), UART_WITHOUT_FCS);
}

/* ----------------------------------------------------------------------------
 * Function      : void SendError(err_t error)
 * ----------------------------------------------------------------------------
 * Description   : Sends an error message.
 * Inputs        : error            - one of error_t
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void SendError(err_t error)
{
    SendResp(END_TYPE, error);
}

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
uint32_t * RecvSector(uint_fast32_t remaining_len, err_t error)
{
    uint32_t *data_p = Drv_Uart_FinishRecv();

    if (error != NO_ERROR)
    {
        SendError(error);
        return NULL;
    }

    if (data_p != NULL && remaining_len > 0)
    {
        SendResp(NXT_TYPE, NO_ERROR);

        /* Prepare receiving the next sector */
        Drv_Uart_StartRecv(MIN(remaining_len, FLASH_SECTOR_SIZE));
    }
    return data_p;
}

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
err_t Verify(uint_fast32_t adr, const uint32_t *data_p, uint_fast32_t length)
{
	uint_fast32_t len;
    for (len = length; len > 0; len -= sizeof(*data_p))
    {
        if (*data_p != *(uint32_t *)adr)
        {
            return VERIFY_FLASH_FAILED;
        }
        CRC->ADD_32 = *data_p++;                /* Update Hash */
        adr += sizeof(*data_p);
    }
    return NO_ERROR;
}

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
err_t ProgFlash(uint_fast32_t adr, const uint32_t *data_p, uint_fast32_t len)
{
    FlashStatus_t status;

    status = Flash_WriteBuffer(adr, len / sizeof(unsigned int),data_p, false);
    if (status != FLASH_ERR_NONE)
    {
        return INVALID_CMD + status;
    }
    return Verify(adr, data_p, len);
}

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
err_t ProgSector(uint_fast32_t adr, const uint32_t *data_p, uint_fast16_t sector_len)
{
    FlashStatus_t status;

    status = Flash_EraseSector(adr,false);
    if (status != FLASH_ERR_NONE)
    {
        return INVALID_CMD + status;
    }
    return ProgFlash(adr, data_p, sector_len);
}

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
bool CopyVersionInfo(Sys_Boot_app_version_t *buffer_p,
                            uint_fast32_t image_adr)
{
    const Sys_Boot_app_version_t *version_p;
    BootROMStatus status;

    if (image_adr != 0)
    {
        /* Check if a valid application is installed already */
        status = Sys_ValidateApp((uint32_t *)image_adr);

        /* We do not use a CRC in the image header,
         * therefore BOOTROM_ERR_BAD_CRC is OK too */
        if (status == BOOTROM_ERR_NONE || status == BOOTROM_ERR_BAD_CRC)
        {
            version_p = Bootloader_get_image_version(image_adr);
            if (version_p != NULL)
            {
                *buffer_p = *version_p;
            }
            else
            {
                memcpy(buffer_p, "??????", sizeof(buffer_p->id));
            }
            return true;
        }
    }
    return false;
}

/* ----------------------------------------------------------------------------
 * Function      : void ProcessHello(void)
 * ----------------------------------------------------------------------------
 * Description   : Processes the HELLO command.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessHello(void)
{
    static hello_resp_msg_t hello;
    uint_fast16_t size = sizeof(hello);

    memset(&hello, 0, sizeof(hello));
    CopyVersionInfo(&hello.boot_ver, BOOT_BASE_ADR);
    hello.sector_size = FLASH_SECTOR_SIZE;
    Drv_Uart_StartSend(&hello, size, UART_WITH_FCS);
}

/* ----------------------------------------------------------------------------
 * Function      : void ProcessProg(prog_cmd_arg_t *arg_p)
 * ----------------------------------------------------------------------------
 * Description   : Processes the PROG command.
 * Inputs        : arg_p            - pointer to PROG command message
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessProg(prog_cmd_arg_t *arg_p)
{
    uint_fast32_t current_adr   = arg_p->adr;
    uint_fast32_t remaining_len = arg_p->length;
    uint_fast32_t sector_len    = MIN(remaining_len, FLASH_SECTOR_SIZE);
    err_t resp_code = NO_ERROR;
    image_dscr_t image;
    uint32_t    *data_p;

    /* Check start address and length of image */
    if ((arg_p->adr != DOWNLOAD_BASE_ADDR && arg_p->adr != BOOT_BASE_ADR)              ||
        arg_p->adr + arg_p->length                 > DOWNLOAD_BASE_ADDR + APP_MAX_SIZE ||
        arg_p->adr    % FLASH_SECTOR_SIZE         != 0                           ||
        arg_p->length % (2 * sizeof(uint32_t))    != 0                           ||
        arg_p->length                              < APP_MIN_SIZE)
    {
        SendError(INVALID_CMD);
        return;
    }
    image.prop = *arg_p;

    /* Prepare receiving the 1st sector */
    SendResp(NXT_TYPE, NO_ERROR);
    Drv_Uart_StartRecv(sector_len);

    /* Process image */
    while (remaining_len > 0)
    {
        remaining_len -= sector_len;

        /* Feed Watchdog */
        Drv_Targ_Poll();

        /* Wait for next image sector */
        data_p = RecvSector(remaining_len, resp_code);
        if (data_p == NULL)
        {
            return;
        }

        SYS_CRC_CONFIG(CRC32_CONFIG);
        if (current_adr == image.prop.adr)
        {
            /* Save image header */
            CRC->VALUE = CRC_32_INIT_VALUE;     /* Init Hash */
            //SaveHeader(&image, data_p, sector_len);
            image.crc = CRC->VALUE;             /* Store Hash for next sector */

            /* Program 1st image sector */
            resp_code = ProgSector(current_adr, data_p, sector_len);
        }
        else
        {
            /* Program next image sector */
            CRC->VALUE = image.crc;             /* Restore Hash */
            if (Verify(current_adr, data_p, sector_len) != NO_ERROR)
            {
                CRC->VALUE = image.crc;         /* Reset Hash */
                resp_code  = ProgSector(current_adr, data_p, sector_len);
            }
            image.crc = CRC->VALUE;             /* Store Hash for next sector */
        }

        current_adr += sector_len;
        sector_len   = MIN(remaining_len, FLASH_SECTOR_SIZE);
    }
    SendResp(END_TYPE, resp_code);

    /* Program saved image header */
    //ProgHeader(&image, resp_code);
}

#if (CFG_READ_SUPPORT)
/* ----------------------------------------------------------------------------
 * Function      : void ProcessRead(cmd_msg_t *cmd_p)
 * ----------------------------------------------------------------------------
 * Description   : Processes the READ command.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessRead(cmd_msg_t *cmd_p)
{
    const void   *src_p  = (const void *)cmd_p->arg.read.adr;
    uint_fast32_t length = cmd_p->arg.read.length;

    /* we recycle the input buffer as output buffer */
    uint8_t      *resp_p = (uint8_t *)cmd_p;

    if (length == 0 || length > FLASH_SECTOR_SIZE)
    {
        SendError(INVALID_CMD);
    }
    else
    {
        memcpy(resp_p, src_p, length);
        length += sizeof(Drv_Uart_fcs_t);
        Drv_Uart_StartSend(resp_p, length, UART_WITH_FCS);
    }
}

#endif /* if (CFG_READ_SUPPORT) */

/* ----------------------------------------------------------------------------
 * Function      : void ProcessRestart(void)
 * ----------------------------------------------------------------------------
 * Description   : Processes the RESTART command.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessRestart(void)
{
    BootROMStatus status;

    /* Check if the BootLoader is intact */
    status = Sys_ValidateApp((uint32_t *)BOOT_BASE_ADR);

    /* We do not use a CRC in the image header,
     * therefore BOOTROM_ERR_BAD_CRC is OK too */
    if (status == BOOTROM_ERR_NONE || status == BOOTROM_ERR_BAD_CRC)
    {
        SendResp(END_TYPE, NO_ERROR);

        /* Wait for completely sent response */
        Drv_Uart_FinishSend();

    }
    if (verify_boot_images() == BOOTLOADER_IMAGE_STATUS_VALID)
    {
    	Sys_StartApp((uint32_t *)APP_BASE_ADDR);
    }

    SendError(NO_VALID_BOOTLOADER);
}




