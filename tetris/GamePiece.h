//Copyright (C) 2013 Valentin Ivanov http://www.breakcontinue.com.
//
//This work is licensed under a Creative Commons 
//Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//See http://creativecommons.org/licenses/by-nc-sa/3.0/
//
//All other rights reserved.
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
//EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
//WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

#ifndef _GAMEPIECE_H_
#define _GAMEPIECE_H_

#include "application.h"

class GamePiece
{
public:
	GamePiece(uint8_t rows, uint8_t columns, uint8_t * inputArray = NULL);
	GamePiece(const GamePiece& copyFrom);
	GamePiece * rotateLeft();
	GamePiece * rotateRight();
	uint8_t& operator() ( int rowIndex,  int columnIndex);
	int  getV(const int rowIndex, const int columnIndex);

	uint8_t Rows;
	uint8_t Columns;
	uint8_t * _data;  
};

#endif // _GAMEPIECE_H_