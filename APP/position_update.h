#ifndef __POSITION_UPDATE_H
#define __POSITION_UPDATE_H

#define TAG_X_NUM 10  //X�����ǩ��
#define TAG_Y_NUM 10  //y�����ǩ��

#define UNIT_LENGTH_X 30  //��ǩX����֮����룬ÿ��λ���Ȳ���control.h
#define UNIT_LENGTH_Y 30 //��ǩY����֮����룬ÿ��λ���Ȳ���control.h

#define ANGLE_CORRECTION_THRESHOLD   3
#define POSITION_CORRECTION_THRESHOLD 7

extern unsigned int IDArray[];

unsigned int IDtoInt(char* ID);
int IntIDtoXY(unsigned int* IDArray, int ArrayLen, unsigned int id, float* X, float* Y);

#endif
