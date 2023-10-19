/*
 * ft6206.h
 *
 *  Created on: 29 juin 2022
 *      Author: MIDOM7
 */

#ifndef INC_FT6206_H_
#define INC_FT6206_H_

#include "stm32f4xx_hal.h"
//#include "Arduino.h"
//#include <Wire.h>

#define FT62XX_ADDR 0x38           //!< I2C address
#define FT62XX_G_FT5201ID 0xA8     //!< FocalTech's panel ID
#define FT62XX_REG_NUMTOUCHES 0x02 //!< Number of touch points

#define FT62XX_NUM_X 0x33 //!< Touch X position
#define FT62XX_NUM_Y 0x34 //!< Touch Y position

#define FT62XX_REG_MODE 0x00        //!< Device mode, either WORKING or FACTORY
#define FT62XX_REG_CALIBRATE 0x02   //!< Calibrate mode
#define FT62XX_REG_WORKMODE 0x00    //!< Work mode
#define FT62XX_REG_FACTORYMODE 0x40 //!< Factory mode
#define FT62XX_REG_THRESHHOLD 0x80  //!< Threshold for touch detection
#define FT62XX_REG_POINTRATE 0x88   //!< Point rate
#define FT62XX_REG_FIRMVERS 0xA6    //!< Firmware version
#define FT62XX_REG_CHIPID 0xA3      //!< Chip selecting
#define FT62XX_REG_VENDID 0xA8      //!< FocalTech's panel ID

#define FT62XX_VENDID 0x11  //!< FocalTech's panel ID
#define FT6206_CHIPID 0x06  //!< Chip selecting
#define FT6236_CHIPID 0x36  //!< Chip selecting
#define FT6236U_CHIPID 0x64 //!< Chip selecting

// calibrated for Adafruit 2.8" ctp screen
#define FT62XX_DEFAULT_THRESHOLD 128 //!< Default threshold for touch detection

struct touch_screen_struct {
	int touche;				//Sera a 1 si une touche a été appuyé
	int position_x;
	int position_y;

};

uint8_t FT6206_BEGIN(int threshold);
uint8_t FT6206_touched(void);
uint8_t FT6206_Read_position(struct touch_screen_struct *ts);
void FT_6206_Wait_touch_release(void);



#endif /* INC_FT6206_H_ */
