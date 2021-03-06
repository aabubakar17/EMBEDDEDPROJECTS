
/* ======================================================
   TSI: FRDM KL25Z development board touch pad 
   
   Interface
     * TSI_init
     * readTSIDistance
         
   The implememtation is interrupt driven
    ========================================================= */

#include "cmsis_os2.h"
#include <MKL25Z4.h>
#include <stdbool.h>
#include "../include/TSI.h"


#define NO_TOUCH                 0
#define SLIDER_LENGTH           40 //LENGTH in mm
#define TOTAL_ELECTRODE          2

#define TSI9         9    // TSI channel for electrode 1
#define TSI10        10   // TSI channel for electrode 2

#define THRESHOLD0   100
#define THRESHOLD1   100


// TSI channel connected to KL25Z slider
const uint8_t elec_array[TOTAL_ELECTRODE] = {TSI9, TSI10} ;

// baseline counts updated during calibration
uint16_t gu16Baseline[TOTAL_ELECTRODE];

// current positions 
uint8_t SliderDistancePosition[TOTAL_ELECTRODE] = {NO_TOUCH,NO_TOUCH};
uint32_t AbsoluteDistancePosition = NO_TOUCH;

// threshold values for returning no touch
const uint16_t gu16Threshold[TOTAL_ELECTRODE]={THRESHOLD0,THRESHOLD1} ;

// ------ variables updated in the ISR ---------
// current count - updated in ISR
volatile uint16_t gu16TSICount[TOTAL_ELECTRODE];

// current different from baseline - updated in ISR
volatile uint16_t gu16Delta[TOTAL_ELECTRODE];

// kkep track of the scanning process
volatile uint8_t ongoing_elec;  // number of the elcetrode currently being scanned
volatile bool end_flag = true ;  // Whether a scan has completed since last read



/* ================================
     Initialisation and Calibration
   ================================ */

/* --------------------------------
    Calibrartion
     * Overwrites the TSI trigger mode
     * Assumes interrupts are disabled
     * Write values to gu16Baseline array

   -------------------------------- */
void tsiCalibration(void) {
    unsigned char cnt;

    // set mode for calibration
    TSI0->GENCS |= TSI_GENCS_EOSF_MASK;      // Clear End of Scan Flag
    TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK;    // Disable TSI module 
    TSI0->GENCS &= ~TSI_GENCS_STM_MASK;      // Use SW trigger
    TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;     // Enable TSI module

    for(cnt=0; cnt < TOTAL_ELECTRODE; cnt++)  // Get Counts when Electrode not pressed
    {
        TSI0->DATA = ((elec_array[cnt] << TSI_DATA_TSICH_SHIFT) );  // top bits are channel numer
        TSI0->DATA |= TSI_DATA_SWTS_MASK;                           // s/w trigger to start scan
        while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK));                // wait for end of scan
        TSI0->GENCS |= TSI_GENCS_EOSF_MASK;                         // write to clear end of scan flag
        gu16Baseline[cnt] = (TSI0->DATA & TSI_DATA_TSICNT_MASK);    // update array of baseline counts
    }
}


/* --------------------------------
    Initialise
     * Set the configuration register
     * Calls calibration
     * Enable the interrupts
     * Start first conversion
   -------------------------------- */
void TSI_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;

    TSI0->GENCS |= (TSI_GENCS_ESOR_MASK       // end of scan interrupt
                   | TSI_GENCS_MODE(0)        // capacitive sense mode
                   | TSI_GENCS_REFCHRG(4)     // ref oscillator 8 micro A - middle charge current
                   | TSI_GENCS_DVOLT(0)       // CVoltage 1.03 to 1.33
                   | TSI_GENCS_EXTCHRG(7)     // electrode oscillator charge 64 micro A
//                   | TSI_GENCS_PS(4)          // pre-scalar - divide by 16 (from mbed)
                   | TSI_GENCS_PS(3)          // pre-scalar - divide by 8 (since clock slower)
                   | TSI_GENCS_NSCN(11)       // 4 scans per electrode
//                   | TSI_GENCS_TSIIEN_MASK    // enable the TSI interrupt
                   | TSI_GENCS_STPE_MASK      // enable in low power mode
                   );

    // module is NOT enabled
    
    // do the calibration - does not use interrupt
    tsiCalibration();
    
    // Enable the interrupt
    NVIC_SetPriority(TSI0_IRQn, 128) ;
    NVIC_ClearPendingIRQ(TSI0_IRQn) ;
    NVIC_EnableIRQ(TSI0_IRQn);

    // Enable interrupt for TSI
    TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK;    // Disable TSI module
    TSI0->GENCS &= ~TSI_GENCS_STM_MASK;      // Use SW trigger
    TSI0->GENCS |= TSI_GENCS_TSIIEN_MASK;    // Enable TSI interrupt
    TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;     // Enable TSI module

    // Start scan on first electrode - this with cause an interrupt 
    ongoing_elec = 0 ;
    TSI0->DATA = ((elec_array[0]<<TSI_DATA_TSICH_SHIFT) );
    TSI0->DATA |= TSI_DATA_SWTS_MASK;
}



/* ================================
     Interrupt handler

   Called when scan has been completed
     ongoing_elec is either 0 or 1
   ================================ */

void TSI0_IRQHandler(void) {
    
    TSI0->GENCS |= TSI_GENCS_EOSF_MASK; // Clear End of Scan Flag
    
    int16_t u16temp_delta;

    // Update count
    //    Calculate the difference over baseline
    //    Ignore differences less than threshold 
    gu16TSICount[ongoing_elec] = (TSI0->DATA & TSI_DATA_TSICNT_MASK);          // Save count for current electrode
    u16temp_delta = gu16TSICount[ongoing_elec] - gu16Baseline[ongoing_elec];   // Counts delta from calibration reference
    if(u16temp_delta < 0)
        gu16Delta[ongoing_elec] = 0;
    else
        gu16Delta[ongoing_elec] = u16temp_delta;

    //Change Electrode to Scan
    if(ongoing_elec == 0) {
        ongoing_elec = 1 ; 
    } else {
        ongoing_elec = 0;
        end_flag = 1;
    }

    // Restart the scan of the next electrode
    TSI0->DATA = ((elec_array[ongoing_elec]<<TSI_DATA_TSICH_SHIFT) );
    TSI0->DATA |= TSI_DATA_SWTS_MASK;
}

    
/* ================================
     Read Distance
   ================================ */


/* ------------------------
     readTSIDistance

   Read the distance from the data updated by the ISR
   Clear the end_flag


    Returns
      0 - no touch
     1-40 position of touch point
   ------------------------ */
uint8_t readTSIDistance() {
    if(end_flag) {  // skip recalculation if no new scan data
        end_flag = false ;
        if((gu16Delta[0] > gu16Threshold[0])||(gu16Delta[1] > gu16Threshold[1])) {
            uint16_t totalDelta = gu16Delta[0] + gu16Delta[1] ;
            SliderDistancePosition[0] = (gu16Delta[0]* SLIDER_LENGTH) / totalDelta ;
            SliderDistancePosition[1] = (gu16Delta[1]* SLIDER_LENGTH) / totalDelta ;
            AbsoluteDistancePosition = ((SLIDER_LENGTH - SliderDistancePosition[0]) + SliderDistancePosition[1])/2;
         } else {
            SliderDistancePosition[0] = NO_TOUCH;
            SliderDistancePosition[1] = NO_TOUCH;
            AbsoluteDistancePosition = NO_TOUCH;
         }
    }
    return AbsoluteDistancePosition;
}

