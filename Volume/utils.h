#pragma once
#include "volume.h"


void Menu1(); //Hien thi menu chuc nang

void createVolume(); // Chuc nang 1 tao volume
void MenuPasswordVolume(); //chuc nang 2 kiem/doi mat khau
void viewListMenu(); //Chuc nang 3 xem ds file
void importFileToVolume(); //chuc nang 5 import file vao vol
void changeFilePassword();
void exportFile(); // chuc nang 6 export file ra ngoai
void deleteFile();//Chuc nag 7 xoa file

void writeOffset(char* buffer, int offSet, char* data, int n); // Ghi n bytes của data vào buffer tại offset  
