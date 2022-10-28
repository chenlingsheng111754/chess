#include "ChessControlCenter.h"
#include <iostream>
#include <cmath>

ChessControlCenter::ChessControlCenter(std::shared_ptr<Theron::Framework> framework, std::shared_ptr<spdlog::logger> log)
	:Theron::Actor(*framework.get(), "ChessControlCenter actor")
{	
	m_log = log;	
	
	initial();
	m_status = GameStatus::GameStatus_Red;	//Ĭ�ϴ�����ֱ�ӽ��жԾ֣��췽����
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
		case 0:	//��	//��~��
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
		case 9:	//��	//��~��
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
		case 2:	//����
			m_chessBoard[i][1] = ChessType::ChessType_BlueCamion;
			m_chessBoard[i][7] = ChessType::ChessType_BlueCamion;
			break;
		case 7:	//����
			m_chessBoard[i][1] = ChessType::ChessType_RedCamion;
			m_chessBoard[i][7] = ChessType::ChessType_RedCamion;
			break;
		case 3:	//����
			for (int j = 0; j < ChessBoard_X; j = j + 2) {
				m_chessBoard[i][j] = ChessType::ChessType_BluePawn;
			}
			break;
		case 6:	//���
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

	//�����㡢����Ƿ�������
	if (msg.m_y1 < 0 || msg.m_y1 >= ChessBoard_Y) return;
	if (msg.m_y2 < 0 || msg.m_y2 >= ChessBoard_Y) return;
	if (msg.m_x1 < 0 || msg.m_x1 >= ChessBoard_X) return;
	if (msg.m_x2 < 0 || msg.m_x2 >= ChessBoard_X) return;
	if (msg.m_x2  == msg.m_x1 && msg.m_y1 == msg.m_y2) return;

	int curVal = m_chessBoard[msg.m_y1][msg.m_x1];
	int nextPosVal = m_chessBoard[msg.m_y2][msg.m_x2];

	if (curVal * nextPosVal > 0)	return;	//ͬɫ����
	switch (m_status) {	//���ηǷ��ƶ�
	case GameStatus::GameStatus_Blue:
		if (curVal >= ChessType::ChessType_None) {
			if (m_log) m_log->warn("��ʱӦ�á�����������");
			return;
		}
		break;
	case GameStatus::GameStatus_Red:
		if (curVal <= ChessType::ChessType_None) {
			if (m_log) m_log->warn("��ʱӦ�á��췽������");
			return;
		}
		break;
	default:
		break;
	}

	switch (curVal) {
	case ChessType::ChessType_BlueCar:	//��	//ֱ���ƶ�
	case ChessType::ChessType_RedCar:
		moveCar((ChessType)curVal, msg);
		break;
	case ChessType_BlueHorse:
	case ChessType_RedHorse:	//��	
		moveHorse((ChessType)curVal, msg);
		break;
	case ChessType_BluePrimeMinister:	//��
	case ChessType_RedPrimeMinister:
		movePrimeMinister((ChessType)curVal, msg);
		break;
	case ChessType_BlueGuards:	//��
	case ChessType_RedGuards:
		moveGuards((ChessType)curVal, msg);
		break;
	case ChessType_BlueKing:	//˧
	case ChessType_RedKing:
		moveKing((ChessType)curVal, msg);
		break;
	case ChessType_BlueCamion:	//��
	case ChessType_RedCamion:
		moveCamion((ChessType)curVal, msg);
		break;
	case ChessType_BluePawn:
	case ChessType_RedPawn:	//��
		movePawn((ChessType)curVal, msg);
		break;
	default:
		break;
	}
}


void ChessControlCenter::directMove(ChessType curChess, const ChessMoveMsg& pos)	//ֱ���ƶ�	//��
{
	if (m_chessBoard[pos.m_y2][pos.m_x2] == ChessType::ChessType_BlueKing || m_chessBoard[pos.m_y2][pos.m_x2] == ChessType::ChessType_RedKing) {
		//if(curChess < ChessType::ChessType_None)	//����
		//	m_status = m_status = GameStatus::GameStatus_BlueWin;
		//else
		//	m_status = m_status = GameStatus::GameStatus_RedWin;

		initial();
		m_status = GameStatus::GameStatus_Red;
		return;
	}
	m_chessBoard[pos.m_y1][pos.m_x1] = ChessType::ChessType_None;
	m_chessBoard[pos.m_y2][pos.m_x2] = curChess;

	switch (m_status) {	//�ƶ��ɹ����л�����������
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

void ChessControlCenter::moveCar(ChessType curChess, const ChessMoveMsg& pos)	//�ƶ���
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X != 0 && iLength_Y != 0) {	//ֱ���ƶ�
		if (m_log) m_log->warn("���ԡ�/������ֻ��ֱ���ƶ�");
		return;
	}

	directMove(curChess, pos);
}

void ChessControlCenter::moveHorse(ChessType curChess, const ChessMoveMsg& pos)	//�ƶ���
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 5)	return;	//���ָ�
	
	int tmpX = pos.m_x1;	//���ڼ���Ƿ����������
	int tmpY = pos.m_y1;

	if (abs(iLength_X) == 2) {
		if (iLength_X > 0) tmpX++;
		else tmpX--;
	}
	else {
		if (iLength_Y > 0) tmpY++;
		else tmpY--;
	}

	//�Ƿ����������
	if (m_chessBoard[tmpY][tmpX] != ChessType::ChessType_None) {
		if (m_log) m_log->warn("��ǰ�����ţ��޷��ƶ�");
		return;
	}

	directMove(curChess, pos);
}
void ChessControlCenter::movePrimeMinister(ChessType curChess, const ChessMoveMsg& pos)	//�ƶ���
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 8) {
		if (m_log) m_log->warn("���ࡿֻ�������Ը�");
		return;	//���ָ�
	}

	//�Ƿ񳬹����Ӻ���
	if (curChess < ChessType::ChessType_None) {//����
		if (pos.m_y2 > 4) {//Խ��
			if (m_log) m_log->warn("��������:�����޷��ƶ��������Ӻ��硿");
			return;
		}	
	}	
	else {
		if (pos.m_y2 < 5) {//Խ��
			if (m_log) m_log->warn("���췽��:���ࡿ�޷��ƶ��������Ӻ��硿");
			return;
		}
	}

	directMove(curChess, pos);
}
void ChessControlCenter::moveGuards(ChessType curChess, const ChessMoveMsg& pos)	//�ƶ�����
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 2)	return;	//���ָ�

	//�Ƿ񳬹������ı�	//3~5
	if (pos.m_x2 < 3 || pos.m_x2 > 5) {
		if (curChess < ChessType::ChessType_None) {
			if (m_log) m_log->warn("��������:���ˡ��޷��뿪����������");
		}		
		else {
			if (m_log) m_log->warn("���췽��:���̡��޷��뿪��������� ����");
		}				
		return;
	}
	if (curChess < ChessType::ChessType_None) {//����
		if (pos.m_y2 > 2) {
			if (m_log) m_log->warn("��������:���ˡ��޷��뿪������ǰ��");
			return;
		}
	}
	else {
		if (pos.m_y2  <  7) {
			if (m_log) m_log->warn("���췽��:���̡��޷��뿪������ǰ��");
			return;
		}
	}

	directMove(curChess, pos);
}
void ChessControlCenter::moveKing(ChessType curChess, const ChessMoveMsg& pos)	//�ƶ���
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;

	if (m_chessBoard[pos.m_y2][pos.m_x2] + m_chessBoard[pos.m_y1][pos.m_x1] == 0) {	//������	
		if (curChess < ChessType::ChessType_None) {//����		
			if (m_log) m_log->warn("�����������ʤ��");			
		}
		else {
			if (m_log) m_log->warn("���췽�����ʤ��");			
		}

		directMove(curChess, pos);
		return;
	}
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 1) {	//����
		if (curChess < ChessType::ChessType_None) {//����		
			if (m_log) m_log->warn("����������������ֻ�ܲ���");
		}
		else {
			if (m_log) m_log->warn("���췽������˧��ֻ�ܲ���");
		}
		return;
	}
	
	//���ó������ָ�
	bool bMove = true;
	if (pos.m_x2 < 3 || pos.m_x2 > 5) return;
	if (curChess < ChessType::ChessType_None) {//����
		if (pos.m_y2 > 2) bMove = false;
	}
	else {
		if (pos.m_y2 < 7) bMove = false;
	}
	
	if (bMove == false) {
		if (curChess < ChessType::ChessType_None) {//����
			if (m_log) m_log->warn("����������������ֻ���ھ�Ӫ�߶�");
		}
		else {
			if (m_log) m_log->warn("���췽������˧��ֻ����Ӫ���߶�");
		}
		return;
	}
	
	directMove(curChess, pos);	
}
void ChessControlCenter::moveCamion(ChessType curChess, const ChessMoveMsg& pos)	//�ƶ���
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;

	if (iLength_X != 0 && iLength_Y != 0) {//ֻ�ܵ����ƶ�
		if(m_log) m_log->warn("���ڡ�ֻ�ܵ����ƶ�");
		return;
	}
	
	bool bHorizontalMove = false;	//ˮƽ�ƶ�	//��X���ƶ�
	if (iLength_X != 0) bHorizontalMove = true;

	//����м��Ƿ��������
	bool bExistChess = false;
	if (bHorizontalMove) {
		for (int i = pos.m_x1; i != pos.m_x2; ) {	//�޷��ж������ƻ�������
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
		for (int i = pos.m_y1; i != pos.m_y2; ) {	//�޷��ж������ƻ�������
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
void ChessControlCenter::movePawn(ChessType curChess, const ChessMoveMsg& pos)	//�ƶ���
{
	int iLength_X = pos.m_x2 - pos.m_x1;
	int iLength_Y = pos.m_y2 - pos.m_y1;
	if (iLength_X * iLength_X + iLength_Y * iLength_Y != 1)	return;	//����

	//�Ƿ����
	if (curChess < ChessType::ChessType_None) {//����
		if (iLength_Y < 0) {
			if (m_log) m_log->warn("���������䡿�н�����");
			return;
		}
		else if (pos.m_y1 <= 4 && iLength_X != 0) {
			if (m_log) m_log->warn("���������䡿��Ҫ���������Ӻ��硿���������ƶ�");
			return;
		}
	}
	else {
		if (iLength_Y > 0) {
			if (m_log) m_log->warn("�췽���������н�����");
			return;
		}
		else if (pos.m_y1 >= 5 && iLength_X != 0) {
			if (m_log) m_log->warn("�췽����������Ҫ���������Ӻ��硿���������ƶ�");
			return;
		}
	}

	directMove(curChess, pos);
}

