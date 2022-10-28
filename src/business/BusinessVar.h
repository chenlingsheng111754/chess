#ifndef BusinessVar_H
#define BusinessVar_H

#define ChessBoard_X 9
#define ChessBoard_Y 10
#define ChessWeightUnit 8	//����x��������8������

class BusinessConfig {
public:
	int m_height;	//���ڸ߶�����
	int m_weight;	//�������
	int m_unit;	//ÿ�������εĵ�λ����
	double m_chessRatio;	//���ӱ���	//����ڵ�λ����
	int m_radius;		//���Ӱ뾶
	int m_innerInterval;	//���	//ÿ�������εı߿����
	int m_leftMargin;	//����߾�	//�����봰�ڵı߾�
	int m_rightMargin;	//����߾�	//�����봰�ڵı߾�
	int m_upMargin;		//����߾�	//�����봰�ڵı߾�
	int m_downMargin;		//����߾�	//�����봰�ڵı߾�	
};

enum ChessType {
	ChessType_BlueCar = -7,		//����
	ChessType_BlueHorse,		//����
	ChessType_BluePrimeMinister,//��ة��
	ChessType_BlueGuards,		//������
	ChessType_BlueKing,			//����
	ChessType_BlueCamion,		//���ڳ�
	ChessType_BluePawn,			//����
	ChessType_None,				//��
	ChessType_RedPawn,			//����
	ChessType_RedCamion,		//���ڳ�
	ChessType_RedKing,			//��˧
	ChessType_RedGuards,		//������
	ChessType_RedPrimeMinister, //��ة��
	ChessType_RedHorse,			//����
	ChessType_RedCar			//�쳵
};

enum GameStatus {	
	GameStatus_Blue = -1,	//��������	//��Ӧ���ӵ�����ֵ
	GameStatus_None,	//δ��ʼ
	GameStatus_Red,		//�췽����
	GameStatus_BlueWin,		//������ʤ
	GameStatus_RedWin,		//�췽��ʤ
};

#endif // !BusinessVar_H
