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
	BusinessConfig m_config;	//����
	std::shared_ptr<spdlog::logger> m_log;
	std::shared_ptr<ChessControlCenter> m_chessControl;
	std::shared_ptr<Theron::Framework> m_framework;
	int m_chessBoard[ChessBoard_Y][ChessBoard_X];	//����
	ChessMoveMsg m_chessMoveMsg;	//���ڱ����������Ϣ

private:
	bool initial();	//��������
	void drawChessBoard();	//������
	void drawChess();	//������

	void drawPromptInfo(GameStatus status);	//����ʾ��Ϣ

	int roundNum(double val);	//������������
	//ͨ��λ�û�ȡ�����±�
	void getChessIndex(int x, int y);	//x��y Ϊ���꣬	chessX��chessYΪ��������
	static void monitorMouse(ChessBusiness* p);	//��������
	static void updateChess(ChessBusiness *p);	//���½���

	//for debug
private:
	void printVal();
};


#endif // !ChessBusiness_H
