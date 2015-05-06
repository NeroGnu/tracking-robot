#ifndef __ROD_H__
#define __ROD_H__



typedef struct
{
	float X;
	float Y;
} Coordinate;

typedef struct
{
	//ax+by+c=0;
	float a;
	float b;
	float c;
} LinearEquation;

typedef struct
{
	Coordinate A;
	Coordinate B;
	Coordinate C;
	Coordinate D;
	LinearEquation AB;
	LinearEquation BC;
	LinearEquation CD;
	LinearEquation DA;
} myRectangle;

//�������룻 
float ComputeDistanceOfPoints(Coordinate Point1, Coordinate Point2);
//�����������ֱ�ߣ� 
LinearEquation ComputeLinearEquation(Coordinate C1, Coordinate C2);
//����ֱ���ϵ�ָ����һ������ĵ�����ꣻ 
int PointOnline(LinearEquation Line, Coordinate MidPoint, float Distance, Coordinate* LPoint, Coordinate* RPoint);
//���������ֱ�����һ�������ƽ���ߣ� 
LinearEquation ComputeParallelLine(LinearEquation SrcLine, float Distance);
//����ͨ������ֱ����һ��Ĵ��ߣ� 
LinearEquation ComputeVerticalLine(LinearEquation SrcLine, Coordinate IntersectionPoint);
//������ֱ�߽��㣬�޽����򷵻�0�� 
int ComputeIntersection(LinearEquation Line1, LinearEquation Line2, Coordinate* IntersectionPoint);
//�������ֱ�ߵ�λ�ù�ϵ�� 
float ComputeRelationBetweenPointAndLine(LinearEquation Line, Coordinate Point);
//��ʼ�����νṹ�壬ʹ���ĸ������ꣻ 
myRectangle InitRectWithPoints(Coordinate A, Coordinate B, Coordinate C, Coordinate D);
//��ʼ������ṹ�壬ʹ���ƶ��켣����չ���룻 
myRectangle InitRectWithTrace(Coordinate Point1, Coordinate Point2, float Extension);
//������������Ƿ��ص��� 
int ifRectOverlapping(myRectangle Rect1, myRectangle Rect2);
#endif
