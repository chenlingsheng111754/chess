#ifndef ChessBusiness_H
#define ChessBusiness_H

#include "Theron/Actor.h"
#include "spdlog/logger.h"
#include "ChessControlCenter.h"

class ChessBusiness {
public:
	ChessBusiness();
	~ChessBusiness();
	
private:
	BusinessConfig m_config;	//后期
	std::shared_ptr<spdlog::logger> m_log;
	std::shared_ptr<ChessControlCenter> m_chessControl;
	std::shared_ptr<Theron::Framework> m_framework;
	int m_chessBoard[ChessBoard_Y][ChessBoard_X];	//棋盘
	ChessMoveMsg m_chessMoveMsg;	//用于保存鼠标点击信息

private:
	bool initial();	//创建棋盘
	void drawChessBoard();	//画棋盘
	void drawChess();	//画棋子

	void drawPromptInfo(GameStatus status);	//画提示信息

	int roundNum(double val);	//四舍五入数字
	//通过位置获取棋子下标
	void getChessIndex(int x, int y);	//x、y 为坐标，	chessX、chessY为棋盘坐标
	static void monitorMouse(ChessBusiness* p);	//检测鼠标点击
	static void updateChess(ChessBusiness *p);	//更新界面

	//for debug
private:
	void printVal();
};


#endif // !ChessBusiness_H
