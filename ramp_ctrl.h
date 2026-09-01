/******************************************************************************
* File Name:   ramp_ctrl.h
*
* Description: This file implements the ramp control module.
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

#ifndef RAMP_CTRL_H_
#define RAMP_CTRL_H_

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/**********************************************************************************************************************
 * MACROS
 **********************************************************************************************************************/


/**********************************************************************************************************************
* ENUMS
**********************************************************************************************************************/


/**********************************************************************************************************************
* DATA STRUCTURES
**********************************************************************************************************************/
/**
 * This structure holds the ramp controller data.
 */
typedef struct
{
    uint16_t RampDelayMax;     // Parameter: Maximum delay rate
    uint16_t RampLowLimit;     // Parameter: Minimum limit
    uint16_t RampHighLimit;    // Parameter: Maximum limit
    uint16_t RampDelayCount;   // Variable: Incremental delay
    uint16_t SetpointValue;    // Output: Target output
    int16_t  Tmp;              // Variable: Temp variable
    bool     disable_flag;     // false: enable ramp, true: Disable ramp
} RAMP_CNTL_t;

/***********************************************************************************************************************
 * API Prototypes
 **********************************************************************************************************************/
/**
 * @brief RAMP Controller initialization. Each motor instance owns its own RAMP_CNTL_t handle.
 * @param hptr Handle of the ramp controller module
 * @param rampDelayMax Maximum delay rate between setpoint steps
 * @param rampLowLimit Minimum output limit
 * @param rampHighLimit Maximum output limit
 * @param initialSetpoint Setpoint value to start from
 * @return None<BR>
 */
static inline void RAMP_Init(RAMP_CNTL_t* const hptr, uint16_t rampDelayMax, uint16_t rampLowLimit,
                              uint16_t rampHighLimit, uint16_t initialSetpoint)
{
    hptr->RampDelayMax   = rampDelayMax;
    hptr->RampLowLimit   = rampLowLimit;
    hptr->RampHighLimit  = rampHighLimit;
    hptr->RampDelayCount = 0u;
    hptr->SetpointValue  = initialSetpoint;
    hptr->Tmp            = 0;
    hptr->disable_flag   = false;
}

/**
 * @brief Advance the handle toward TargetValue by one step when the delay expires.
 * @param hptr Handle of the ramp controller module
 * @param TargetValue Requested output value
 * @return Current ramp output value
 */
static inline uint16_t RAMP_Controller(RAMP_CNTL_t* const hptr, uint16_t TargetValue)
{
    hptr->Tmp = (TargetValue - hptr->SetpointValue);

    if (hptr->Tmp != 0)
    {
        hptr->RampDelayCount++;

        if (hptr->RampDelayCount >= hptr->RampDelayMax)
        {
            if (TargetValue >= hptr->SetpointValue)
            {
                if (hptr->disable_flag == false)
                    hptr->SetpointValue += 1;
            }
            else
            {
                if (hptr->disable_flag == false)
                    hptr->SetpointValue -= 1;
            }

            /*Limit the set point value*/
            if(hptr->SetpointValue < hptr->RampLowLimit)
            {
                hptr->SetpointValue = hptr->RampLowLimit;
            }
            else if(hptr->SetpointValue > hptr->RampHighLimit)
            {
                hptr->SetpointValue = hptr->RampHighLimit;
            }

            hptr->RampDelayCount = 0;
        }
    }
    return (hptr->SetpointValue);
}

#endif //End of RAMP_CTRL_H_
