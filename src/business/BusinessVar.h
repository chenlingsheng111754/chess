#ifndef BusinessVar_H
#define BusinessVar_H

#define ChessBoard_X 9
#define ChessBoard_Y 10
#define ChessWeightUnit 8	//棋盘x坐标上有8个方形

class BusinessConfig {
public:
	int m_height;	//窗口高度像素
	int m_weight;	//宽度像素
	int m_unit;	//每个正方形的单位长度
	double m_chessRatio;	//棋子比例	//相对于单位长度
	int m_radius;		//棋子半径
	int m_innerInterval;	//间隔	//每个正方形的边框距离
	int m_leftMargin;	//左外边距	//棋盘与窗口的边距
	int m_rightMargin;	//右外边距	//棋盘与窗口的边距
	int m_upMargin;		//上外边距	//棋盘与窗口的边距
	int m_downMargin;		//下外边距	//棋盘与窗口的边距	
};

enum ChessType {
	ChessType_BlueCar = -7,		//蓝车
	ChessType_BlueHorse,		//蓝马
	ChessType_BluePrimeMinister,//蓝丞相
	ChessType_BlueGuards,		//蓝侍卫
	ChessType_BlueKing,			//蓝将
	ChessType_BlueCamion,		//蓝炮车
	ChessType_BluePawn,			//蓝卒
	ChessType_None,				//无
	ChessType_RedPawn,			//红卒
	ChessType_RedCamion,		//红炮车
	ChessType_RedKing,			//红帅
	ChessType_RedGuards,		//红侍卫
	ChessType_RedPrimeMinister, //红丞相
	ChessType_RedHorse,			//红马
	ChessType_RedCar			//红车
};

enum GameStatus {	
	GameStatus_Blue = -1,	//蓝方下子	//对应棋子的正负值
	GameStatus_None,	//未开始
	GameStatus_Red,		//红方下子
	GameStatus_BlueWin,		//蓝方获胜
	GameStatus_RedWin,		//红方获胜
};

#endif // !BusinessVar_H
