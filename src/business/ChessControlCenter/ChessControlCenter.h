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
	void initial();	//��ʼ�������Լ����Ӳ���

	
private:
	//�ƶ�����
	void moveChess(const ChessMoveMsg& msg, Theron::Address from);

	//curChess �𵽱�ע����������ҵ
	void directMove(ChessType curChess, const ChessMoveMsg& pos);			//ֱ���ƶ�	
	void moveCar(ChessType curChess, const ChessMoveMsg& pos);			//�ƶ���
	void moveHorse(ChessType curChess, const ChessMoveMsg& pos);			//�ƶ���
	void movePrimeMinister(ChessType curChess, const ChessMoveMsg& pos);	//�ƶ���
	void moveGuards(ChessType curChess, const ChessMoveMsg& pos);			//�ƶ�����
	void moveKing(ChessType curChess, const ChessMoveMsg& pos);				//�ƶ���
	void moveCamion(ChessType curChess, const ChessMoveMsg& pos);			//�ƶ���
	void movePawn(ChessType curChess, const ChessMoveMsg& pos);				//�ƶ���

private:
	

	std::shared_ptr<spdlog::logger> m_log;

	/*
	* ��7������
	* ��Ϊ�췽����Ϊ����
	* ��Ӧ ChessType ö��
	* ����7
	* ��6
	* �ࣺ5
	* �̣�4
	* ˧��3
	* �ڣ�2
	* ����1
	* �ޣ�0
	*/
	//�̶��Ϸ�Ϊ�������·�Ϊ�췽	//���������ӵߵ�����
	int m_chessBoard[ChessBoard_Y][ChessBoard_X];	//����

	GameStatus m_status;
};


#endif //ChessControlCenter_H