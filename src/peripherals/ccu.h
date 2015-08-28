unsigned int ccuEnable(void);
void ccuConfigureSlice0(const unsigned int input_selector,
                        const unsigned int connections,
                        const unsigned int timer_control);
void ccuConfigureSlice1(const unsigned int input_selector,
                        const unsigned int connections,
                        const unsigned int timer_control);
void ccuStopSlices(const unsigned int slices);
void ccuStartSlices(const unsigned int slices);

// TODO: A lot of the stuff below should be structs and enums.

// CCU4xINS event 0 input.
#define EV0IS_INyA (0x0000 << 0)
#define EV0IS_INyB (0x0001 << 0)
#define EV0IS_INyC (0x0002 << 0)
#define EV0IS_INyD (0x0003 << 0)
#define EV0IS_INyE (0x0004 << 0)
#define EV0IS_INyF (0x0005 << 0)
#define EV0IS_INyG (0x0006 << 0)
#define EV0IS_INyH (0x0007 << 0)
#define EV0IS_INyI (0x0008 << 0)
#define EV0IS_INyJ (0x0009 << 0)
#define EV0IS_INyK (0x000a << 0)
#define EV0IS_INyL (0x000b << 0)
#define EV0IS_INyM (0x000c << 0)
#define EV0IS_INyN (0x000d << 0)
#define EV0IS_INyO (0x000e << 0)
#define EV0IS_INyP (0x000f << 0)
// CCU4xINS event 0 edge.
#define EV0EM_RISING (0x01 << 16)
#define EV0EM_FALLING (0x02 << 16)
#define EV0EM_BOTH (0x03 << 16)
// CCU4xINS event 0 level.
#define EV0LM_HIGH (0x00 << 22)
#define EV0LM_LOW (0x01 << 22)
// CCU4xINS event 0 low pass filter.
#define LPF0M_3 (0x01 << 25)
#define LPF0M_5 (0x02 << 25)
#define LPF0M_7 (0x03 << 25)
// CCU4xINS event 1 input.
#define EV1IS_INyA (0x0000 << 4)
#define EV1IS_INyB (0x0001 << 4)
#define EV1IS_INyC (0x0002 << 4)
#define EV1IS_INyD (0x0003 << 4)
#define EV1IS_INyE (0x0004 << 4)
#define EV1IS_INyF (0x0005 << 4)
#define EV1IS_INyG (0x0006 << 4)
#define EV1IS_INyH (0x0007 << 4)
#define EV1IS_INyI (0x0008 << 4)
#define EV1IS_INyJ (0x0009 << 4)
#define EV1IS_INyK (0x000a << 4)
#define EV1IS_INyL (0x000b << 4)
#define EV1IS_INyM (0x000c << 4)
#define EV1IS_INyN (0x000d << 4)
#define EV1IS_INyO (0x000e << 4)
#define EV1IS_INyP (0x000f << 4)
// CCU4xINS event 1 edge.
#define EV1EM_RISING (0x01 << 18)
#define EV1EM_FALLING (0x02 << 18)
#define EV1EM_BOTH (0x03 << 18)
// CCU4xINS event 1 level.
#define EV1LM_HIGH (0x00 << 23)
#define EV1LM_LOW (0x01 << 23)
// CCU4xINS event 1 low pass filter.
#define LPF1M_0 (0x00 << 27)
#define LPF1M_3 (0x01 << 27)
#define LPF1M_5 (0x02 << 27)
#define LPF1M_7 (0x03 << 27)
// CCU4xINS event 2 input.
#define EV2IS_INyA (0x0000 << 8)
#define EV2IS_INyB (0x0001 << 8)
#define EV2IS_INyC (0x0002 << 8)
#define EV2IS_INyD (0x0003 << 8)
#define EV2IS_INyE (0x0004 << 8)
#define EV2IS_INyF (0x0005 << 8)
#define EV2IS_INyG (0x0006 << 8)
#define EV2IS_INyH (0x0007 << 8)
#define EV2IS_INyI (0x0008 << 8)
#define EV2IS_INyJ (0x0009 << 8)
#define EV2IS_INyK (0x000a << 8)
#define EV2IS_INyL (0x000b << 8)
#define EV2IS_INyM (0x000c << 8)
#define EV2IS_INyN (0x000d << 8)
#define EV2IS_INyO (0x000e << 8)
#define EV2IS_INyP (0x000f << 8)
// CCU4xINS event 2 edge.
#define EV2EM_RISING (0x01 << 20)
#define EV2EM_FALLING (0x02 << 20)
#define EV2EM_BOTH (0x03 << 20)
// CCU4xINS event 2 level.
#define EV2LM_HIGH (0x00 << 24)
#define EV2LM_LOW (0x01 << 24)
// CCU4xINS event 2 low pass filter.
#define LPF2M_3 (0x01 << 29)
#define LPF2M_5 (0x02 << 29)
#define LPF2M_7 (0x03 << 29)

// CCY4yCMC connection matrix control.
// CCU4yCMC start event.
#define STRTS_EV0 (0x01 << 0)
#define STRTS_EV1 (0x02 << 0)
#define STRTS_EV2 (0x03 << 0)
// CCU4yCMC stop event.
#define ENDS_EV0 (0x01 << 2)
#define ENDS_EV1 (0x02 << 2)
#define ENDS_EV2 (0x03 << 2)
// CCU4yCMC capture event 0.
#define CAP0S_EV0 (0x01 << 4)
#define CAP0S_EV1 (0x02 << 4)
#define CAP0S_EV2 (0x03 << 4)
// CCU4yCMC capture event 1.
#define CAP1S_EV0 (0x01 << 6)
#define CAP1S_EV1 (0x02 << 6)
#define CAP1S_EV2 (0x03 << 6)
// CCU4yCMC gate event.
#define GATES_EV0 (0x01 << 8)
#define GATES_EV1 (0x02 << 8)
#define GATES_EV2 (0x03 << 8)
// CCU4yCMC up/down functionality event.
#define UDS_EV0 (0x01 << 10)
#define UDS_EV1 (0x02 << 10)
#define UDS_EV2 (0x03 << 10)
// CCU4yCMC timer load event.
#define LDS_EV0 (0x01 << 12)
#define LDS_EV1 (0x02 << 12)
#define LDS_EV2 (0x03 << 12)
// CCU4yCMC count event.
#define CNTS_EV0 (0x01 << 14)
#define CNTS_EV1 (0x02 << 14)
#define CNTS_EV2 (0x03 << 14)
// CCU4yCMC override enable (when enabled event 1 is trigger and 2 is value).
#define OFS_ENABLE BIT16
// CCU4yCMC trap enable (when enabled event 2 is trap).
#define TS_ENABLE BIT17
// CCU4yCMC modulation event.
#define MOS_EV0 (0x01 << 18)
#define MOS_EV1 (0x02 << 18)
#define MOS_EV2 (0x03 << 18)
// CCU4yCMC timer concat enable (when enabled concat from previous slice(
#define TCE_ENABLE BIT20

// CCU4yTC timer control.
// CCU4yTCM timer counting mode
#define TCM_EDGE 0x00
#define TCM_CENTER BIT0
// CCU4yTSSM timer single shot mode
#define TSSM_ENABLE BIT1
// CCU4yCLST shadow transfer on clear
#define CLST_ENABLE BIT2
// CCU4yCMOD capture compare mode
#define CMOD_COMPARE 0x00
#define CMOD_CAPTURE (0x01 << 3)
// CCU4yECM extended capture mode
#define ECM_ENABLE BIT4
// CCU4yCAPC clear on capture
#define CAPC_CLEAR23 (0x01 << 5)
#define CAPC_CLEAR01 (0x02 << 5)
#define CAPC_CLEAR (0x03 << 5)
// CCU4yENDM extended stop function
#define ENDM_RUNBIT (0x00 << 8)
#define ENDM_TIMER (0x01 << 8)
#define ENDM_BOTH (0x02 << 8)
// CCU4ySTRM extended start function
#define STRM_RUNBIT 0x00
#define STRM_BOTH BIT10
// CCU4ySCE equal capture event enable
#define SCE_CAPT01 0x00
#define SCE_CAPT11 BIT11
// CCU4yCCS continuous capture enable
#define CCS_ENABLE BIT12
// CCU4yDITHE dither enable
#define DITHE_PERIOD (0x01 << 13)
#define DITHE_COMPARE (0x02 << 13)
#define DITHE_BOTH (0x03 << 13)
// CCU4yDIM dither input selector
#define DIM_LOCAL 0x00
#define DIM_SLICE0 BIT15
// CCU4yFPE floating prescaler enable
#define FPE_ENABLE BIT16
// CCU4yTRAPE trap enable
#define TRAPE_ENABLE BIT17
// CCU4yTRPSE trap sync enable
#define TRPSE_ENABLE BIT21
// CCU4yTRPSW trap state clear control
#define TRPSW_ENABLE BIT22
// CCU4yEMS external modulation sync
#define EMS_ENABLE BIT23
// CCU4yEMT external modulation type
#define EMT_ENABLE BIT24
// CCU4yMCME multi channel mode enable
#define MCME_ENABLE BIT25

