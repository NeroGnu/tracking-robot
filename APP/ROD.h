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

//两点间距离； 
float ComputeDistanceOfPoints(Coordinate Point1, Coordinate Point2);
//根据两点计算直线； 
LinearEquation ComputeLinearEquation(Coordinate C1, Coordinate C2);
//计算直线上到指定点一定距离的点的坐标； 
int PointOnline(LinearEquation Line, Coordinate MidPoint, float Distance, Coordinate* LPoint, Coordinate* RPoint);
//计算与给定直线相距一定距离的平行线； 
LinearEquation ComputeParallelLine(LinearEquation SrcLine, float Distance);
//计算通过给定直线上一点的垂线； 
LinearEquation ComputeVerticalLine(LinearEquation SrcLine, Coordinate IntersectionPoint);
//计算两直线交点，无交点则返回0； 
int ComputeIntersection(LinearEquation Line1, LinearEquation Line2, Coordinate* IntersectionPoint);
//计算点与直线的位置关系； 
float ComputeRelationBetweenPointAndLine(LinearEquation Line, Coordinate Point);
//初始化矩形结构体，使用四个角坐标； 
myRectangle InitRectWithPoints(Coordinate A, Coordinate B, Coordinate C, Coordinate D);
//初始化矩阵结构体，使用移动轨迹和延展距离； 
myRectangle InitRectWithTrace(Coordinate Point1, Coordinate Point2, float Extension);
//检查两个矩形是否重叠； 
int ifRectOverlapping(myRectangle Rect1, myRectangle Rect2);
#endif
