#include "ChessControlCenter.h"
#include <iostream>
#include <cmath>

ChessControlCenter::ChessControlCenter(std::shared_ptr<Theron::Framework> framework, std::shared_ptr<spdlog::logger> log)
	:Theron::Actor(*framework.get(), "ChessControlCenter actor")
{	
	m_log = log;	
	
	initial();
	m_status = GameStatus::GameStatus_Red;	//默认创建后直接进行对局，红方先走
	RegisterHandler(this, &ChessControlCenter::moveChess);
}
ChessControlCenter::~ChessControlCenter() {
	
	if (m_log) m_log->flush();
};

void ChessControlCenter::getChessBoardStatus(int chessBoard[][ChessBoard_X], GameStatus &curStatus)
{
	memcpy(chessBoard, m_chessBoard, sizeof(m_chessBoard));
	curStatus = m_status;
}

bool ChessControlCenter::isExistChess(int x, int y)
{
	if (y < 0 || y >= ChessBoard_Y) return false;
	if (x < 0 || x >= ChessBoard_X) return false;
	
	if (m_chessBoard[y][x] != ChessType::ChessType_None)	return true;
	return false;
}

void ChessControlCenter::initial() {
	for (int i = 0; i < ChessBoard_Y; i++) {
		for (int j = 0; j < ChessBoard_X; j++) {
			m_chessBoard[i][j] = ChessType::ChessType_None;
		}

		switch (i) {
		case 0:	//蓝	//车~将
		{
			int iLeftVal = ChessType::ChessType_BlueCar;
			m_chessBoard[i][4] = ChessType::ChessType_BlueKing;
			for (int j = 0; j < ChessBoard_X / 2; j++) {
				m_chessBoard[i][j] = iLeftVal;
				m_chessBoard[i][ChessBoard_X - 1 - j] = iLeftVal;
				iLeftVal++;
			}
			break;
		}
		case 9:	//红	//车~将
		{
			int iLeftVal = ChessType::ChessType_RedCar;
			m_chessBoard[i][4] = ChessType::ChessType_RedKing;
			for (int j = 0; j < ChessBoard_X / 2; j++) {
				m_chessBoard[i][j] = iLeftVal;
				m_chessBoard[i][ChessBoard_X - 1 - j] = iLeftVal;
				iLeftVal--;
			}
			break;
		}
		case 2:	//蓝炮
			m_chessBoard[i][1] = ChessType::ChessType_BlueCamion;
			m_chessBoard[i][7] = ChessType::ChessType_BlueCamion;
			break;
		case 7:	//红炮
			m_chessBoard[i][1] = ChessType::ChessType_RedCamion;
			m_chessBoard[i][7] = ChessType::ChessType_RedCamion;
			break;
		case 3:	//蓝兵
			for (int j = 0; j < ChessBoard_X; j = j + 2) {
				m_chessBoard[i][j] = ChessType::ChessType_BluePawn;
			}
			break;
		case 6:	//红兵
			for (int j = 0; j < ChessBoard_X; j = j + 2) {
				m_chessBoard[i][j] = ChessType::ChessType_RedPawn;
			}
			break;
		default:
			break;
		}
	}
}


void ChessControlCenter::moveChess(const ChessMoveMsg& msg, Theron::Address from)
{
	if (m_status == GameStatus::GameStatus_None)	return;

	//检测起点、落点是否在棋盘
	if (msg.m_y1 < 0 || msg.m_y1 >= ChessBoard_Y) return;
	if (msg.m_y2 < 0 || msg.m_y2 >= ChessBoard_Y) return;
	if (msg.m_x1 < 0 || msg.m_x1 >= ChessBoard_X) return;
	if (msg.m_x2 < 0 || msg.m_x2 >= ChessBoard_X) return;
	if (msg.m_x2  == msg.m_x1 && msg.m_y1 == msg.m_y2) return;

	int curVal = m_chessBoard[msg.m_y1][msg.m_x1];
	int nextPosVal = m_chessBoard[msg.m_y2][msg.m_x2];

	if (curVal * nextPosVal > 0)	return;	//同色棋子
	switch (m_status) {	//屏蔽非法移动
	case GameStatus::GameStatus_Blue:
		if (curVal >= ChessType::ChessType_None) {
			if (m_log) m_log->warn("此时应该【蓝方】落子");
			return;
		}
		break;
	case GameStatus::GameStatus_Red:
		if (curVal <= ChessType::ChessType_None) {
			if (m_log) m_log->warn("此时应该【红方】落子");
			return;
		}
		break;
	default:
		break;
	}

	switch (curVal) {
	case ChessType::ChessType_BlueCar:	//车	//直接移动
	case ChessType::ChessType_RedCar:
		moveCar((ChessType)curVal, msg);
		break;
	case ChessType_BlueHorse:
	case ChessType_RedHorse:	//马	
		moveHorse((ChessType)curVal, msg);
		break;
	case ChessType_BluePrimeMinister:	//相
	case ChessType_RedPrimeMinister:
		movePrimeMinister((ChessType)curVal, msg);
		break;
	case ChessType_BlueGuards:	//侍
	case ChessType_RedGuards:
		moveGuards((ChessType)curVal, msg);
		break;
	case ChessType_BlueKing:	//帅
	case ChessType_RedKing:
		moveKing((ChessType)curVal, msg);
		break;
	case ChessType_BlueCamion:	//炮
	case ChessType_RedCamion:
		moveCamion((ChessType)curVal, msg);
		break;
	case ChessType_BluePawn:
	case ChessType_RedPawn:	//兵
		movePawn((ChessType)curVal, msg);
		break;
	default:
		break;
	}
}


void ChessControlCenter::directMove(ChessType curChess, const ChessMoveMsg& pos)	//直接移动	//车
{
	if (m_chessBoard[pos.m_y2][pos.m_x2] == ChessType::ChessType_BlueKing || m_chessBoard[pos.m_y2][pos.m_x2] == ChessType::ChessType_RedKing) {
		//if(curChess < ChessType::ChessType_None)	//蓝方
		//	m_status = m_status = GameStatus::GameStatus_BlueWin;
		//else
		//	m_status = m_status = GameStatus::GameStatus_RedWin;

		initial();
		m_status = GameStatus::GameStatus_Red;
		return;
	}
	m_chessBoard[pos.m_y1][pos.m_x1] = ChessType::ChessType_None;
	m_chessBoard[pos.m_y2][pos.m_x2] = curChess;

	switch (m_status) {	//移动成功后切换红蓝方下子
	case GameStatus::GameStatus_Blue:
		m_status = GameStatus::GameStatus_Red;
		break;
	case GameStatus::GameStatus_Red:
		m_status = GameStatus::GameStatus_Blue;
		break;
	default:
		break;
	}
}

void ChessControlCenter::moveCar(ChessType curChess, const ChessMoveMsg& pos)	//移动车
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X != 0 && iLength_Y != 0) {	//直线移动
		if (m_log) m_log->warn("【驹】/【车】只能直线移动");
		return;
	}

	directMove(curChess, pos);
}

void ChessControlCenter::moveHorse(ChessType curChess, const ChessMoveMsg& pos)	//移动马
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 5)	return;	//日字格
	
	int tmpX = pos.m_x1;	//用于检测是否存在蹩脚马
	int tmpY = pos.m_y1;

	if (abs(iLength_X) == 2) {
		if (iLength_X > 0) tmpX++;
		else tmpX--;
	}
	else {
		if (iLength_Y > 0) tmpY++;
		else tmpY--;
	}

	//是否存在蹩脚马
	if (m_chessBoard[tmpY][tmpX] != ChessType::ChessType_None) {
		if (m_log) m_log->warn("当前马蹩脚，无法移动");
		return;
	}

	directMove(curChess, pos);
}
void ChessControlCenter::movePrimeMinister(ChessType curChess, const ChessMoveMsg& pos)	//移动相
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 8) {
		if (m_log) m_log->warn("【相】只能走田自格");
		return;	//田字格
	}

	//是否超过楚河汉界
	if (curChess < ChessType::ChessType_None) {//蓝方
		if (pos.m_y2 > 4) {//越线
			if (m_log) m_log->warn("【蓝方】:【象】无法移动出【楚河汉界】");
			return;
		}	
	}	
	else {
		if (pos.m_y2 < 5) {//越线
			if (m_log) m_log->warn("【红方】:【相】无法移动出【楚河汉界】");
			return;
		}
	}

	directMove(curChess, pos);
}
void ChessControlCenter::moveGuards(ChessType curChess, const ChessMoveMsg& pos)	//移动侍卫
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 2)	return;	//口字格

	//是否超过王的四边	//3~5
	if (pos.m_x2 < 3 || pos.m_x2 > 5) {
		if (curChess < ChessType::ChessType_None) {
			if (m_log) m_log->warn("【蓝方】:【仕】无法离开【将】左右");
		}		
		else {
			if (m_log) m_log->warn("【红方】:【侍】无法离开【将】身边 左右");
		}				
		return;
	}
	if (curChess < ChessType::ChessType_None) {//蓝方
		if (pos.m_y2 > 2) {
			if (m_log) m_log->warn("【蓝方】:【仕】无法离开【将】前后");
			return;
		}
	}
	else {
		if (pos.m_y2  <  7) {
			if (m_log) m_log->warn("【红方】:【侍】无法离开【将】前后");
			return;
		}
	}

	directMove(curChess, pos);
}
void ChessControlCenter::moveKing(ChessType curChess, const ChessMoveMsg& pos)	//移动将
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;

	if (m_chessBoard[pos.m_y2][pos.m_x2] + m_chessBoard[pos.m_y1][pos.m_x1] == 0) {	//王对王	
		if (curChess < ChessType::ChessType_None) {//蓝方		
			if (m_log) m_log->warn("【蓝方】获得胜利");			
		}
		else {
			if (m_log) m_log->warn("【红方】获得胜利");			
		}

		directMove(curChess, pos);
		return;
	}
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 1) {	//步进
		if (curChess < ChessType::ChessType_None) {//蓝方		
			if (m_log) m_log->warn("【蓝方】：【将】只能步进");
		}
		else {
			if (m_log) m_log->warn("【红方】：【帅】只能步进");
		}
		return;
	}
	
	//不得超过田字格
	bool bMove = true;
	if (pos.m_x2 < 3 || pos.m_x2 > 5) return;
	if (curChess < ChessType::ChessType_None) {//蓝方
		if (pos.m_y2 > 2) bMove = false;
	}
	else {
		if (pos.m_y2 < 7) bMove = false;
	}
	
	if (bMove == false) {
		if (curChess < ChessType::ChessType_None) {//蓝方
			if (m_log) m_log->warn("【蓝方】：【将】只能在军营走动");
		}
		else {
			if (m_log) m_log->warn("【红方】：【帅】只能在营地走动");
		}
		return;
	}
	
	directMove(curChess, pos);	
}
void ChessControlCenter::moveCamion(ChessType curChess, const ChessMoveMsg& pos)	//移动炮
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;

	if (iLength_X != 0 && iLength_Y != 0) {//只能单线移动
		if(m_log) m_log->warn("【炮】只能单线移动");
		return;
	}
	
	bool bHorizontalMove = false;	//水平移动	//沿X轴移动
	if (iLength_X != 0) bHorizontalMove = true;

	//检测中间是否存在棋子
	bool bExistChess = false;
	if (bHorizontalMove) {
		for (int i = pos.m_x1; i != pos.m_x2; ) {	//无法判断是左移还是右移
			if (i != pos.m_x1){	// 
				if (m_chessBoard[pos.m_y1][i] != ChessType::ChessType_None) {
					bExistChess = true;
					break;
				}
			}
			
			i += (pos.m_x2 > pos.m_x1 ? 1 : -1);
		}
	}
	else {
		for (int i = pos.m_y1; i != pos.m_y2; ) {	//无法判断是左移还是右移
			if (i != pos.m_y1) {
				if (m_chessBoard[i][pos.m_x1] != ChessType::ChessType_None) {
					bExistChess = true;
					break;
				}
			}
			
			i += (pos.m_y2 > pos.m_y1 ? 1 : -1);
		}
	}

	if (!bExistChess) {
		if (m_chessBoard[pos.m_y2][pos.m_x2] != ChessType::ChessType_None)	return;
	}	
	
	directMove(curChess, pos);
}
void ChessControlCenter::movePawn(ChessType curChess, const ChessMoveMsg& pos)	//移动兵
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 1)	return;	//步进

	//是否后退
	if (curChess < ChessType::ChessType_None) {//蓝方
		if (iLength_Y < 0) {
			if (m_log) m_log->warn("蓝方：【卒】有进无退");
			return;
		}
		else if (pos.m_y1 <= 4 && iLength_X != 0) {
			if (m_log) m_log->warn("蓝方：【卒】需要经过【楚河汉界】才能左右移动");
			return;
		}
	}
	else {
		if (iLength_Y > 0) {
			if (m_log) m_log->warn("红方：【兵】有进无退");
			return;
		}
		else if (pos.m_y1 >= 5 && iLength_X != 0) {
			if (m_log) m_log->warn("红方：【兵】需要经过【楚河汉界】才能左右移动");
			return;
		}
	}

	directMove(curChess, pos);
}

