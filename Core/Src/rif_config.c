/**
 ******************************************************************************
 * @file           : rif_config.c
 * @brief          : RIF (Resource Isolation Framework) security configuration
 *                   Extracted from main.c SystemIsolation_Config()
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "rif_config.h"

/**
 * @brief  Configure RIF security attributes for all peripherals and GPIO pins.
 * @note   Must be called after all MX_*_Init() and before peripheral usage.
 */
void rif_config(void)
{
  /* USER CODE BEGIN RIF_Init 0 */

  /* USER CODE END RIF_Init 0 */

  /* set all required IPs as secure privileged */
  __HAL_RCC_RIFSC_CLK_ENABLE();
  RIMC_MasterConfig_t RIMC_master = {0};
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;

  /*====================== RIMC configuration ======================*/
  /* All DMA-capable bus masters must be registered here */
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC1, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC2, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DCMIPP,
                                      &RIMC_master); /* New: camera DMA */
  // HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_XSPI1, &RIMC_master);
  // /* New: XSPI1 storage controller */

  /*====================== RISUP configuration =====================*/
  /* All peripherals accessed by CPU/DMA must be registered here */
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DMA2D,
                                        RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL1,
                                        RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL2,
                                        RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DCMIPP,
                                        RIF_ATTRIBUTE_SEC |
                                            RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_XSPI1,
                                        RIF_ATTRIBUTE_SEC |
                                            RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_USART3,
                                        RIF_ATTRIBUTE_SEC |
                                            RIF_ATTRIBUTE_PRIV);

  /*=================== RIF-Aware IPs Config =======================*/

  /* set up PWR configuration */
  HAL_PWR_ConfigAttributes(PWR_ITEM_0, PWR_SEC_NPRIV);

  /*=============== GPIO Pin Security Attributes ===================*/

  /* --- Original: General output control pins --- */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_3,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_10,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_10,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);

  /* --- Original: XSPI1 all pins (Port O + Port P) --- */
  HAL_GPIO_ConfigPinAttributes(GPIOO, GPIO_PIN_0,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO, GPIO_PIN_2,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO, GPIO_PIN_4,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO, GPIO_PIN_5,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_0,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_1,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_2,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_3,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_4,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_5,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_6,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP, GPIO_PIN_7,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV);

  /* --- Original: DCMIPP partial data pins (7 configured) --- */
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_7,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D0  */
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_6,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D1  */
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_0,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D2  */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_9,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D3  */
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_5,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D5  */
  HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_9,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D6  */

  /*
   * Note: PB9 appears twice in original code (duplicate), and
   * D4/HSYNC/PIXCLK/VSYNC/D7 were missing.  Added below:
   */
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_8,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D4     */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_7,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* D7     */
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_0,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* HSYNC  */
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_5,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* PIXCLK */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_8,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* VSYNC  */

  /* --- New: LTDC all RGB565/666 interface pins --- */
  /* GPIOA: R5, B4, G2, B5, B3, B7, B6, G3, CLK */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_15,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* R5  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_10,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* B4  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_1,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* G2  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_9,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* B5  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_11,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* B3  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_2,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* B7  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_8,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* B6  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_0,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* G3  */
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_5,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* CLK */
  /* GPIOB: G4, G6, G5, R3, G7 */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_15,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* G4  */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_11,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* G6  */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_12,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* G5  */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_4,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* R3  */
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_10,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* G7  */
  /* GPIOF: R6, HSYNC */
  HAL_GPIO_ConfigPinAttributes(GPIOF, GPIO_PIN_8,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* R6    */
  HAL_GPIO_ConfigPinAttributes(GPIOF, GPIO_PIN_9,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* HSYNC */
  /* GPIOG: VSYNC, R7, DE */
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_0,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* VSYNC */
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_9,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* R7    */
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_13,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* DE    */
  /* GPIOH: R4 */
  HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_4,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* R4    */

  /* --- New: USART3 pins --- */
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_1,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* TX */
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_9,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* RX */

  /* --- New: Camera software I2C pins (configured in MX_GPIO_Init) --- */
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_13,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* SCL */
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_14,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* SDA */

  /* --- New: Other GPIO output control pins --- */
  HAL_GPIO_ConfigPinAttributes(GPIOQ, GPIO_PIN_2,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* PQ2 */
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_14,
                               GPIO_PIN_SEC | GPIO_PIN_NPRIV); /* PG14 */

  /* USER CODE BEGIN RIF_Init 1 */

  /* USER CODE END RIF_Init 1 */

  /* USER CODE BEGIN RIF_Init 2 */

  /* USER CODE END RIF_Init 2 */
}
