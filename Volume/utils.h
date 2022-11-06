#pragma once
#include "volume.h"


void Menu1(); //Hien thi menu chuc nang

void createVolume(); // Chuc nang 1 tao volume


void writeOffset(char* buffer, int offSet, char* data, int n); // Ghi n bytes của data vào buffer tại offset  
