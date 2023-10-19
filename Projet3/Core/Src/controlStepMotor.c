#include "controlStepMotor.h"
#include "main.h"

int matriceDemiPas[8][4] = {
	{1,0,1,0},
	{1,0,0,0},
	{1,0,0,1},
	{0,0,0,1},
	{0,1,0,1},
	{0,1,0,0},
	{0,1,1,0},
	{0,0,1,0}
};

int indiceMatriceDemiPas = 0;

void fonctionControlMoteur(int direction){

	HAL_GPIO_WritePin(A2m_GPIO_Port, A2m_Pin, matriceDemiPas[indiceMatriceDemiPas][0]);
	HAL_GPIO_WritePin(A2p_GPIO_Port, A2p_Pin, matriceDemiPas[indiceMatriceDemiPas][1]);
	HAL_GPIO_WritePin(B1m_GPIO_Port, B1m_Pin, matriceDemiPas[indiceMatriceDemiPas][2]);
	HAL_GPIO_WritePin(B1p_GPIO_Port, B1p_Pin, matriceDemiPas[indiceMatriceDemiPas][3]);

	indiceMatriceDemiPas+=direction;

	// Redémarrage du compteur
	if (indiceMatriceDemiPas == 8){
		indiceMatriceDemiPas = 0;
	}else if (indiceMatriceDemiPas == -1){
		indiceMatriceDemiPas = 7;
	}


};

