#ifndef ChessControlMsg_H
#define ChessControlMsg_H

#include "Theron/Actor.h"



class ChessMoveMsg {	//表示棋子从一个位置移动到另一个位置	//此处为二维数组棋盘下标
public:
	int m_x1;
	int	m_y1;
	int	m_x2;
	int m_y2;
	ChessMoveMsg();
};

THERON_DECLARE_REGISTERED_MESSAGE(ChessMoveMsg);


#endif //ChessControlMsg_H