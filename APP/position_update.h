#ifndef __POSITION_UPDATE_H
#define __POSITION_UPDATE_H

#define TAG_X_NUM 10  //X方向标签数
#define TAG_Y_NUM 10  //y方向标签数

#define UNIT_LENGTH_X 30  //标签X方向之间距离，每单位长度参照control.h
#define UNIT_LENGTH_Y 30 //标签Y方向之间距离，每单位长度参照control.h

#define ANGLE_CORRECTION_THRESHOLD   3
#define POSITION_CORRECTION_THRESHOLD 7

extern unsigned int IDArray[];

unsigned int IDtoInt(char* ID);
int IntIDtoXY(unsigned int* IDArray, int ArrayLen, unsigned int id, float* X, float* Y);

#endif
