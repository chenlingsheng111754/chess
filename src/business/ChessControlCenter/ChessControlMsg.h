#ifndef ChessControlMsg_H
#define ChessControlMsg_H

#include "Theron/Actor.h"



class ChessMoveMsg {	//��ʾ���Ӵ�һ��λ���ƶ�����һ��λ��	//�˴�Ϊ��ά���������±�
public:
	int m_x1;
	int	m_y1;
	int	m_x2;
	int m_y2;
	ChessMoveMsg();
};

THERON_DECLARE_REGISTERED_MESSAGE(ChessMoveMsg);


#endif //ChessControlMsg_H