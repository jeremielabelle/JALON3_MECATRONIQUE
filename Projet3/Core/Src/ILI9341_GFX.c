//	MIT License
//
//	Copyright (c) 2017 Matej Artnak
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//
//
//
//-----------------------------------
//	ILI9341 GFX library for STM32
//-----------------------------------
//
//	Very simple GFX library built upon ILI9342_STM32_Driver library.
//	Adds basic shapes, image and font drawing capabilities to ILI9341
//
//	Library is written for STM32 HAL library and supports STM32CUBEMX. To use the library with Cube software
//	you need to tick the box that generates peripheral initialization code in their own respective .c and .h file
//
//
//-----------------------------------
//	How to use this library
//-----------------------------------
//
//	-If using MCUs other than STM32F7 you will have to change the #include "stm32f7xx_hal.h" in the ILI9341_GFX.h to your respective .h file
//
//	If using "ILI9341_STM32_Driver" then all other prequisites to use the library have allready been met
//	Simply include the library and it is ready to be used
//
//-----------------------------------


#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include "12X16_font2.h"
#include "main.h"
#include "string.h"
//#include "spi.h"


extern SPI_HandleTypeDef hspi3;
int LCD_SPI_DMA_IN_PROGRESS;

uint16_t buffer_1_ligne[16*320];			//largeur du char X largeur de l'écran
uint16_t buffer_one_char[12*16*4*4];		//Assez gros pour avoir la possibilité de faire un X4 sur la grosseur du char
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
	//while(!__HAL_SPI_GET_FLAG(&hspi3,SPI_FLAG_TXE)){};
	LCD_SPI_DMA_IN_PROGRESS=0;
}

/*Draw hollow circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (Radius << 1);

    while (x >= y)
    {
        ILI9341_Draw_Pixel(X + x, Y + y, Colour);
        ILI9341_Draw_Pixel(X + y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - x, Y + y, Colour);
        ILI9341_Draw_Pixel(X - x, Y - y, Colour);
        ILI9341_Draw_Pixel(X - y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + x, Y - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-Radius << 1) + dx;
        }
    }
}

/*Draw filled circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	
		int x = Radius;
    int y = 0;
    int xChange = 1 - (Radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = X - x; i <= X + x; i++)
        {
            ILI9341_Draw_Pixel(i, Y + y,Colour);
            ILI9341_Draw_Pixel(i, Y - y,Colour);
        }
        for (int i = X - y; i <= X + y; i++)
        {
            ILI9341_Draw_Pixel(i, Y + x,Colour);
            ILI9341_Draw_Pixel(i, Y - x,Colour);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
		//Really slow implementation, will require future overhaul
		//TODO:	https://stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles	
}

/*Draw a hollow rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	float 		Calc_Negative = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;		
	}
	else
	{
		X_length = X0 - X1;		
	}
	ILI9341_Draw_Horizontal_Line(X0, Y0, X_length, Colour);
	ILI9341_Draw_Horizontal_Line(X0, Y1, X_length, Colour);
	
	
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;		
	}
	ILI9341_Draw_Vertical_Line(X0, Y0, Y_length, Colour);
	ILI9341_Draw_Vertical_Line(X1, Y0, Y_length, Colour);
	
	if((X_length > 0)||(Y_length > 0)) 
	{
		ILI9341_Draw_Pixel(X1, Y1, Colour);
	}
	
}

/*Draw a filled rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	int32_t 	Calc_Negative = 0;
	
	uint16_t X0_true = 0;
	uint16_t Y0_true = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;
		X0_true = X0;
	}
	else
	{
		X_length = X0 - X1;
		X0_true = X1;
	}
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;
		Y0_true = Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;
		Y0_true = Y1;	
	}
	
	ILI9341_Draw_Rectangle(X0_true, Y0_true, X_length, Y_length, Colour);	
}

/*Draws a character (fonts imported from fonts.h) at X,Y location with specified font colour, Size and Background colour*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void ILI9341_Draw_Char(char Character, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
		uint8_t 	function_char;
    uint8_t 	i,j;
		
		function_char = Character;
		
    if (function_char < ' ') {
        Character = 0;
    } else {
        function_char -= 32;
		}
   	
		char temp[CHAR_WIDTH12x16];
		for(uint8_t k = 0; k<CHAR_WIDTH12x16; k++)
		{
		temp[k] = font12x16[function_char][k];
		}
		
    // Draw pixels
		ILI9341_Draw_Rectangle(X, Y, CHAR_WIDTH12x16*Size, CHAR_HEIGHT12x16*Size, Background_Colour);
    for (j=0; j<CHAR_WIDTH12x16; j++) {
        for (i=0; i<CHAR_HEIGHT12x16; i++) {
            if (temp[j] & (1<<i)) {			
							if(Size == 1)
							{
              ILI9341_Draw_Pixel(X+j, Y+i, Colour);
							}
							else
							{
							ILI9341_Draw_Rectangle(X+(j*Size), Y+(i*Size), Size, Size, Colour);
							}
            }						
        }
    }
}



/*
 * Fonction pour afficher du texte sur l'écran LCD
 * Si la variable Size est de 1, l'écriture se fera en DMA, par conséquent beaucoup plus rapidement
 * Voici les temps aproximatifs que le programme restera dans cette fonction:
 * Si Size==1 -> 	1 char = 0.39ms
 * 					1 ligne complète(25char)=7.4ms
 * Si Size==2 ->	2.4mS par caractère
 *    Size==3 ->	4.9mS par caractère
 *    Size==4 ->	8.6ms par caractère
 */

void ILI9341_Draw_Text(const char* Text, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
	if(Size>4)Size=4;
	if(Size<1)Size=1;
	if(Size==1)LCD_WRITE_DMA_STRING(Text, X, Y, Colour, Size, Background_Colour);
	else{
		while(LCD_SPI_DMA_IN_PROGRESS){}		//ON NE PEUT PAS ECRIRE SUR LE LCD TANT QUE LE DMA L'OCCUPE
		while (*Text) {

			ILI9341_Draw_Char_burst(*Text++, X, Y, Colour, Size, Background_Colour);
			X += (CHAR_WIDTH12x16+1)*Size;
		}
	}
}

/*Draws a full screen picture from flash. Image converted from RGB .jpeg/other to C array using online converter*/
//USING CONVERTER: http://www.digole.com/tools/PicturetoC_Hex_converter.php
//65K colour (2Bytes / Pixel)
void ILI9341_Draw_Image(const char* Image_Array, uint8_t Orientation)
{
	if(Orientation == SCREEN_HORIZONTAL_1)
	{
		ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
		ILI9341_Set_Address(0,0,ILI9341_SCREEN_WIDTH,ILI9341_SCREEN_HEIGHT);
			
		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
		
		unsigned char Temp_small_buffer[BURST_MAX_SIZE];
		uint32_t counter = 0;
		for(uint32_t i = 0; i < ILI9341_SCREEN_WIDTH*ILI9341_SCREEN_HEIGHT*2/BURST_MAX_SIZE; i++)
		{			
				for(uint32_t k = 0; k< BURST_MAX_SIZE; k++)
				{
					Temp_small_buffer[k]	= Image_Array[counter+k];		
				}						
				HAL_SPI_Transmit(&hspi3, (unsigned char*)Temp_small_buffer, BURST_MAX_SIZE, 10);
				counter += BURST_MAX_SIZE;			
		}
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	}
	else if(Orientation == SCREEN_HORIZONTAL_2)
	{
		ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
		ILI9341_Set_Address(0,0,ILI9341_SCREEN_WIDTH,ILI9341_SCREEN_HEIGHT);
			
		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
		
		unsigned char Temp_small_buffer[BURST_MAX_SIZE];
		uint32_t counter = 0;
		for(uint32_t i = 0; i < ILI9341_SCREEN_WIDTH*ILI9341_SCREEN_HEIGHT*2/BURST_MAX_SIZE; i++)
		{			
				for(uint32_t k = 0; k< BURST_MAX_SIZE; k++)
				{
					Temp_small_buffer[k]	= Image_Array[counter+k];		
				}						
				HAL_SPI_Transmit(&hspi3, (unsigned char*)Temp_small_buffer, BURST_MAX_SIZE, 10);
				counter += BURST_MAX_SIZE;			
		}
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	}
	else if(Orientation == SCREEN_VERTICAL_2)
	{
		ILI9341_Set_Rotation(SCREEN_VERTICAL_2);
		ILI9341_Set_Address(0,0,ILI9341_SCREEN_HEIGHT,ILI9341_SCREEN_WIDTH);
			
		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
		
		unsigned char Temp_small_buffer[BURST_MAX_SIZE];
		uint32_t counter = 0;
		for(uint32_t i = 0; i < ILI9341_SCREEN_WIDTH*ILI9341_SCREEN_HEIGHT*2/BURST_MAX_SIZE; i++)
		{			
				for(uint32_t k = 0; k< BURST_MAX_SIZE; k++)
				{
					Temp_small_buffer[k]	= Image_Array[counter+k];		
				}						
				HAL_SPI_Transmit(&hspi3, (unsigned char*)Temp_small_buffer, BURST_MAX_SIZE, 10);
				counter += BURST_MAX_SIZE;			
		}
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	}
	else if(Orientation == SCREEN_VERTICAL_1)
	{
		ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
		ILI9341_Set_Address(0,0,ILI9341_SCREEN_HEIGHT,ILI9341_SCREEN_WIDTH);
			
		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
		
		unsigned char Temp_small_buffer[BURST_MAX_SIZE];
		uint32_t counter = 0;
		for(uint32_t i = 0; i < ILI9341_SCREEN_WIDTH*ILI9341_SCREEN_HEIGHT*2/BURST_MAX_SIZE; i++)
		{			
				for(uint32_t k = 0; k< BURST_MAX_SIZE; k++)
				{
					Temp_small_buffer[k]	= Image_Array[counter+k];		
				}						
				HAL_SPI_Transmit(&hspi3, (unsigned char*)Temp_small_buffer, BURST_MAX_SIZE, 10);
				counter += BURST_MAX_SIZE;			
		}
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	}
}



//USED BY DMA FUNCTION
//Converti un caractère dans la table ASCII dans un buffer en un format compatible avec l'écran
void ILI9341_Draw_Char_IN_BUFFER(char Character, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
	uint8_t 	function_char;
	uint16_t 	i,j;
	uint16_t *ptr1;
	char *ptr_ascii;
	if(Character>127){		//conversion entre ISO8859-1 ET le CP437
			if(Character==233)Character=130;
			if(Character==201)Character=144;
			if(Character==232)Character=138;
			if(Character==200)Character=69;
			if(Character==234)Character=136;
			if(Character==202)Character=69;
			if(Character==224)Character=133;
			if(Character==192)Character=65;
			if(Character==226)Character=131;
			if(Character==194)Character=65;
			if(Character==244)Character=147;
			if(Character==231)Character=135;
			if(Character==199)Character=128;
			if(Character==249)Character=151;
			if(Character==217)Character=85;
			if(Character==251)Character=150;
			if(Character==219)Character=85;

	}
	function_char = Character;

	if (function_char < ' ') {
		Character = 0;
	}
	else {
		function_char -= 32;
	}

	ptr_ascii = (char*)&font12x16[function_char][0];
	ptr1 = buffer_one_char;
	for (j=0; j<16; j++) {
		for (i=0; i<8; i++) {
			if (*ptr_ascii & (0x01<<i)) {		//if (temp[j*2] & (0x01<<i)) {
				*(ptr1+11-i) = Colour;
			}
			else *(ptr1+11-i) = Background_Colour;
		}
		ptr_ascii++;
		for (i=0; i<4; i++) {
				if (*ptr_ascii & (0x01<<i)) {	//if (temp[(j*2)+1] & (0x01<<i)) {
					*(ptr1+3-i) = Colour;
				}
				else *(ptr1+3-i) = Background_Colour;
			}
		ptr_ascii++;
		ptr1+=CHAR_WIDTH12x16;
	}

}

void LCD_WRITE_DMA_STRING(const char* Text, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
	uint16_t *ptr_1ligne;
	int cpt_char=0;
	int x_save,y_save;
	int nb_pixel_string;

	while(LCD_SPI_DMA_IN_PROGRESS){}
	LCD_SPI_DMA_IN_PROGRESS=1;

	nb_pixel_string = strlen(Text)*CHAR_WIDTH12x16;			//Combien de pixel de large

	x_save = X;
	y_save = Y;
	ptr_1ligne = buffer_1_ligne;

	 while (*Text) {				//Tant que la string n'est pas terminé
		 	 cpt_char++;			//On compte les caractère
		 	 ILI9341_Draw_Char_IN_BUFFER(*Text++, X, Y, Colour, Size, Background_Colour);	//Convertit le char dans un buffer en pixel
	        X += (CHAR_WIDTH12x16+1)*Size;

	        for(uint8_t k=0;k<16;k++){		//Copy le caractère dans la ligne a envoyé par DMA
				for(uint8_t z=0;z<12;z++){
					*ptr_1ligne++ = buffer_one_char[z+(k*12)];
				}
				ptr_1ligne+=nb_pixel_string-12;
	        }
	        ptr_1ligne = &(buffer_1_ligne[12*cpt_char]);
	    }

	 ILI9341_Set_Address(x_save, y_save, x_save+nb_pixel_string-1, y_save+16-1);
	 ILI9341_Draw_buffer_Burst_DMA(buffer_1_ligne, nb_pixel_string*16 );	//Envoie la ligne au complet par DMA

}

void ILI9341_Draw_Char_burst(char Character, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
	uint8_t 	function_char;
	uint16_t 	i,j;
	uint16_t *ptr1;
	char temp[32];
	if(Character>127){		//conversion entre ISO8859-1 ET le CP437
		if(Character==233)Character=130;
		if(Character==201)Character=144;
		if(Character==232)Character=138;
		if(Character==200)Character=69;
		if(Character==234)Character=136;
		if(Character==202)Character=69;
		if(Character==224)Character=133;
		if(Character==192)Character=65;
		if(Character==226)Character=131;
		if(Character==194)Character=65;
		if(Character==244)Character=147;
		if(Character==231)Character=135;
		if(Character==199)Character=128;
		if(Character==249)Character=151;
		if(Character==217)Character=85;
		if(Character==251)Character=150;
		if(Character==219)Character=85;

	}
	function_char = Character;

	if (function_char < ' ') {
		Character = 0;
	}
	else {
		function_char -= 32;
	}

	for(uint8_t k = 0; k<32; k++)			//Copie du caratère dans un buffer temporaire
	{
		temp[k] = font12x16[function_char][k];
	}

// Draw pixels
	for(uint16_t q=0;q<CHAR_HEIGHT12x16*CHAR_WIDTH12x16*Size*Size;q++){
			buffer_one_char[q]=Background_Colour;			//EFFACE LE BUFFER
	}

	ptr1 = buffer_one_char;
	for (j=0; j<16; j++) {				//Pour les 16 lignes
		for (i=0; i<8; i++) {			//pour les 8 dernière colonne
			if (temp[j*2] & (0x01<<i)) {
				if(Size==1)*(ptr1+11-i) = Colour;
				else {
					for(uint8_t ww=0;ww<Size;ww++){			//For width
						for(uint8_t xx=0;xx<Size;xx++){	//Si le Size est >1, on fait un carré pour agrandir le pixel
							*(ptr1+(11*Size)-(i*Size)+(CHAR_WIDTH12x16*Size*ww)+xx) =Colour;
						}
					}
				}
			}
		}

		for (i=0; i<4; i++) {			//pour les 4 premières colonne
				if (temp[(j*2)+1] & (0x01<<i)) {
					if(Size==1)*(ptr1+3-i) = Colour;
					else{
						for(uint8_t ww=0;ww<Size;ww++){
							for(uint8_t xx=0;xx<Size;xx++){			//Si le Size est >1, on fait un carré pour agrandir le pixel
									*(ptr1+(3*Size)-(i*Size)+(CHAR_WIDTH12x16*Size*ww)+xx) =Colour;
							 }
						}
					}
				}
        }
    ptr1+=CHAR_WIDTH12x16*Size*Size;		//Changement de ligne pour les pixel
}
ILI9341_Set_Address(X, Y, X+(12*Size)-1, Y+(16*Size)-1);
ILI9341_Draw_buffer_Burst(buffer_one_char, 12*16*Size*Size );		//Ecrit le buffer sur le port SPI

}



