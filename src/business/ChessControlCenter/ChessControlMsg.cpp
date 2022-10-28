#include "ChessControlMsg.h"

ChessMoveMsg::ChessMoveMsg(){
	m_x1 = -1;
	m_y1 = -1;
	m_x2 = -1;
	m_y2 = -1;
}
THERON_DEFINE_REGISTERED_MESSAGE(ChessMoveMsg);