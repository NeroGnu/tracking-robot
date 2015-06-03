#ifndef __CONTROL_H__
#define __CONTROL_H__


#define DEBUG

#define No5 

#define ENABLE_PATROL

#ifdef No1
	#define STEP_ANGLE (2.84444444444444)  //ת�Ǳ���
	#define STEP_METER (2017.593244194229/100)           //�������,��ǰһ����λ����Ϊ10cm
	#define SELFADDRESS  0x01    //0x01~0xFE
#endif

#ifdef No2
	#define STEP_ANGLE (2.73611111111111)  //ת�Ǳ���
	#define STEP_METER (2017.593244194229/100)           //�������,��ǰһ����λ����Ϊ10cm
	#define SELFADDRESS  0x02    //0x01~0xFE
#endif

#ifdef No3
	#define STEP_ANGLE (2.74444444444444)  //ת�Ǳ���
	#define STEP_METER (2017.593244194229/100)           //�������,��ǰһ����λ����Ϊ10cm
	#define SELFADDRESS  0x03    //0x01~0xFE
#endif

#ifdef No4
	#define STEP_ANGLE (2.6059405940594)  //ת�Ǳ���
	#define STEP_METER (2017.593244194229/100)           //�������,��ǰһ����λ����Ϊ10cm
	#define SELFADDRESS  0x04    //0x01~0xFE.
#endif

#ifdef No5
	#define STEP_ANGLE (2.726643599)  //ת�Ǳ���
	#define STEP_METER (2017.593244194229/100)           //�������,��ǰһ����λ����Ϊ10cm
	#define SELFADDRESS  0x05    //0x01~0xFE
#endif

//#define ADJUST_STEP (3)                     //������������С���㲿�ֻ�������ADJUST_STEPʱ���е�����

#define ADJUST_STEP_ANGLE (3)
#define ADJUST_STEP_DIRECT (10)

#define SAFEDISTANCE (20)             //��ȫ���룬С������֮��Ӧ���־��롣

#define MaxDIS_PER_TIMES (100)

#define X_INTERVAL 30
#define Y_INTERVAL 30

#define PER_DISTANCE 60 

#define sweep_DOWNUP 1
#define sweep_LEFTRIGHT 0

#define dir_LITTLETOBIG 1
#define dir_BIGTOLITTLE 0

#define MIN_LEFT_DOWN_X -30
#define MIN_LEFT_DOWN_Y -30
#define MAX_RIGHT_X 330
#define MAX_RIGHT_Y  330

typedef enum
{
	forward,
	backward,
	turnright,
	turnleft
} Direction;

typedef struct
{
	Direction direct;
	unsigned int step;
} MoveCmd;

typedef struct
{
	float Angle;
	float x;
	float y;
} Position;

typedef struct TraceNode
{
	char ID;
	float x1;
	float y1;
	float x2;
	float y2;
	unsigned int UpdateTime;
	struct TraceNode *next;
} TraceNode, *TraceList;

typedef union
{
	Position pos;
	int leftOrRight;
	int Result;
}Para;


typedef enum
{
	seenTarget,					//����Ŀ��
	looseTarget,				// ��ʧĿ��
	patrol,							//Ѳ��
	goToOtherTar,				//Ѱ�Ҳ�����Ұ�е�Ŀ��
}Steps;


typedef struct 
{
	float Angle;
	float x;
	float y;
	unsigned int time;
	int update;
} TargetATPos;

typedef struct 
{
	Steps  which_step;
	Para   pvParameter;
}Procedure;


#ifdef DEBUG
	#define Debug_printf SystemOut
#else
	#define Debug_printf none_printf
#endif

///////////////////////////////////////////////////
extern Position CurPos;
//extern Position TargetPos;
extern Direction GlobleCmdDir;

extern TraceList ListHead;

extern Position GoThroght[60];
extern Procedure GloblePathProcedure;
extern int sweep,dir;

double computeAngle(double distance, double deltaX);
int generate_Pos(int leftDown_x,int leftDown_y,int right_Up_x,int right_Up_y);
Position searchTarg(int);
int divideDis(Position *destPos,Position TargetPos);
void setActiveArea(int left_Down_x,int left_Down_y,int right_Up_x,int right_Up_y);
Position OneNextPos(int sweep,int dir,Position PrePos);
Position getNextPos(Position tmpPos);
int ifCollision(float x1, float y1, float x2, float y2);
int IsOutSide(Position pos);
void none_printf(char *fmt, ...);

#endif
