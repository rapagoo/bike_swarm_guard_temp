#include "vs1003b.h"

#include "main.h"

#include <stdbool.h>
#include <stddef.h>

#define VS1003B_SCI_READ_OPCODE 0x03U
#define VS1003B_SCI_WRITE_OPCODE 0x02U
#define VS1003B_SCI_MODE_ADDRESS 0x00U
#define VS1003B_SCI_MODE_RESET_VALUE 0x0800U

#define VS1003B_RESET_LOW_MS 2U
#define VS1003B_DREQ_TIMEOUT_MS 100U
#define VS1003B_SPI_TIMEOUT_MS 10U

static SPI_HandleTypeDef *vs1003b_spi = NULL;

bool vs1003b_is_ready(void)
{
    return HAL_GPIO_ReadPin(VS_DREQ_GPIO_Port, VS_DREQ_Pin) == GPIO_PIN_SET;
}

static bool vs1003b_wait_ready(uint32_t timeout_ms)
{
    uint32_t started_at_ms = HAL_GetTick();

    while (!vs1003b_is_ready())
    {
        if ((uint32_t)(HAL_GetTick() - started_at_ms) >= timeout_ms)
        {
            return false;
        }
    }

    return true;
}

static void vs1003b_hardware_reset(void)
{
    /* 리셋 중에는 두 SPI 선택 신호를 모두 비활성(High)으로 유지합니다. */
    HAL_GPIO_WritePin(VS_XCS_GPIO_Port, VS_XCS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(VS_XDCS_GPIO_Port, VS_XDCS_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(VS_RST_GPIO_Port, VS_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(VS1003B_RESET_LOW_MS);
    HAL_GPIO_WritePin(VS_RST_GPIO_Port, VS_RST_Pin, GPIO_PIN_SET);
}

vs1003b_status_t vs1003b_read_register(uint8_t address, uint16_t *value)
{
    uint8_t tx_data[4] = {
        VS1003B_SCI_READ_OPCODE,
        address,
        0xFFU,
        0xFFU,
    };
    uint8_t rx_data[4] = {0U};

    if ((vs1003b_spi == NULL) || (value == NULL))
    {
        return VS1003B_STATUS_INVALID_ARGUMENT;
    }

    if (!vs1003b_wait_ready(VS1003B_DREQ_TIMEOUT_MS))
    {
        return VS1003B_STATUS_DREQ_TIMEOUT;
    }

    HAL_GPIO_WritePin(VS_XCS_GPIO_Port, VS_XCS_Pin, GPIO_PIN_RESET);

    HAL_StatusTypeDef hal_status = HAL_SPI_TransmitReceive(
        vs1003b_spi,
        tx_data,
        rx_data,
        sizeof(tx_data),
        VS1003B_SPI_TIMEOUT_MS
    );

    HAL_GPIO_WritePin(VS_XCS_GPIO_Port, VS_XCS_Pin, GPIO_PIN_SET);

    if (hal_status != HAL_OK)
    {
        return VS1003B_STATUS_SPI_ERROR;
    }

    *value = ((uint16_t)rx_data[2] << 8U) | rx_data[3];
    return VS1003B_STATUS_OK;
}

vs1003b_status_t vs1003b_write_register(uint8_t address, uint16_t value)
{
    uint8_t tx_data[4] = {
        VS1003B_SCI_WRITE_OPCODE,
        address,
        (uint8_t)(value >> 8U),
        (uint8_t)value,
    };

    if (vs1003b_spi == NULL)
    {
        return VS1003B_STATUS_INVALID_ARGUMENT;
    }

    if (!vs1003b_wait_ready(VS1003B_DREQ_TIMEOUT_MS))
    {
        return VS1003B_STATUS_DREQ_TIMEOUT;
    }

    HAL_GPIO_WritePin(VS_XCS_GPIO_Port, VS_XCS_Pin, GPIO_PIN_RESET);

    HAL_StatusTypeDef hal_status = HAL_SPI_Transmit(
        vs1003b_spi,
        tx_data,
        sizeof(tx_data),
        VS1003B_SPI_TIMEOUT_MS
    );

    HAL_GPIO_WritePin(VS_XCS_GPIO_Port, VS_XCS_Pin, GPIO_PIN_SET);

    if (hal_status != HAL_OK)
    {
        return VS1003B_STATUS_SPI_ERROR;
    }

    /*
     * 마지막 SCLK 직후 DREQ가 Low로 전환되기 전에 이전 High를 읽는 경쟁을 피합니다.
     * 특히 SCI_CLOCKF는 내부 클럭이 바뀌므로 짧은 안정화 시간을 둡니다.
     */
    HAL_Delay(1U);

    if (!vs1003b_wait_ready(VS1003B_DREQ_TIMEOUT_MS))
    {
        return VS1003B_STATUS_DREQ_TIMEOUT;
    }

    return VS1003B_STATUS_OK;
}

vs1003b_status_t vs1003b_init(SPI_HandleTypeDef *hspi, uint16_t *mode_value)
{
    vs1003b_status_t status;

    if ((hspi == NULL) || (mode_value == NULL))
    {
        return VS1003B_STATUS_INVALID_ARGUMENT;
    }

    vs1003b_spi = hspi;
    vs1003b_hardware_reset();

    if (!vs1003b_wait_ready(VS1003B_DREQ_TIMEOUT_MS))
    {
        return VS1003B_STATUS_DREQ_TIMEOUT;
    }

    status = vs1003b_read_register(VS1003B_SCI_MODE_ADDRESS, mode_value);
    if (status != VS1003B_STATUS_OK)
    {
        return status;
    }

    if (*mode_value != VS1003B_SCI_MODE_RESET_VALUE)
    {
        return VS1003B_STATUS_MODE_MISMATCH;
    }

    return VS1003B_STATUS_OK;
}
