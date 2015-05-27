#include "control.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "ROD.h"
#include "position_update.h"
#include "arm_math.h"

#ifdef No1
Position CurPos = {90,0,-10};
#endif

#ifdef No2
Position CurPos = {90,240,-10};
#endif

#ifdef No3
Position CurPos = {90,240,-10};
#endif

#ifdef No4
Position CurPos = {90,90,-10};
#endif

#ifdef No5
Position CurPos = {90,150,-10};
#endif


Direction GlobleCmdDir;
Procedure GlobleCurProcedure;
Procedure GloblePathProcedure;

TraceList ListHead = NULL;


int IsOutSide(Position pos)
{
	if(pos.x > MAX_RIGHT_X || pos.x < MIN_LEFT_DOWN_X || pos.y > MAX_RIGHT_Y || pos.y < MIN_LEFT_DOWN_X)
		return 1;
	else 
		return 0;
}

int LEFT_DOWN_X,LEFT_DOWN_Y,RIGHT_UP_X,RIGHT_UP_Y;

Position GoThroght[60];

double computeAngle(double distance, double deltaX)
{
	return ((-11.6/pow(distance, 2.275) + 0.09782) * deltaX);
}

void none_printf(char *fmt, ...)
{
	return;
}

int ifCollision(float x1, float y1, float x2, float y2)
{
	TraceList p;
	Coordinate SPoint1, SPoint2, CPoint1, CPoint2;
	myRectangle SRect, CRect;
	
	SPoint1.X = x1;
	SPoint1.Y = y1;
	SPoint2.X = x2;
	SPoint2.Y = y2;
	SRect = InitRectWithTrace(SPoint1, SPoint2, SAFEDISTANCE);
	
	p = ListHead->next;
	
	while (p)
	{
		CPoint1.X = p->x1;
		CPoint1.Y = p->y1;
		CPoint2.X = p->x2;
		CPoint2.Y = p->y2;
		CRect = InitRectWithTrace(CPoint1, CPoint2, SAFEDISTANCE);
		
		if (0 == ifRectOverlapping(CRect, SRect))
		{
			p = p->next;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

int generate_Pos(int leftDown_x,int leftDown_y,int right_Up_x,int right_Up_y)
{
	int i = 0,tmp = 0,j;
	int count = 0,x_times,y_times;
	//count = *number;
	x_times = (right_Up_x-leftDown_x)/PER_DISTANCE;
	y_times = (right_Up_y - leftDown_y)/PER_DISTANCE;
	for(i = 0; (leftDown_y + (i*2+1)*UNIT_LENGTH_Y) <= right_Up_y; i++)
	{
		GoThroght[count].Angle = 0;
		GoThroght[count].x = leftDown_x;
		GoThroght[count].y = leftDown_y + i*2*UNIT_LENGTH_Y;
		
//		printf("GoThroght[%d]: %f %f %f\r\n",count,GoThroght[count].Angle,GoThroght[count].x,GoThroght[count].y);
		count++;
		for(j = 1; j <= x_times; j++)
		{
			GoThroght[count].Angle = 0;
			GoThroght[count].x = leftDown_x + j*PER_DISTANCE;
			GoThroght[count].y = leftDown_y + i*2*UNIT_LENGTH_Y;
//			printf("GoThroght[%d]: %f %f %f\r\n",count,GoThroght[count].Angle,GoThroght[count].x,GoThroght[count].y);
			count++;
		}
		if(right_Up_x > leftDown_x+x_times*PER_DISTANCE)
		{
			GoThroght[count].Angle = 90;
			GoThroght[count].x = right_Up_x;
			GoThroght[count].y = leftDown_y + i*2*UNIT_LENGTH_Y;
//			printf("GoThroght[%d]: %f %f %f\r\n",count,GoThroght[count].Angle,GoThroght[count].x,GoThroght[count].y);
			count++;
		}
		
		
		
		GoThroght[count].Angle = 180;
		GoThroght[count].x = right_Up_x;
		GoThroght[count].y = leftDown_y + (i*2+1)*UNIT_LENGTH_Y;
//		printf("GoThroght[%d]: %f %f %f\r\n",count,GoThroght[count].Angle,GoThroght[count].x,GoThroght[count].y);
			count++;
		
		for(j = 1; j <= x_times; j++)
		{
			GoThroght[count].Angle = 180;
			GoThroght[count].x = right_Up_x - j*PER_DISTANCE;
			GoThroght[count].y = leftDown_y + (i*2+1)*UNIT_LENGTH_Y;
//			printf("GoThroght[%d]: %f %f %f\r\n",count,GoThroght[count].Angle,GoThroght[count].x,GoThroght[count].y);
			count++;
		}
		if(right_Up_x > leftDown_x+x_times*PER_DISTANCE)
		{
			GoThroght[count].Angle = 90;
			GoThroght[count].x = leftDown_x;
			GoThroght[count].y = leftDown_y + (i*2+1)*UNIT_LENGTH_Y;
//			printf("GoThroght[%d]: %f %f %f\r\n",count,GoThroght[count].Angle,GoThroght[count].x,GoThroght[count].y);
			count++;
		}
		

	}
	if((leftDown_y + i*2*UNIT_LENGTH_Y) <= right_Up_y)
	{
		GoThroght[count].Angle = 0;
		GoThroght[count].x = leftDown_x;
		GoThroght[count].y = leftDown_y + i*2*UNIT_LENGTH_Y;
		
		
		GoThroght[1+count].Angle = 90;
		GoThroght[1+count].x = right_Up_x;
		GoThroght[1+count].y = leftDown_y + i*2*UNIT_LENGTH_Y;
		count = count + 2;
	}
	
	
	if(GoThroght[count-1].x != leftDown_x)
	{
		GoThroght[count].Angle = 270;
		GoThroght[count].x = leftDown_x;
		GoThroght[count].y = right_Up_y;
		count++;
	}
	
	for(j = 1; j <= y_times; j++)
	{
		GoThroght[count].Angle = 270;
		GoThroght[count].x = leftDown_x;
		GoThroght[count].y = right_Up_y - j*PER_DISTANCE;
		count++;
	}
	return count;
}


Position searchTarg(int count)
{
	Position temPos;
	temPos = CurPos;
	if(count%2)
	{
		temPos.Angle = temPos.Angle + count*20;
	}
	else
	{
		temPos.Angle = temPos.Angle - count*20;
	}
	
	if (temPos.Angle > 180)
	{
		temPos.Angle = temPos.Angle - 360;
	}
	
	if (temPos.Angle < -180)
	{
		temPos.Angle = 360 + temPos.Angle;
	}
	return temPos;
}

int divideDis(Position *destPos,Position TargetPos)
{
	Position tmpPos;
	float tmpAngle;
	if(sqrt(pow(TargetPos.y - CurPos.y, 2) + pow(TargetPos.x - CurPos.x, 2)) >= PER_DISTANCE)
	{
		printf("I am IN\r\n");
		tmpAngle = (atan2(TargetPos.y - CurPos.y, TargetPos.x - CurPos.x)/PI)*180;
		tmpPos.x = CurPos.x + MaxDIS_PER_TIMES * arm_cos_f32((tmpAngle / 180) * PI);
		tmpPos.y = CurPos.y + MaxDIS_PER_TIMES * arm_cos_f32((tmpAngle / 180) * PI);
		tmpPos.Angle = tmpAngle;
		//printf("TargetPos: %f %f %f\r\n",tmpPos.Angle,tmpPos.x,tmpPos.y);
		*destPos = tmpPos;
		return 1;
	}
	else
		return 0;

}


void setActiveArea(int left_Down_x,int left_Down_y,int right_Up_x,int right_Up_y)
{
	LEFT_DOWN_X = left_Down_x;
	LEFT_DOWN_Y = left_Down_y;
	RIGHT_UP_X = right_Up_x;
	RIGHT_UP_Y = right_Up_y;
}

	//根据prePos的角度，来判断下一个位置
int flag = 1;
Position OneNextPos(int sweep,int dir,Position PrePos)
{
	Position nextpos;
	int tmp_angle;
	
	tmp_angle = (int)(PrePos.Angle);
	if(sweep)
	{
		switch(tmp_angle)
		{
			case 0: 
				nextpos.x = PrePos.x + PER_DISTANCE;
				if(nextpos.x > RIGHT_UP_X)
					nextpos.x = RIGHT_UP_X;
					
				nextpos.y = PrePos.y;
				
				if(nextpos.x == RIGHT_UP_X)
				{
					if(dir)
						nextpos.Angle = 90;
					else
						nextpos.Angle = -90;
				}
				else
					nextpos.Angle = 0;
				break;
			case 90:
				nextpos.x = PrePos.x;
				nextpos.y = PrePos.y + Y_INTERVAL;
				if(PrePos.x == LEFT_DOWN_X)
					flag = 0;
				if(PrePos.x == RIGHT_UP_X)
					flag = 1;
				if(flag)
				{
					nextpos.Angle = 180;
					flag = 0;
				}
				else
				{
					nextpos.Angle = 0;
					flag = 1;
				}
				break;
			case 180:
				nextpos.x = PrePos.x - PER_DISTANCE;
				if(nextpos.x < LEFT_DOWN_X)
					nextpos.x = LEFT_DOWN_X;
				nextpos.y = PrePos.y;
				if(nextpos.x == LEFT_DOWN_X)
				{
				
					if(dir)
						nextpos.Angle = 90;
					else
						nextpos.Angle = -90;
				}
				else
					nextpos.Angle = 180;
				break;
			case -90:
				nextpos.x = PrePos.x;
				nextpos.y = PrePos.y - Y_INTERVAL;
				if(PrePos.x == LEFT_DOWN_X)
					flag = 0;
				if(PrePos.x == RIGHT_UP_X)
					flag = 1;
				if(flag)
				{
					nextpos.Angle = 180;
					flag = 0;
				}
				else
				{
					nextpos.Angle = 0;
					flag = 1;
				}
				break;
			default:
				break;
		}
	}
	else
	{
		switch(tmp_angle)
		{
			case 0: 
				nextpos.x = PrePos.x + X_INTERVAL;
				nextpos.y = PrePos.y;
				if(PrePos.y == LEFT_DOWN_Y)
					flag = 1;
				if(PrePos.y == RIGHT_UP_Y)
					flag = 0;
				if(flag)
				{
					nextpos.Angle = 90;
					flag = 0;
				}
				else
				{
					nextpos.Angle = -90;
					flag = 1;
				}
				break;
			case 90:
				nextpos.x = PrePos.x;
				nextpos.y = PrePos.y + PER_DISTANCE;
				if(nextpos.y > RIGHT_UP_Y)
					nextpos.y = RIGHT_UP_Y;
				if(nextpos.y == RIGHT_UP_Y)
				{
					if(dir)
						nextpos.Angle = 0;
					else
						nextpos.Angle = 180;
				}
				else
					nextpos.Angle = 90;
				break;
			case 180:
				nextpos.x = PrePos.x - X_INTERVAL;
				nextpos.y = PrePos.y;
				if(PrePos.y == LEFT_DOWN_Y)
					flag = 0;
				if(PrePos.y == RIGHT_UP_Y)
					flag = 1;
				if(flag)
				{
					nextpos.Angle = -90;
					flag = 0;
				}
				else
				{
					nextpos.Angle = 90;
					flag = 1;
				}
				break;
			case -90:
				nextpos.x = PrePos.x;
				nextpos.y = PrePos.y - PER_DISTANCE;
				if(nextpos.y < LEFT_DOWN_Y)
					nextpos.y = LEFT_DOWN_Y;
				if(nextpos.y == LEFT_DOWN_Y)
				{
					if(dir)
						nextpos.Angle = 0;
					else
						nextpos.Angle = 180;
				}
				else
					nextpos.Angle = -90;
				break;
			default:
				break;
		}

	}
	return nextpos;
}


int sweep = sweep_LEFTRIGHT,dir = dir_LITTLETOBIG;
Position getNextPos(Position tmpPos)
{
	Position tmp1Pos;
	//tmpPos = intiPos;
	tmp1Pos = OneNextPos(sweep,dir,tmpPos);
	if(tmp1Pos.y > RIGHT_UP_Y )
	{
		sweep = 0,dir = 0;
		tmpPos.Angle = -90;
		if(tmpPos.x == LEFT_DOWN_X)
			dir = 1;
		else
			dir = 0;
		tmp1Pos = OneNextPos(sweep,dir,tmpPos);
	}
	if(tmp1Pos.x < LEFT_DOWN_X)
	{
		sweep = 1,dir = 1;
		tmpPos.Angle = 0;
		if(tmpPos.y == LEFT_DOWN_Y)
			dir = 1;
		else
			dir = 0;
		tmp1Pos = OneNextPos(sweep,dir,tmpPos);
	}
	
	if(tmp1Pos.x > RIGHT_UP_X)
	{
		sweep = 1,dir = 1;
		tmpPos.Angle = 180;
		if(tmpPos.y == LEFT_DOWN_Y)
			dir = 1;
		else
			dir = 0;
		tmp1Pos = OneNextPos(sweep,dir,tmpPos);
	}
	if(tmp1Pos.y < LEFT_DOWN_Y)
	{
		sweep = 0,dir = 0;
		tmpPos.Angle = 90;
		if(tmpPos.x == LEFT_DOWN_X)
			dir = 0;
		else
			dir = 1;
		tmp1Pos = OneNextPos(sweep,dir,tmpPos);
	}
	return tmp1Pos;
}
