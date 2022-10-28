#ifndef ChessControlCenter_H
#define ChessControlCenter_H

#include "Theron/Actor.h"
#include "spdlog/logger.h"
#include "BusinessVar.h"
#include "ChessControlMsg.h"

class ChessControlCenter : public Theron::Actor
{
public:
	ChessControlCenter(std::shared_ptr<Theron::Framework> framework, std::shared_ptr<spdlog::logger> log);
	~ChessControlCenter();

	void getChessBoardStatus(int chessBoard[][ChessBoard_X], GameStatus &curStatus);
	bool isExistChess(int x, int y);

private:
	void initial();	//初始化棋盘以及棋子参数

	
private:
	//移动棋子
	void moveChess(const ChessMoveMsg& msg, Theron::Address from);

	//curChess 起到标注红蓝方的作业
	void directMove(ChessType curChess, const ChessMoveMsg& pos);			//直接移动	
	void moveCar(ChessType curChess, const ChessMoveMsg& pos);			//移动车
	void moveHorse(ChessType curChess, const ChessMoveMsg& pos);			//移动马
	void movePrimeMinister(ChessType curChess, const ChessMoveMsg& pos);	//移动相
	void moveGuards(ChessType curChess, const ChessMoveMsg& pos);			//移动侍卫
	void moveKing(ChessType curChess, const ChessMoveMsg& pos);				//移动将
	void moveCamion(ChessType curChess, const ChessMoveMsg& pos);			//移动炮
	void movePawn(ChessType curChess, const ChessMoveMsg& pos);				//移动兵

private:
	

	std::shared_ptr<spdlog::logger> m_log;

	/*
	* 共7种棋子
	* 正为红方，负为蓝方
	* 对应 ChessType 枚举
	* 车：7
	* 马：6
	* 相：5
	* 侍：4
	* 帅：3
	* 炮：2
	* 兵：1
	* 无：0
	*/
	//固定上方为蓝方，下方为红方	//后续可增加颠倒条件
	int m_chessBoard[ChessBoard_Y][ChessBoard_X];	//棋盘

	GameStatus m_status;
};


#endif //ChessControlCenter_H