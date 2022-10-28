#include "ChessBusiness.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <thread>

#include <iostream>
#include <graphics.h>		// 引用图形库头文件
#include <conio.h>

ChessBusiness::ChessBusiness(){
	//每天凌晨6：30创建一个日志
	//m_log = spdlog::daily_logger_mt("chess", "./logs/chess.txt", 6, 30);
	//m_log = spdlog::rotating_logger_mt("chess", "./logs/chess.txt", 1048576 * 5, 3);
	//m_log = spdlog::stdout_color_mt("chess");
	//m_log->set_level(spdlog::level::debug);
	
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_level(spdlog::level::debug);
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("./logs/chess.txt", true);
	file_sink->set_level(spdlog::level::debug);

	m_log = std::shared_ptr<spdlog::logger>(new spdlog::logger("chess", { console_sink, file_sink }));
	if(m_log) m_log->set_level(spdlog::level::debug);
	
	//使用读取json方式获取初始化配置
	//高宽比为9：8
	m_config.m_innerInterval = 1;
	m_config.m_weight = 400;
	m_config.m_height = m_config.m_weight / 8 * 9;
	m_config.m_unit = m_config.m_weight / ChessWeightUnit;
	m_config.m_chessRatio = 3.0 / 8.0;
	m_config.m_radius = m_config.m_unit * m_config.m_chessRatio - m_config.m_innerInterval;

	m_config.m_leftMargin = m_config.m_weight / 5;
	m_config.m_rightMargin = m_config.m_weight / 5;
	m_config.m_upMargin = m_config.m_height / 5;
	m_config.m_downMargin = m_config.m_height / 10;


	int iThreadCount = std::thread::hardware_concurrency();
	if (m_log) m_log->info("current device hardware concurrency threads:{}", iThreadCount);
	m_framework = std::make_shared<Theron::Framework>(Theron::Framework::Parameters(iThreadCount));
	m_chessControl = std::make_shared<ChessControlCenter>(m_framework, m_log);

	initial();
}
ChessBusiness::~ChessBusiness() {
	closegraph(); // 关闭绘图窗口
	if (m_log) m_log->flush();
};

bool ChessBusiness::initial()	//创建棋盘
{
	initgraph(m_config.m_weight + m_config.m_leftMargin + m_config.m_rightMargin, m_config.m_height + m_config.m_upMargin + m_config.m_downMargin, EX_SHOWCONSOLE);	// 创建绘图窗口
	//initgraph(m_config.m_weight + m_config.m_leftMargin + m_config.m_rightMargin, m_config.m_height + m_config.m_upMargin + m_config.m_downMargin);	// 创建绘图窗口
	GameStatus status;
	if (m_chessControl) m_chessControl->getChessBoardStatus(m_chessBoard, status);
	
	drawChess();

	if (m_log) m_log->info("ChessBusiness::initial: chess board initial success");

	//显示状态

	std::thread(monitorMouse, this).detach();
	 //monitorMouse(this);
	std::thread(updateChess, this).detach();
	return true;
}

void ChessBusiness::drawChessBoard()//画棋盘
{
	int iInterval = m_config.m_innerInterval;
	for (int i = 0; i < ChessWeightUnit + 1; i++) {	//画宽高9个图形
		if (i == ChessWeightUnit / 2) {	//楚河汉界
			//setlinecolor(BLUE);
			int x1 = 0 + iInterval + m_config.m_leftMargin;
			int y1 = i * m_config.m_unit + iInterval + m_config.m_upMargin;
			int x2 = m_config.m_weight - iInterval + m_config.m_leftMargin;
			int y2 = (i + 1) * m_config.m_unit - iInterval + m_config.m_upMargin;
			rectangle(x1, y1, x2, y2);

			RECT rect;
			rect.left = x1;
			rect.top = y1;
			rect.right = x2;
			rect.bottom = y2;
			settextcolor(WHITE);
			//settextstyle(y2-y1, x2-x1, "黑体");
			drawtext("楚河    汉界", &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);	//文字处于水平垂直居中
			continue;
		}
		for (int j = 0; j < ChessWeightUnit; j++) {
			//setlinecolor(GREEN);
			int x1 = j * m_config.m_unit + iInterval + m_config.m_leftMargin;
			int y1 = i * m_config.m_unit + iInterval + m_config.m_upMargin;
			int x2 = (j + 1) * m_config.m_unit - iInterval + m_config.m_leftMargin;
			int y2 = (i + 1) * m_config.m_unit - iInterval + m_config.m_upMargin;
			rectangle(x1, y1, x2, y2);
			//fillroundrect(j * iUnit, i * iUnit, (j + 1) * iUnit, (i + 1) * iUnit, iUnit, iUnit);
		}
	}
}

void ChessBusiness::drawChess()	//画棋子
{
	//BeginBatchDraw(); FlushBatchDraw(); EndBatchDraw();	//批量绘图，解决画面闪烁问题	
	BeginBatchDraw();
	clearcliprgn();
	drawChessBoard();
	setlinecolor(WHITE);
	for (int y = 0; y < ChessBoard_Y; y++) {
		for (int x = 0; x < ChessBoard_X; x++) {
			if (m_chessBoard[y][x] == ChessType::ChessType_None) continue;

			if (m_chessBoard[y][x] > ChessType::ChessType_None) { //红方
				//setlinecolor(RED);
				settextcolor(LIGHTRED);
			}
			else {
				//setlinecolor(BLUE);
				settextcolor(LIGHTBLUE);
			}

			int x1 = x * m_config.m_unit + m_config.m_leftMargin;
			int y1 = y * m_config.m_unit + m_config.m_upMargin;
			

			circle(x1, y1, m_config.m_radius);

			LPCTSTR str = "";
			switch (m_chessBoard[y][x]) {
			case ChessType::ChessType_BlueCar:
				str = "驹";
				break;
			case ChessType::ChessType_RedCar:
				str = "车";
				break;
			case ChessType_BlueHorse:
			case ChessType_RedHorse:
				str = "马";
				break;
			case ChessType_BluePrimeMinister:
				str = "象";
				break;
			case ChessType_RedPrimeMinister:
				str = "相";
				break;
			case ChessType_BlueGuards:
				str = "士";
				break;
			case ChessType_RedGuards:
				str = "侍";
				break;
			case ChessType_BlueKing:
				str = "将";
				break;
			case ChessType_RedKing:
				str = "帅";
				break;
			case ChessType_BlueCamion:
			case ChessType_RedCamion:
				str = "炮";
				break;
			case ChessType_BluePawn:
				str = "卒";
				break;
			case ChessType_RedPawn:
				str = "兵";
				break;
			default:
				break;
			}
			RECT rect;
			double dLength = m_config.m_radius * sqrt(2);	//圆形中正方形的边长
			rect.left = x1 - dLength / 2;
			rect.top = y1 - dLength / 2;
			rect.right = x1 + dLength / 2;
			rect.bottom = y1 + dLength / 2;

			settextstyle(dLength / 2, dLength / 2, "黑体");
			drawtext(str, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);	//文字处于水平垂直居中
			//outtextxy(x1,y1, str);
			//在该圆中输出类型
		}
	}
	//EndBatchDraw();
	FlushBatchDraw();	

	//printVal();
}

void ChessBusiness::drawPromptInfo(GameStatus status)	//画提示信息
{
	int x1 = m_config.m_leftMargin + m_config.m_innerInterval;
	int y1 = m_config.m_innerInterval;
	int x2 = x1 + m_config.m_weight - m_config.m_innerInterval;
	int y2 = m_config.m_upMargin - m_config.m_innerInterval;
	setlinecolor(YELLOW);
	//rectangle(x1, y1, x2, y2);
	fillrectangle(x1, y1, x2, y2);
}

int ChessBusiness::roundNum(double val)	//四舍五入数字
{
	//floor() 不大于变量的最大整数
	//ceil() 不小于变量的最大整数
	return (val > 0.0 ? floor(val + 0.5) : ceil(val - 0.5));
}

void ChessBusiness::getChessIndex(int x, int y)
{
	if (!m_chessControl) return;
	if (x < m_config.m_leftMargin - m_config.m_radius || x > m_config.m_weight + m_config.m_leftMargin + m_config.m_radius) return;
	if (y < m_config.m_upMargin - m_config.m_radius || y > m_config.m_height + m_config.m_upMargin + m_config.m_radius) return;

	int posX = roundNum((double)(x - m_config.m_leftMargin) / m_config.m_unit);
	int posY = roundNum((double)(y - m_config.m_upMargin) / m_config.m_unit);
	
	if (m_chessMoveMsg.m_x1 <  0) {
		if (!m_chessControl->isExistChess(posX, posY)) return;
	}		
	
	if (m_log) m_log->info("real mouse index x:{}, y:{}", posX, posY);
	if (m_chessMoveMsg.m_x1 < 0) {
		m_chessMoveMsg.m_x1 = posX;
		m_chessMoveMsg.m_y1 = posY;
	}
	else {
		m_chessMoveMsg.m_x2 = posX;
		m_chessMoveMsg.m_y2 = posY;
		if(m_chessControl)	m_framework->Send(m_chessMoveMsg, Theron::Address::Null(), m_chessControl->GetAddress());
		m_chessMoveMsg = ChessMoveMsg();
	}
}

void ChessBusiness::monitorMouse(ChessBusiness* p)
{
	while (p){		
		if (MouseHit()) {
			MOUSEMSG msg = GetMouseMsg();
			switch (msg.uMsg){
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				if (p) p->getChessIndex(msg.x, msg.y);
				break;
			default:break;
			}
		}		
		//Sleep(500);	//100ms	//不能加延时
	}
}

void ChessBusiness::updateChess(ChessBusiness* p)
{
	int curChessBoard[ChessBoard_Y][ChessBoard_X];	//棋盘
	memset(curChessBoard, 0, sizeof(curChessBoard));
	while (p) {
		GameStatus status = GameStatus::GameStatus_None;
		if (p->m_chessControl) p->m_chessControl->getChessBoardStatus(curChessBoard, status);
		//显示输赢等状态信息
		if (p) p->drawPromptInfo(status);
		if (memcmp(curChessBoard, p->m_chessBoard, sizeof(curChessBoard)) != 0){
			memcpy(p->m_chessBoard, curChessBoard, sizeof(curChessBoard));
			p->drawChess();
		}
		Sleep(500);	//100ms
	}
	
	
}


//for debug
void ChessBusiness::printVal()
{
	if (m_log) m_log->info("=============data print start=================");
	for (int y = 0; y < ChessBoard_Y; y++) {
		for (int x = 0; x < ChessBoard_X; x++) {
			printf("%2d", m_chessBoard[y][x]);
		}
		printf("\n");
	}
	if (m_log) m_log->info("=============data print end =================");
}
