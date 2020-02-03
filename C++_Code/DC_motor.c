// ECE 4550 Lab 6
// Purpose: DC Motor Position Control
// Author: Faiyaz Chowdhury, Justin Zheng

#include "F2806x_Device.h"
#include <math.h>

/**
 * main.c
 */
interrupt void TimerISR(void);

//Variables
float32 Vavg = 0.0;
Uint16 timerCount = 0;
Uint16 cycleCount = 0;
Uint16 VDC = 24;
int32 pos = 0;
float32 angle;
Uint16 sat = 0;

float32 data_u[2000];
float32 data_y[2000];

//Integral Control Setup
const float32 alpha = 102.2972;
const float32 beta = 581.42;

float32 lambda_r, lambda_e, K11, K12, K2, L1, L2;
float32 r = 10 * 3.1415926;
float32 u, x_hat1, x_hat2, sigma, y;
float32 u_prime, x_hat1_prime, x_hat2_prime, sigma_prime, y_prime;

float32 T = 0.001;

Uint16 anti_windup = 1;

int main(void)
{
    EALLOW;
    SysCtrlRegs.WDCR = 0x68; //Disable watchdog
    //System Clock
    if (SysCtrlRegs.PLLSTS.bit.MCLKSTS == 1) {
        return 0; //Limp mode, don't do anything
    } else if (SysCtrlRegs.PLLSTS.bit.DIVSEL >= 2) {
        SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
    }
    SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
    SysCtrlRegs.PLLCR.bit.DIV = 9;
    while (SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1) {
        ;
    }
    SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
    SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;

    //Timer setup
    CpuTimer0Regs.TCR.bit.TSS = 1; // Deactivate timer
    CpuTimer0Regs.PRD.all = 89999; // CPUclk = 90MHZ
    CpuTimer0Regs.TPR.bit.TDDR = 0;
    CpuTimer0Regs.TPRH.bit.TDDRH = 0;
    CpuTimer0Regs.TCR.bit.TRB = 1;
    CpuTimer0Regs.TCR.bit.TIE = 1;

    //PWM Setup
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; //pwma
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; //reset
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1; //pwmb
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; //reset

    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;

    EPwm1Regs.TBCTL.bit.CTRMODE = 2;
    EPwm2Regs.TBCTL.bit.CTRMODE = 2;
    EPwm1Regs.TBPRD = 1500;
    EPwm2Regs.TBPRD = 1500;

    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm2Regs.TBCTL.bit.CLKDIV = 0;

    EPwm1Regs.AQCTLA.bit.CAU = 1;
    EPwm1Regs.AQCTLA.bit.CAD = 2;
    EPwm2Regs.AQCTLA.bit.CAU = 2;
    EPwm2Regs.AQCTLA.bit.CAD = 1;

    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

    //GPIO setup
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;

    EPwm1Regs.CMPA.half.CMPA = 1500/2.0;
    EPwm2Regs.CMPA.half.CMPA = 1500/2.0;

    GpioDataRegs.GPASET.bit.GPIO1 = 1;
    GpioDataRegs.GPASET.bit.GPIO3 = 1;

    //QEP Setup
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;
    SysCtrlRegs.PCLKCR1.bit.EQEP1ENCLK = 1;
    EQep1Regs.QPOSMAX = 0xFFFFFFFF;
    EQep1Regs.QPOSINIT = 0;
    EQep1Regs.QEPCTL.bit.QPEN = 1;
    EQep1Regs.QEPCTL.bit.SWI = 1;

        //Interrupts Setup
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    //TimerIsr setup
    PieVectTable.TINT0 = &TimerISR;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;


    PieCtrlRegs.PIEACK.all = 1;
    IER = 1; // 1
    EINT;

    CpuTimer0Regs.TCR.bit.TSS = 0;// Activate timer

    //End initialization here

    //Integral Control params
    lambda_r = 62;
    lambda_e = 4 * lambda_r;

    L1 = 2 * lambda_e - alpha;
    L2 = lambda_e * lambda_e - 2 * alpha * lambda_e + alpha * alpha;
    K11 = 1 / beta * 3 * lambda_r * lambda_r;
    K12 = 1 / beta * (3 * lambda_r - alpha);
    K2 = 1 / beta * lambda_r * lambda_r * lambda_r;

    x_hat1 = 0;
    x_hat2 = 0;
    sigma = 0;
    x_hat1_prime = 0;
    x_hat2_prime = 0;
    sigma_prime = 0;


    SysCtrlRegs.WDCR = 0x28;
    EDIS;

    while(1){
        EALLOW;
        SysCtrlRegs.WDKEY = 0x55;
        SysCtrlRegs.WDKEY = 0xAA;
        EDIS;
    }
    return 0;
}

interrupt void TimerISR(void) {

    EALLOW;
    cycleCount++;
    if (cycleCount < 2000) {
        data_u[cycleCount] = u;
        data_y[cycleCount] = y;
    }
    timerCount++;
    if (timerCount == 1000) {
        timerCount = 0;
        r = 10 * 3.1415926 - r;
    }
    pos = EQep1Regs.QPOSCNT;
    angle = pos / 1000.0 * 2 * 3.14159;

    //Control
    x_hat1 = x_hat1_prime;
    x_hat2 = x_hat2_prime;
    sigma = sigma_prime;

    y = angle;
    u = -1 * K11 * x_hat1 - 1 * K12 * x_hat2 - K2 * sigma;
    //Actuator saturation here
    if (u > 24) {
        u = 24.0;
    } else if (u < -24) {
        u = -24.0;
    }

    if (anti_windup == 0) {
        sigma_prime = sigma + T * (y - r);
    } else if (anti_windup == 1 && (u == 24.0 || u == -24.0)) {
        sigma_prime = sigma;
    } else {
        sigma_prime = sigma + T * (y - r);
    }
    //////////////////////////
    x_hat1_prime = x_hat1 + T * x_hat2 - T * L1 * (x_hat1 - y);
    x_hat2_prime = x_hat2 - T * alpha * x_hat2 + T * beta * u - T * L2 * (x_hat1 - y);


    Vavg = u;

    EPwm1Regs.CMPA.half.CMPA = (Uint16) (1500 * (Vavg / VDC + 1) / 2);
    EPwm2Regs.CMPA.half.CMPA = (Uint16) (1500 * (Vavg / VDC + 1) / 2);
    EDIS;



    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
