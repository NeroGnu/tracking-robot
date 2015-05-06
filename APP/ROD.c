#include "ROD.h"
//#include "arm_math.h"
#include <math.h>


#define STM32F4

#ifdef STM32F4

#include "arm_math.h"
#define mySQRT(a)     my_sqrt(a)
#define myCOS(a)      arm_cos_f32(a)
#define mySIN(a)      arm_sin_f32(a)

float my_sqrt(float a)
{
	float Res;
	if (ARM_MATH_SUCCESS == arm_sqrt_f32(a, &Res))
	{
		return Res;
	}
	else
	{
		return -1.0;
	}
}

#else

#define mySQRT(a)     sqrt(a)
#define myCOS(a)      cos(a) 
#define mySIN(a)      sin(a)
#define PI            M_PI

#endif


#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))


float ComputeDistanceOfPoints(Coordinate Point1, Coordinate Point2)
{
	return mySQRT((Point2.X - Point1.X) * (Point2.X - Point1.X) + (Point2.Y - Point1.Y) * (Point2.Y - Point1.Y));
}

LinearEquation ComputeLinearEquation(Coordinate C1, Coordinate C2)
{
	LinearEquation Res;
	if (C1.X != C2.X && C1.Y != C2.Y)
	{
		Res.a = (C2.Y - C1.Y) / (C2.X - C1.X);
		Res.b = -1;
		Res.c = (C2.X * C1.Y - C1.X * C2.Y) / (C2.X - C1.X);
		return Res;
	}
	
	if (C1.X == C2.X)
	{
		Res.a = 1;
		Res.b = 0;
		Res.c = 0 - C1.X;
		return Res;
	}
	
	if (C1.Y == C2.Y)
	{
		Res.a = 0;
		Res.b = 1;
		Res.c = 0 - C1.Y;
		return Res;
	}
}

int PointOnline(LinearEquation Line, Coordinate MidPoint, float Distance, Coordinate* LPoint, Coordinate* RPoint)
{
	float theta, deltaX, deltaY;
	if (Distance <= 0)
	{
		return -1;
	}
	theta = atan2(Line.a, 0 - Line.b);
	//printf("theta %f\r\n",theta);
	deltaX = Distance * myCOS(theta);
	deltaY = Distance * mySIN(theta);
	if (deltaX > 0)
	{
		RPoint->X = MidPoint.X + deltaX;
		RPoint->Y = MidPoint.Y + deltaY;
		
		LPoint->X = MidPoint.X - deltaX;
		LPoint->Y = MidPoint.Y - deltaY;
	}
	if (deltaX = 0)
	{
		if (deltaY > 0)
		{
			RPoint->X = MidPoint.X - deltaX;
			RPoint->Y = MidPoint.Y - deltaY;
			
			LPoint->X = MidPoint.X + deltaX;
			LPoint->Y = MidPoint.Y + deltaY;
		}
		else
		{
			RPoint->X = MidPoint.X + deltaX;
			RPoint->Y = MidPoint.Y + deltaY;
			
			LPoint->X = MidPoint.X - deltaX;
			LPoint->Y = MidPoint.Y - deltaY;
		}
	}
	if (deltaX < 0)
	{
		RPoint->X = MidPoint.X - deltaX;
		RPoint->Y = MidPoint.Y - deltaY;
		
		LPoint->X = MidPoint.X + deltaX;
		LPoint->Y = MidPoint.Y + deltaY;
	}
	
	return 0;
}

LinearEquation ComputeParallelLine(LinearEquation SrcLine, float Distance)
{
	LinearEquation Res;
	Res.a = SrcLine.a;
	Res.b = SrcLine.b;
	Res.c = SrcLine.c + Distance * mySQRT(SrcLine.a * SrcLine.a + SrcLine.b * SrcLine.b);
	return Res;
}

LinearEquation ComputeVerticalLine(LinearEquation SrcLine, Coordinate IntersectionPoint)
{
	LinearEquation Res;
	Res.a = SrcLine.b;
	Res.b = 0 - SrcLine.a;
	Res.c = SrcLine.a * IntersectionPoint.Y - SrcLine.b * IntersectionPoint.X;
	return Res;
}

int ComputeIntersection(LinearEquation Line1, LinearEquation Line2, Coordinate* IntersectionPoint)
{
	if (Line1.a / Line1.b == Line2.a / Line2.b)
	{
		return 0;
	}
	
	IntersectionPoint->X = (Line2.b * Line1.c - Line2.c * Line1.b) / (Line2.a * Line1.b - Line2.b * Line1.a);
	IntersectionPoint->Y = (Line1.a * Line2.c - Line2.a * Line1.c) / (Line2.a * Line1.b - Line2.b * Line1.a);
	return 1;
}

float ComputeRelationBetweenPointAndLine(LinearEquation Line, Coordinate Point)
{
	return Line.a * Point.X + Line.b * Point.Y + Line.c;
}

myRectangle InitRectWithPoints(Coordinate A, Coordinate B, Coordinate C, Coordinate D)
{
	myRectangle rec;
	rec.A = A;
	rec.B = B;
	rec.C = C;
	rec.C = C;
	
	rec.AB = ComputeLinearEquation(A, B);
	rec.BC = ComputeLinearEquation(B, C);
	rec.CD = ComputeLinearEquation(C, D);
	rec.DA = ComputeLinearEquation(D, A);
	return rec;
}

myRectangle InitRectWithTrace(Coordinate Point1, Coordinate Point2, float Extension)
{
	myRectangle rec;
	LinearEquation MidLine;
	Coordinate AB_X_MidLine, CD_X_MidLine, tempL, tempR;
	MidLine = ComputeLinearEquation(Point1, Point2);
	//printf ("a=%f b=%f c=%f\r\n", MidLine.a, MidLine.b, MidLine.c);
	PointOnline(MidLine, Point2, Extension, &tempL, &tempR);
	//printf("tempL %f, %f\r\ntempR %f, %f", tempL.X, tempL.Y, tempR.X, tempR.Y);
	AB_X_MidLine = ComputeDistanceOfPoints(Point1, tempL) > ComputeDistanceOfPoints(Point1, tempR) ? tempL : tempR;
	PointOnline(MidLine, Point1, Extension, &tempL, &tempR);
	CD_X_MidLine = ComputeDistanceOfPoints(Point2, tempL) > ComputeDistanceOfPoints(Point2, tempR) ? tempL : tempR;
	rec.AB = ComputeVerticalLine(MidLine, AB_X_MidLine);
	rec.CD = ComputeVerticalLine(MidLine, CD_X_MidLine);
	rec.DA = ComputeParallelLine(MidLine, Extension);
	rec.BC = ComputeParallelLine(MidLine, 0 - Extension);
	//printf("AB_X_MidLine %f, %f\r\n", AB_X_MidLine.X, AB_X_MidLine.Y);
	ComputeIntersection(rec.AB, rec.DA, &rec.A);
	ComputeIntersection(rec.AB, rec.BC, &rec.B);
	ComputeIntersection(rec.BC, rec.CD, &rec.C);
	ComputeIntersection(rec.CD, rec.DA, &rec.D);
	
	return rec;
}

int ifRectOverlapping(myRectangle Rect1, myRectangle Rect2)
{
	Coordinate IntersectionPoint;
	if ((ComputeRelationBetweenPointAndLine(Rect1.AB, Rect2.A) * ComputeRelationBetweenPointAndLine(Rect1.CD, Rect2.A) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect1.BC, Rect2.A) * ComputeRelationBetweenPointAndLine(Rect1.DA, Rect2.A) <= 0.00))
	 {
	 	return 1;
	 }
	if ((ComputeRelationBetweenPointAndLine(Rect1.AB, Rect2.B) * ComputeRelationBetweenPointAndLine(Rect1.CD, Rect2.B) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect1.BC, Rect2.B) * ComputeRelationBetweenPointAndLine(Rect1.DA, Rect2.B) <= 0.00))
	 {
	 	return 2;
	 }
	if ((ComputeRelationBetweenPointAndLine(Rect1.AB, Rect2.C) * ComputeRelationBetweenPointAndLine(Rect1.CD, Rect2.C) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect1.BC, Rect2.C) * ComputeRelationBetweenPointAndLine(Rect1.DA, Rect2.C) <= 0.00))
	 {
	 	return 3;
	 }
	if ((ComputeRelationBetweenPointAndLine(Rect1.AB, Rect2.D) * ComputeRelationBetweenPointAndLine(Rect1.CD, Rect2.D) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect1.BC, Rect2.D) * ComputeRelationBetweenPointAndLine(Rect1.DA, Rect2.D) <= 0.00))
	 {
	 	return 4;
	 }
	
	if ((ComputeRelationBetweenPointAndLine(Rect2.AB, Rect1.A) * ComputeRelationBetweenPointAndLine(Rect2.CD, Rect1.A) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect2.BC, Rect1.A) * ComputeRelationBetweenPointAndLine(Rect2.DA, Rect1.A) <= 0.00))
	 {
	 	return 5;
	 }
	if ((ComputeRelationBetweenPointAndLine(Rect2.AB, Rect1.B) * ComputeRelationBetweenPointAndLine(Rect2.CD, Rect1.B) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect2.BC, Rect1.B) * ComputeRelationBetweenPointAndLine(Rect2.DA, Rect1.B) <= 0.00))
	 {
	 	return 6;
	 }
	if ((ComputeRelationBetweenPointAndLine(Rect2.AB, Rect1.C) * ComputeRelationBetweenPointAndLine(Rect2.CD, Rect1.C) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect2.BC, Rect1.C) * ComputeRelationBetweenPointAndLine(Rect2.DA, Rect1.C) <= 0.00))
	 {
	 	return 7;
	 }
	if ((ComputeRelationBetweenPointAndLine(Rect2.AB, Rect1.D) * ComputeRelationBetweenPointAndLine(Rect2.CD, Rect1.D) <= 0.00)\
	 && (ComputeRelationBetweenPointAndLine(Rect2.BC, Rect1.D) * ComputeRelationBetweenPointAndLine(Rect2.DA, Rect1.D) <= 0.00))
	 {
	 	return 8;
	 }
	 
	if (ComputeIntersection(Rect1.AB, Rect2.AB, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.A.X,Rect1.B.X) && \
		    IntersectionPoint.X <= MAX(Rect1.A.X,Rect1.B.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.A.X,Rect2.B.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.A.X,Rect2.B.X)) 
		    		{
						return 9;
		    		}
		    	
		    }
		    
		
	}
	if (ComputeIntersection(Rect1.AB, Rect2.BC, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.A.X,Rect1.B.X) && \
		    IntersectionPoint.X <= MAX(Rect1.A.X,Rect1.B.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.B.X,Rect2.C.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.B.X,Rect2.C.X)) 
		    		{
						return 10;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.AB, Rect2.CD, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.A.X,Rect1.B.X) && \
		    IntersectionPoint.X <= MAX(Rect1.A.X,Rect1.B.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.C.X,Rect2.D.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.C.X,Rect2.D.X)) 
		    		{
						return 11;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.AB, Rect2.DA, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.A.X,Rect1.B.X) && \
		    IntersectionPoint.X <= MAX(Rect1.A.X,Rect1.B.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.D.X,Rect2.A.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.D.X,Rect2.A.X)) 
		    		{
						return 12;
		    		}
		    	
		    }
		
	}
	
	if (ComputeIntersection(Rect1.BC, Rect2.AB, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.B.X,Rect1.C.X) && \
		    IntersectionPoint.X <= MAX(Rect1.B.X,Rect1.C.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.A.X,Rect2.B.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.A.X,Rect2.B.X)) 
		    		{
						return 13;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.BC, Rect2.BC, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.B.X,Rect1.C.X) && \
		    IntersectionPoint.X <= MAX(Rect1.B.X,Rect1.C.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.B.X,Rect2.C.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.B.X,Rect2.C.X))
					{
						return 14;
					} 
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.BC, Rect2.CD, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.B.X,Rect1.C.X) && \
		    IntersectionPoint.X <= MAX(Rect1.B.X,Rect1.C.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.C.X,Rect2.D.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.C.X,Rect2.D.X)) 
		    		{
						return 15;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.BC, Rect2.DA, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.B.X,Rect1.C.X) && \
		    IntersectionPoint.X <= MAX(Rect1.B.X,Rect1.C.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.D.X,Rect2.A.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.D.X,Rect2.A.X)) 
		    		{
						return 16;
		    		}
		    	
		    }
		
	}
	
	if (ComputeIntersection(Rect1.CD, Rect2.AB, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.C.X,Rect1.D.X) && \
		    IntersectionPoint.X <= MAX(Rect1.C.X,Rect1.D.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.A.X,Rect2.B.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.A.X,Rect2.B.X)) 
		    		{
						return 17;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.CD, Rect2.BC, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.C.X,Rect1.D.X) && \
		    IntersectionPoint.X <= MAX(Rect1.C.X,Rect1.D.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.B.X,Rect2.C.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.B.X,Rect2.C.X)) 
		    		{
						return 18;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.CD, Rect2.CD, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.C.X,Rect1.D.X) && \
		    IntersectionPoint.X <= MAX(Rect1.C.X,Rect1.D.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.C.X,Rect2.D.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.C.X,Rect2.D.X)) 
		    		{
						return 19;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.CD, Rect2.DA, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.C.X,Rect1.D.X) && \
		    IntersectionPoint.X <= MAX(Rect1.C.X,Rect1.D.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.D.X,Rect2.A.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.D.X,Rect2.A.X))
					{
						return 20;
					} 
		    		
		    }
		
	}
	
	if (ComputeIntersection(Rect1.DA, Rect2.AB, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.D.X,Rect1.A.X) && \
		    IntersectionPoint.X <= MAX(Rect1.D.X,Rect1.A.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.A.X,Rect2.B.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.A.X,Rect2.B.X)) 
		    		{
						return 21;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.DA, Rect2.BC, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.D.X,Rect1.A.X) && \
		    IntersectionPoint.X <= MAX(Rect1.D.X,Rect1.A.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.B.X,Rect2.C.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.B.X,Rect2.C.X)) 
		    		{
						return 22;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.DA, Rect2.CD, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.D.X,Rect1.A.X) && \
		    IntersectionPoint.X <= MAX(Rect1.D.X,Rect1.A.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.C.X,Rect2.D.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.C.X,Rect2.D.X)) 
		    		{
						return 23;
		    		}
		    	
		    }
		
	}
	if (ComputeIntersection(Rect1.DA, Rect2.DA, &IntersectionPoint))
	{
		if (IntersectionPoint.X >= MIN(Rect1.D.X,Rect1.A.X) && \
		    IntersectionPoint.X <= MAX(Rect1.D.X,Rect1.A.X))
		    {
		    	if (IntersectionPoint.X >= MIN(Rect2.D.X,Rect2.A.X) && \
		    		IntersectionPoint.X <= MAX(Rect2.D.X,Rect2.A.X))
					{
						return 24;
					} 
		    	
		    }
		
	}
	 return 0;
}
