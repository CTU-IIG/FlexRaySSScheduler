/*
 * File: fray_basic_demo_rpp.h
 *
 * Target selection: rpp.tlc
 * Embedded hardware selection: 32-bit Generic
 */

#ifndef RTW_HEADER_fray_basic_demo_rpp_h_
#define RTW_HEADER_fray_basic_demo_rpp_h_
#ifndef fray_basic_demo_rpp_COMMON_INCLUDES_
# define fray_basic_demo_rpp_COMMON_INCLUDES_

#include <base.h>
#include <rpp/sci.h>
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#include "rpp/rpp.h"

#endif                                 /* fray_basic_demo_rpp_COMMON_INCLUDES_ */

#include "fray_basic_demo_rpp_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((void*) 0)
#endif

/* Block signals (auto storage) */
typedef struct {
    uint8_T ConfigureRXbuffer_o2[6];
    /* '<Root>/Configure RX buffer' */
    uint8_T ConfigureRXbuffer_o3;
    /* '<Root>/Configure RX buffer' */
    boolean_T ConfigureFlexRaynode1;
    /* '<Root>/Configure FlexRay node1' */
    boolean_T ConfigureTXbuffer;
    /* '<Root>/Configure TX buffer' */
    boolean_T ConfigureRXbuffer_o1;
    /* '<Root>/Configure RX buffer' */
    boolean_T ConfigureRXbuffer_o4;
    /* '<Root>/Configure RX buffer' */
    boolean_T SerialSend;
    /* '<Root>/Serial Send' */
    boolean_T SerialSend1;
    /* '<Root>/Serial Send1' */
    boolean_T SerialSend2;
    /* '<Root>/Serial Send2' */
    boolean_T SerialSend3;
    /* '<Root>/Serial Send3' */
    boolean_T SerialSend4;
    /* '<Root>/Serial Send4' */
    boolean_T SerialSend5;               /* '<Root>/Serial Send5' */
} BlockIO_fray_basic_demo_rpp;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
    boolean_T Out1;                      /* '<Root>/Out1' */
} ExternalOutputs_fray_basic_demo;

/* Parameters (auto storage) */
struct Parameters_fray_basic_demo_rpp_ {
    uint8_T Constant_Value[6];           /* Computed Parameter: Constant_Value
                                        * Referenced by: '<Root>/Constant'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_fray_basic_demo_rpp {
    const char_T *errorStatus;
};

/* Block parameters (auto storage) */
extern Parameters_fray_basic_demo_rpp fray_basic_demo_rpp_P;

/* Block signals (auto storage) */
extern BlockIO_fray_basic_demo_rpp fray_basic_demo_rpp_B;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_fray_basic_demo fray_basic_demo_rpp_Y;

/* Model entry point functions */
extern void fray_basic_demo_rpp_initialize(void);

extern void fray_basic_demo_rpp_step(void);

extern void fray_basic_demo_rpp_terminate(void);

extern void fray_basic_demo_rpp_initialize_transmission(void);

/* Real-time Model object */
extern RT_MODEL_fray_basic_demo_rpp *const fray_basic_demo_rpp_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'fray_basic_demo_rpp'
 */
#endif                                 /* RTW_HEADER_fray_basic_demo_rpp_h_ */
