/*******************************************************************************
* File Name:   main.c
*
* Description: This code example demonstrates the use of MOTIF IP in Hall sensor
*              mode by controlling the BLDC motor in trapezoidal control.
*
* Related Document: See README.md
*
*
********************************************************************************
* (c) 2024-2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cy_pdl.h"
#include "mtb_hal.h"
#include "cybsp.h"
#include "motif_hall_drive.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define RAMP_RATE (400u)
/* Consecutive wrong-Hall events tolerated before a safe stop is forced */
/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void motif_che_handler(void);
void pattern_update_handler(void);
void n_fault_hw_handler(void);

/*******************************************************************************
* Global Variables
*******************************************************************************/
const cy_stc_sysint_t che_intrCfg =
{
    .intrSrc      = tcpwm_0_motif_interrupt_64_IRQn,
    .intrPriority = 1u
};

const cy_stc_sysint_t PattUpdate_intrCfg =
{
    .intrSrc = PattUpdate_Timer_IRQ,
    .intrPriority = 1u
};

const cy_stc_sysint_t n_fault_hw_intrCfg =
{
    .intrSrc = N_FAULT_HW_IRQ,
    .intrPriority = 0u
};

/* One handle per motor; each handle contains its device configuration and runtime state. */
MOTIF_HALL_DRIVE_t motor0 =
{
    .pwmHw                 = PWM_U_HW,
    .pwmUNum               = PWM_U_NUM,
    .pwmVNum               = PWM_V_NUM,
    .pwmWNum               = PWM_W_NUM,
    .pwmUConfig            = &PWM_U_config,
    .pwmVConfig            = &PWM_V_config,
    .pwmWConfig            = &PWM_W_config,
    .pattUpdateTimerNum    = PattUpdate_Timer_NUM,
    .pattUpdateTimerConfig = &PattUpdate_Timer_config,
    .pattUpdateIntrCfg     = &PattUpdate_intrCfg,
    .pattUpdateIsr         = pattern_update_handler,
    .motifHw               = MOTIF0_HW,
    .motifHallConfig       = &MOTIF0_hall_config,
    .hallMlut              = &hall_mlut_config,
    .motifIntrCfg          = &che_intrCfg,
    .motifIsr              = motif_che_handler,
};

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CPU.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif /* #if defined (CY_DEVICE_SECURE) */

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    Cy_HPPASS_AC_Start(0,1000u); //HPPASS (ADC Start)

    /* All PWM/counter/MOTIF register bring-up lives behind this call. */
    MotifHallDrive_HwInit(&motor0, RAMP_RATE);
    Cy_SysInt_Init(&n_fault_hw_intrCfg, n_fault_hw_handler);
    Cy_GPIO_ClearInterrupt(N_FAULT_HW_PORT, N_FAULT_HW_NUM);
    NVIC_EnableIRQ(n_fault_hw_intrCfg.intrSrc);

    /* Yellow indicates the example is active; red is reserved for a latched fault. */
    Cy_GPIO_Write(DIR_LED_PORT, DIR_LED_NUM, true);
    Cy_GPIO_Write(FAULT_LED_ALL_PORT, FAULT_LED_ALL_NUM, false);

    /* Enable global interrupts */
    __enable_irq();

//    Cy_GPIO_Write(POW_EN_PORT, POW_EN_NUM ,1);
    /* A stationary rotor never produces the edge MOTIF otherwise waits for, so the first
     * commutation vector can be loaded from a stale Hall pattern and stall at high current.
     * PattUpdate_Timer's one-shot delay (see pattern_update_handler) gates Start() until the
     * forced sample below has settled. */
    MotifHallDrive_RequestResync(&motor0);

    for (;;)
    {
        /* Read the potentiometer (AN_B4) Value and control the target speed */
        uint16_t target_speed = motor0.pwmUConfig->period0 - ((Cy_HPPASS_SAR_Result_ChannelRead(CY_HPPASS_SAR_CHAN_12_IDX) * motor0.pwmUConfig->period0)>>12u);

        MotifHallDrive_Update(&motor0, target_speed);
        Cy_GPIO_Write(DIR_LED_PORT, DIR_LED_NUM, true);
        Cy_GPIO_Write(FAULT_LED_ALL_PORT, FAULT_LED_ALL_NUM, motor0.motorFault);
    }
}


void motif_che_handler(void)
{
    MotifHallDrive_HallEventIsr(&motor0);
}

void pattern_update_handler(void)
{
    MotifHallDrive_PatternUpdateIsr(&motor0);
}

void n_fault_hw_handler(void)
{
    Cy_GPIO_ClearInterrupt(N_FAULT_HW_PORT, N_FAULT_HW_NUM);
    MotifHallDrive_TripFault(&motor0);
}

/* [] END OF FILE */
