/******************************************************************************
* File Name:   motif_hall_drive.h
*
* Description: Instance-based MOTIF Hall-sensor commutation driver. Each motor
*              on the board owns one MOTIF_HALL_DRIVE_t handle bound to its
*              own PWM/MOTIF/pattern-update-timer hardware and ramp state, so
*              the same driver code can run several motor instances.
*
* Related Document: See README.md
*
*
*******************************************************************************
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

#ifndef MOTIF_HALL_DRIVE_H_
#define MOTIF_HALL_DRIVE_H_

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "cy_pdl.h"
#include "ramp_ctrl.h"

/**********************************************************************************************************************
 * MACROS
 **********************************************************************************************************************/
/* Number of compare counts below pwmPeriod accepted as zero-speed command to trigger Hall re-synchronization. */
#define MOTIF_HALL_STANDSTILL_GUARD_COUNTS (100u)

/**********************************************************************************************************************
* DATA STRUCTURES
**********************************************************************************************************************/
/**
 * Single per-motor handle containing immutable board configuration and mutable runtime state.
 * Initialize the configuration fields once for each motor; the driver updates only the
 * runtime fields after MotifHallDrive_HwInit() has brought up the hardware.
 */
typedef struct
{
    /* Device/board configuration for this motor instance */
    TCPWM_Type                            *pwmHw;
    uint32_t                               pwmUNum;
    uint32_t                               pwmVNum;
    uint32_t                               pwmWNum;
    const cy_stc_tcpwm_pwm_config_t       *pwmUConfig;
    const cy_stc_tcpwm_pwm_config_t       *pwmVConfig;
    const cy_stc_tcpwm_pwm_config_t       *pwmWConfig;
    uint32_t                               pattUpdateTimerNum;
    const cy_stc_tcpwm_counter_config_t   *pattUpdateTimerConfig;
    const cy_stc_sysint_t                 *pattUpdateIntrCfg;
    cy_israddress                          pattUpdateIsr;
    TCPWM_MOTIF_GRP_MOTIF_Type            *motifHw;
    const cy_stc_tcpwm_motif_hall_sensor_config_t *motifHallConfig;
    cy_stc_tcpwm_motif_mlut_t              *hallMlut;
    const cy_stc_sysint_t                 *motifIntrCfg;
    cy_israddress                          motifIsr;

    /* Per-motor control parameters and runtime state */
    uint16_t                               pwmPeriod;
    RAMP_CNTL_t                            ramp;
    volatile bool                          motorFault;
    volatile bool                          hallResyncPending;
} MOTIF_HALL_DRIVE_t;

/***********************************************************************************************************************
 * API Prototypes
 **********************************************************************************************************************/
/**
 * @brief Initialize the mutable state of a configured drive instance. Pure state init: no
 *        register access. The hardware/configuration fields must be initialized by the caller.
 * @param drv Handle of the drive instance
 * @param pwmPeriod PWM period (also the "zero speed" duty command)
 * @param rampDelayMax Ramp rate applied to the duty command
 */
static inline void MotifHallDrive_Init(MOTIF_HALL_DRIVE_t *const drv,
                                        uint16_t pwmPeriod, uint16_t rampDelayMax)
{
    drv->pwmPeriod           = pwmPeriod;
    drv->motorFault          = false;
    drv->hallResyncPending   = false;

    RAMP_Init(&drv->ramp, rampDelayMax, 0u, pwmPeriod, pwmPeriod);
}

/**
 * @brief Bring up the configured TCPWM PWM/counter/MOTIF hardware and initialize one motor handle.
 *        Call it once for each motor instance before enabling global interrupts.
 * @param drv Handle of the drive instance
 * @param rampDelayMax Ramp rate applied to the duty command
 */
static inline void MotifHallDrive_HwInit(MOTIF_HALL_DRIVE_t *const drv,
                                          uint16_t rampDelayMax)
{
    Cy_TCPWM_PWM_Init(drv->pwmHw, drv->pwmUNum, drv->pwmUConfig);
    Cy_TCPWM_PWM_Enable(drv->pwmHw, drv->pwmUNum);
    Cy_TCPWM_PWM_Init(drv->pwmHw, drv->pwmVNum, drv->pwmVConfig);
    Cy_TCPWM_PWM_Enable(drv->pwmHw, drv->pwmVNum);
    Cy_TCPWM_PWM_Init(drv->pwmHw, drv->pwmWNum, drv->pwmWConfig);
    Cy_TCPWM_PWM_Enable(drv->pwmHw, drv->pwmWNum);

    Cy_TCPWM_Counter_Init(drv->pwmHw, drv->pattUpdateTimerNum, drv->pattUpdateTimerConfig);
    Cy_TCPWM_Counter_Enable(drv->pwmHw, drv->pattUpdateTimerNum);
    Cy_SysInt_Init(drv->pattUpdateIntrCfg, drv->pattUpdateIsr);
    NVIC_EnableIRQ(drv->pattUpdateIntrCfg->intrSrc);

    Cy_TCPWM_MOTIF_Hall_Sensor_Init(drv->motifHw, drv->motifHallConfig);
    Cy_TCPWM_MOTIF_Enable(drv->motifHw);
    Cy_TCPWM_MOTIF_Update_MLUT(drv->motifHw, drv->hallMlut);
    Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern_Immediately(drv->motifHw);
    Cy_TCPWM_MOTIF_SetInterruptMask(drv->motifHw, CY_MOTIF_CORRECT_HALL_EVENT | CY_MOTIF_WRONG_HALL_EVENT |
                                                  CY_MOTIF_PREVIOUS_PATTERN_MATCH_EVENT);
    Cy_SysInt_Init(drv->motifIntrCfg, drv->motifIsr);
    NVIC_EnableIRQ(drv->motifIntrCfg->intrSrc);

    MotifHallDrive_Init(drv, drv->pwmUConfig->period0, rampDelayMax);
}

/**
 * @brief Force a Hall re-sample; the instance's pattern-update timer times the settle
 *        delay before MotifHallDrive_PatternUpdateIsr() applies it via Start().
 * @param drv Handle of the drive instance
 */
static inline void MotifHallDrive_RequestResync(MOTIF_HALL_DRIVE_t *const drv)
{
    if (!drv->hallResyncPending)
    {
        Cy_TCPWM_MOTIF_Hall_Input_Sample_Trigger(drv->motifHw);
        drv->hallResyncPending = true;
        Cy_TCPWM_TriggerStart_Single(drv->pwmHw, drv->pattUpdateTimerNum);
    }
}

/**
 * @brief Call from this instance's pattern-update-timer interrupt handler.
 * @param drv Handle of the drive instance
 */
static inline void MotifHallDrive_PatternUpdateIsr(MOTIF_HALL_DRIVE_t *const drv)
{
    uint32_t interrupts = Cy_TCPWM_GetInterruptStatusMasked(drv->pwmHw, drv->pattUpdateTimerNum);
    Cy_TCPWM_ClearInterrupt(drv->pwmHw, drv->pattUpdateTimerNum, interrupts);

    if (drv->hallResyncPending)
    {
        /* Hall lines have settled since the forced sample: apply the now-current commutation vector */
        drv->hallResyncPending = false;
        Cy_TCPWM_MOTIF_Start(drv->motifHw);
    }
}

static inline void MotifHallDrive_TripFault(MOTIF_HALL_DRIVE_t *const drv)
{
    drv->motorFault = true;
    drv->ramp.SetpointValue = drv->pwmPeriod;
    drv->ramp.RampDelayCount = 0u;
    /* MOTIF holds the last commutation vector when it self-stops, so force the safe MCPF.MCFV pattern first. */
    Cy_TCPWM_MOTIF_Clear_Multi_Channel_Pattern(drv->motifHw);
    Cy_TCPWM_MOTIF_Stop(drv->motifHw);
    Cy_TCPWM_PWM_Disable(drv->pwmHw, drv->pwmUNum);
    Cy_TCPWM_PWM_Disable(drv->pwmHw, drv->pwmVNum);
    Cy_TCPWM_PWM_Disable(drv->pwmHw, drv->pwmWNum);
    Cy_TCPWM_PWM_SetCompare0BufVal(drv->pwmHw, drv->pwmUNum, drv->pwmPeriod);
    Cy_TCPWM_PWM_SetCompare0BufVal(drv->pwmHw, drv->pwmVNum, drv->pwmPeriod);
    Cy_TCPWM_PWM_SetCompare0BufVal(drv->pwmHw, drv->pwmWNum, drv->pwmPeriod);
}

/**
 * @brief Call from this instance's MOTIF Hall-event interrupt handler.
 *        In LUT mode a reversed rotor reports the previous-pattern match, not a wrong-Hall
 *        event; both mean the commutation sequence is broken, so both force a safe stop.
 * @param drv Handle of the drive instance
 */
static inline void MotifHallDrive_HallEventIsr(MOTIF_HALL_DRIVE_t *const drv)
{
    uint32_t intr = Cy_TCPWM_MOTIF_GetInterruptStatus(drv->motifHw);
    Cy_TCPWM_MOTIF_ClearInterrupt(drv->motifHw, intr);

    if (intr & (CY_MOTIF_WRONG_HALL_EVENT | CY_MOTIF_PREVIOUS_PATTERN_MATCH_EVENT))
    {
        /* Stop before any further commutation can apply current to an invalid Hall state. */
        MotifHallDrive_TripFault(drv);
    }
}

/**
 * @brief Run one control-loop step: ramp toward targetSpeed, manage Hall re-sync/fault, and
 *        apply the resulting duty to the three phase PWM compare registers.
 * @param drv Handle of the drive instance
 * @param targetSpeed Desired duty command (same units as pwmPeriod; pwmPeriod itself means "standstill")
 * @return The duty value written to the PWM compare registers
 */
static inline uint16_t MotifHallDrive_Update(MOTIF_HALL_DRIVE_t *const drv, uint16_t targetSpeed)
{
    RAMP_Controller(&drv->ramp, targetSpeed);
    uint16_t duty_cycle = drv->ramp.SetpointValue;
    uint16_t standstill_threshold = (drv->pwmPeriod > MOTIF_HALL_STANDSTILL_GUARD_COUNTS) ?
                                    (drv->pwmPeriod - MOTIF_HALL_STANDSTILL_GUARD_COUNTS) : 0u;
    bool at_standstill_command = (duty_cycle > standstill_threshold);

    if (drv->motorFault)
    {
        /* Stay in the safe state until the external fault-release interrupt clears the latch. */
        duty_cycle = drv->pwmPeriod;
    }
    else if (at_standstill_command)
    {
        /* Keep re-syncing to the (static) Hall pattern while the drive is commanded to standstill */
        MotifHallDrive_RequestResync(drv);
    }

    Cy_TCPWM_PWM_SetCompare0BufVal(drv->pwmHw, drv->pwmUNum, duty_cycle);
    Cy_TCPWM_PWM_SetCompare0BufVal(drv->pwmHw, drv->pwmVNum, duty_cycle);
    Cy_TCPWM_PWM_SetCompare0BufVal(drv->pwmHw, drv->pwmWNum, duty_cycle);

    return duty_cycle;
}

#endif /* MOTIF_HALL_DRIVE_H_ */
