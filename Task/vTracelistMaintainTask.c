#include "main.h"

void vTracelistMaintainTask(void *pvParameters)
{
	TraceNode tempNode, *p, *q;
	ListHead = (TraceNode*) malloc(sizeof (TraceNode));
	ListHead->next = NULL;
	ListHead->ID = 0;
	ListHead->x1 = 0;
	ListHead->y1 = 0;
	ListHead->x2 = 0;
	ListHead->y2 = 0;
	
	while (1)
	{
		if (pdTRUE == xQueueReceive(TraceNodeQueue, &tempNode, 5000))
		{
			p = ListHead->next;
			q = ListHead;
			while (p)
			{
				if (tempNode.ID == p->ID)
				{
					p->x1 = tempNode.x1;
					p->y1 = tempNode.y1;
					p->x2 = tempNode.x2;
					p->y2 = tempNode.y2;
					break;
				}
				else
				{
					p = p->next;
					q = q->next;
				}
			}
			if (NULL == p)
			{
				p = q;
				q = (TraceNode*) malloc(sizeof (TraceNode));
				*q = tempNode;
				q->next = NULL;
				p->next = q;
			}
		}
		
		p = ListHead->next;
		q = ListHead;
		while (p)
		{
			if (Timer2Counter - p->UpdateTime > 10000)
			{
				q->next = p->next;
				free(p);
				p = q->next;
			}
			else
			{
				p = p->next;
				q = q->next;
			}
		}
	}
}
