// Support code for the temperature sensor.
// All TSE events are handled through interrupt SCU_SR1.

#ifndef PERIPHERALS_TEMPERATURE_H
#define PERIPHERALS_TEMPERATURE_H

// Define ERRATA_SCU_CM_014 to avoid the ROM routine affected by
// errata SCU_CM.014.  Temperature values will be raw from the sensor
// rather than Kelvin if this is defined.
#define ERRATA_SCU_CM_014

unsigned int tseEnable(void);
unsigned int tseDisable(void);
unsigned int tseRead(void);

// SCU_SRMSK and SCU_SRRAW mask values for temperature sensor events.
#define TSE_DONE BIT29
#define TSE_HIGH BIT30
#define TSE_LOW BIT31

#endif
