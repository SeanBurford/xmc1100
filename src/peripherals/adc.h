int enableADCPin(const unsigned int port, const unsigned int pin,
                 const unsigned int mode);
int disableADCPin(const unsigned int port, const unsigned int pin);
void adcCalibrate(int wait);
int adcEnable(void);
int adcDisable(void);
