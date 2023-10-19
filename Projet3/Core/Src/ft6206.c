/*
 * ft6206.c
 *
 *  Created on: 29 juin 2022
 *      Author: MIDOM7
 */
#include "stm32f4xx_hal.h"

#include "ft6206.h"


extern I2C_HandleTypeDef hi2c1;
#define del_TIMOUT 10000


//Fonction: FT62006_BEGIN
//Paramètre: threshold: sensibilité de l'écran (0-255)
uint8_t FT6206_BEGIN(int threshold)
{

	char ptr[5];
	HAL_StatusTypeDef ret;

	ptr[0] = FT62XX_REG_THRESHHOLD;
	ptr[1] = threshold;

	ret = HAL_I2C_Master_Transmit(&hi2c1, FT62XX_ADDR<<1, (uint8_t*)ptr, 2, del_TIMOUT);
	if(ret !=HAL_OK){
		return HAL_ERROR;
	}
	else {
		return HAL_OK;
	}


}


/**************************************************************************/
/*!
    @brief  Determines if there are any touches detected
    @returns Number of touches detected, can be 0, 1 or 2
*/
/**************************************************************************/
uint8_t FT6206_touched(void) {

	uint8_t n=0;
	char buffer[20];
	buffer[0] = FT62XX_REG_NUMTOUCHES;
	HAL_I2C_Master_Transmit(&hi2c1, FT62XX_ADDR<<1, (uint8_t*)buffer, 1, del_TIMOUT);
	HAL_I2C_Master_Receive(&hi2c1, FT62XX_ADDR<<1, (uint8_t*)buffer, 1, del_TIMOUT);
	n=buffer[0];
	if (n > 2) {		//SEULEMENT UN NOMBRE ENTRE 0 ET 2 EST VALIDE
    n = 0;
  }
  return n;
}


//********************************************************************************************************
//Fonction : FT6206_Read_position
//Desription: Regarde si un touche a été détecté sur le pavée tactile et lit la position si c'est le cas
// Parametre: Structure de pour le Touch Screen
//Retour: Nombre de touche détectée, donc 0 ou 1
//********************************************************************************************************
uint8_t FT6206_Read_position(struct touch_screen_struct *ts) {

	char buffer[10];
	buffer[0] = 0;

		if(FT6206_touched()==2) {		//Cette condition règle un petit problème, l'écran gèle si on a deux touche simultané. Donc on reset si ça arrive
			HAL_I2C_Init(&hi2c1);
			ts->touche=0;
		}
		else{
			HAL_I2C_Master_Transmit(&hi2c1, FT62XX_ADDR<<1, (uint8_t*)buffer, 1, del_TIMOUT);
				HAL_I2C_Master_Receive(&hi2c1, FT62XX_ADDR<<1, (uint8_t*)buffer, 9, del_TIMOUT);
				ts->touche = buffer[2];
				if(ts->touche>=1){
					ts->position_y= 255-buffer[4];

					ts->position_x = (buffer[5]&0x0f)<<8;
					ts->position_x += buffer[6];
				}

		}

	return ts->touche;		//Retourne le registre du nombre de touche
}


//Attendre tant que l'écran détecte une touche
void FT_6206_Wait_touch_release(void)
{
	struct touch_screen_struct tt;
	while(FT6206_Read_position(&tt) !=0){
		HAL_Delay(1);
	}


}
