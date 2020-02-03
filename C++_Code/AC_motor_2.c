#include "F2806x_Device.h"
#include <math.h>

/**
 * main.c
 */

#define DUTY_CYCLE(Vavg) (Vavg + 24)/48
#define ALPHA           9.17
#define BETA            99.09

#define DT              .0002

#define N       2
#define Vmax   20

#define Rr  2.24
#define Rs  1.0
#define gamma .9256
#define R   2.9192
#define K   .0926

float32 lambda_r =      25.0;
float32 lambda_e =      100.0;
float32 K11;
float32 K12;
float32 K2;
float32 L1;
float32 L2;

float32 alphaV, thetaV;
float32 thetaE = 0, rho = 0;

interrupt void timerISR(void);

float time = 0;

float u = 0.0f;

float desiredLegVoltages[] = {16, 8, 12};
float dutyCycles[] = {.5, .5, .5};

volatile Uint8 angleIndex = 0;
float desiredAngles[2] = {0, 6.28318530718};
float times[2] = {1, 1};
float lastTime = 0;

float32 uVals[1000], yVals[1000];
Uint16 nReadings = 0;

volatile int32 rawAngle;

float32 desiredAngle = 0 * 2 * 3.1415926535;
float32 rotorAngle = 0.0f;

// Estimator parameters
float32 omegaHat = 0.0f, thetaHat = 0.0f;
float32 sigma = 0.0f;

Uint8 initializing = 1;

void InitClock(void) {
    if(SysCtrlRegs.PLLSTS.bit.DIVSEL == 2 || SysCtrlRegs.PLLSTS.bit.DIVSEL == 3) {
        SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
    }

    SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
    SysCtrlRegs.PLLCR.bit.DIV = 9;              // OSC * 9 / 1

    while(SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1);

    SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
    SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;

    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1; // Enable PWM clock
    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1; // Enable PWM clock
    SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1; // Enable PWM clock
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
}

void InitGPIO(void) {
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // Set mux to EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1; // Set mux to EPWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // for PWMA enable
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; // for PWNB enable
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0; // for PWNB enable

    GpioCtrlRegs.GPADIR.bit.GPIO1 = 0x01;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 0x01;
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 0x01;
}

void InitPWMA(void) {
    EPwm1Regs.TBCTL.bit.CTRMODE = 2;        // Count up/down (triangular)

    EPwm1Regs.TBPRD = 1500;                 // Counter = 2 * 24V / .032 mV

    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;      // Corresponds to 30 kHz pwm frequency @ 90 MHz clock

    EPwm1Regs.AQCTLA.bit.CAU = 1;
    EPwm1Regs.AQCTLA.bit.CAD = 2;

    // Set up phase offset to 0 (this is the base module)
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = 1;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = 1;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = 0;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = 0;
    EPwm1Regs.TBPHS.half.TBPHS = 0;
    EPwm1Regs.TBCTL.bit.PHSEN = 0;
    EPwm1Regs.TBCTL.bit.PRDLD = 1;
}

void InitPWMB(void) {
    EPwm2Regs.TBCTL.bit.CTRMODE = 2;        // Count up/down (triangular)

    EPwm2Regs.TBPRD = 1500;                 // Counter = 2 * 24V / .032 mV
    EPwm2Regs.TBCTL.bit.CLKDIV = 0;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;      // Corresponds to 30 kHz pwm frequency @ 90 MHz clock

    EPwm2Regs.AQCTLA.bit.CAU = 1;
    EPwm2Regs.AQCTLA.bit.CAD = 2;

    EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = 1;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = 1;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = 0;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = 0;
    EPwm2Regs.TBPHS.half.TBPHS = 1000;      // 120 / (360 / 3000) = 1000
    EPwm2Regs.TBCTL.bit.PHSEN = 1;
    EPwm2Regs.TBCTL.bit.PRDLD = 1;
}

void InitPWMC(void) {
    EPwm3Regs.TBCTL.bit.CTRMODE = 2;        // Count up/down (triangular)

    EPwm3Regs.TBPRD = 1500;                 // Counter = 2 * 24V / .032 mV
    EPwm3Regs.TBCTL.bit.CLKDIV = 0;
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;      // Corresponds to 30 kHz pwm frequency @ 90 MHz clock

    EPwm3Regs.AQCTLA.bit.CAU = 1;
    EPwm3Regs.AQCTLA.bit.CAD = 2;

    EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = 1;
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = 1;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = 0;
    EPwm3Regs.CMPCTL.bit.LOADBMODE = 0;
    EPwm3Regs.TBPHS.half.TBPHS = 1000;      // 120 / (360 / 3000) = 1000
    EPwm3Regs.TBCTL.bit.PHSDIR = 1;
    EPwm3Regs.TBCTL.bit.PHSEN = 1;
    EPwm3Regs.TBCTL.bit.PRDLD = 1;
}

void InitTimer(Uint32 period) {
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    PieVectTable.TINT0 = &timerISR;

    CpuTimer0Regs.TCR.bit.TSS = 1;
    CpuTimer0Regs.TPR.bit.TDDR = 0;
    CpuTimer0Regs.TPRH.bit.TDDRH = 0;
    CpuTimer0Regs.PRD.all = period;       // 90 MHz / 5 kHz - 1
    CpuTimer0Regs.TCR.bit.TIE = 1;
    CpuTimer0Regs.TCR.bit.TRB = 1;
    CpuTimer0Regs.TCR.bit.TSS = 0;
}

void setPowerStageEnabled(Uint8 enabled){
    if(enabled) {
        GpioDataRegs.GPASET.bit.GPIO1 = 1;
        GpioDataRegs.GPASET.bit.GPIO3 = 1;
        GpioDataRegs.GPASET.bit.GPIO5 = 1;
    } else {
        GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
        GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
        GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
    }
}

void updateDutyCycles(void) {
    EPwm1Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[0] * EPwm1Regs.TBPRD);
    EPwm2Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[1] * EPwm2Regs.TBPRD);
    EPwm3Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[2] * EPwm3Regs.TBPRD);
}

inline void setAverageLegVoltages(float Vdc, float u) {
    float32 Vd = sqrt(pow(Vmax, 2) - pow(u, 2));
    alphaV = sqrt(pow(0, 2) + pow(u, 2));
    thetaV = atan2(u, 0);

    desiredLegVoltages[0] = .5 * Vdc + 0.47140452079 * alphaV * cos(N * thetaE + thetaV - 0.52359877559);
    desiredLegVoltages[1] = .5 * Vdc + 0.47140452079 * alphaV * cos(N * thetaE + thetaV - 2.61799387799);
    desiredLegVoltages[2] = .5 * Vdc + 0.47140452079 * alphaV * cos(N * thetaE + thetaV + 1.57079632679);

    dutyCycles[0] = desiredLegVoltages[0]/Vdc;
    dutyCycles[1] = desiredLegVoltages[1]/Vdc;
    dutyCycles[2] = desiredLegVoltages[2]/Vdc;

    EPwm1Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[0] * EPwm1Regs.TBPRD);
    EPwm2Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[1] * EPwm2Regs.TBPRD);
    EPwm3Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[2] * EPwm3Regs.TBPRD);
}

inline void setAverageLegVoltagesABC(float Vdc, float Va, float Vb, float Vc) {

    desiredLegVoltages[0] = Va;
    desiredLegVoltages[1] = Vb;
    desiredLegVoltages[2] = Vc;

    dutyCycles[0] = desiredLegVoltages[0]/Vdc;
    dutyCycles[1] = desiredLegVoltages[1]/Vdc;
    dutyCycles[2] = desiredLegVoltages[2]/Vdc;

    EPwm1Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[0] * EPwm1Regs.TBPRD);
    EPwm2Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[1] * EPwm2Regs.TBPRD);
    EPwm3Regs.CMPA.half.CMPA = (Uint16)(dutyCycles[2] * EPwm3Regs.TBPRD);
}

void resetRotorToZero(void) {
    initializing = 1;
    setAverageLegVoltagesABC(24, 13.2, 10.8, 12.0);
}

int main(void) {
    while(SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0);    // Wait up to 512 clock cycles or reset to get PLL out of limp mode

    EALLOW;
    DINT;
    SysCtrlRegs.WDCR = 0x68;        // Disable watchdog

    InitClock();
    InitGPIO();
    InitPWMA();
    InitPWMB();
    InitPWMC();
    InitTimer(17999);

    updateDutyCycles();

    //setAverageLegVoltages(5, 0);
    setPowerStageEnabled(0);

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    // Set up QEP module
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;

    SysCtrlRegs.PCLKCR1.bit.EQEP1ENCLK = 1;
    SysCtrlRegs.PCLKCR1.bit.EQEP2ENCLK = 1;

    EQep1Regs.QPOSMAX = 0xFFFFFFFF;
    EQep2Regs.QPOSMAX = 0xFFFFFFFF;

    IER = 1;
    SysCtrlRegs.WDCR = 0x28;
    EDIS;
    EINT;

    setPowerStageEnabled(1);

    resetRotorToZero();
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
    while(1) {
        // Service the dog
        EALLOW;
        SysCtrlRegs.WDKEY = 0x55;
        SysCtrlRegs.WDKEY = 0xAA;
        EDIS;
    }

	return 0;
}

Uint32 counts = 0;
interrupt void timerISR(void) {
    time += DT;
    rawAngle = (int32)EQep1Regs.QPOSCNT;
    rotorAngle = 2 * 3.14159265358979 * rawAngle / 4000;
    if(time - lastTime > times[angleIndex]) {
        angleIndex = (angleIndex + 1) % 2;
        lastTime = time;
    }
    desiredAngle = desiredAngles[angleIndex];
    // State estimation
    if(initializing) {
        if(time >= 1.0) {
            EQep1Regs.QPOSINIT = 0;
            EQep1Regs.QEPCTL.bit.QPEN = 1;
            EQep1Regs.QEPCTL.bit.SWI = 1;

            EQep2Regs.QPOSINIT = 0;
            EQep2Regs.QEPCTL.bit.QPEN = 1;
            EQep2Regs.QEPCTL.bit.SWI = 1;
            rotorAngle = 0;
            rawAngle = 0;
            initializing = 0;
        }
    } else {

            u = -(3*lambda_r*lambda_r)/BETA * thetaHat - (3*lambda_r - ALPHA)/BETA * omegaHat - (lambda_r*lambda_r*lambda_r)/BETA * sigma;
            if(u > Vmax) {
                u = Vmax;
            } else if(u < -Vmax) {
                u = -Vmax;
            } else {
                sigma += DT * (rotorAngle - desiredAngle);
            }
            if(counts % 10 == 0 && time > 1.0) {
                uVals[nReadings] = u;
                yVals[nReadings] = rotorAngle;
                nReadings = (nReadings + 1) % 1000;
            }
            counts++;

            omegaHat += DT * (-ALPHA * omegaHat + BETA * u + (lambda_e*lambda_e - 2 * ALPHA * lambda_e + ALPHA * ALPHA) * (rotorAngle - thetaHat));
            thetaHat += DT * (omegaHat + (2*lambda_e - ALPHA) * (rotorAngle - thetaHat));

            thetaE = Rs/R * thetaHat + rho;
            rho += DT * (gamma*gamma * Rr / R) * u / K;


            if(time - lastTime > times[angleIndex]) {
                angleIndex = (angleIndex + 1) % 2;
                lastTime = time;
            }

            //if(fabs(thetaHat - desiredAngle) >= .05) {
                setAverageLegVoltages(24, u);
            //}
            // SET AVERAGE VOLTAGES
    }
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}
