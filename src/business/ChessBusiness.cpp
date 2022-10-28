#include "ChessBusiness.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <thread>

#include <iostream>
#include <graphics.h>		// ����ͼ�ο�ͷ�ļ�
#include <conio.h>

ChessBusiness::ChessBusiness(){
	//ÿ���賿6��30����һ����־
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
	
	//ʹ�ö�ȡjson��ʽ��ȡ��ʼ������
	//�߿��Ϊ9��8
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
	closegraph(); // �رջ�ͼ����
	if (m_log) m_log->flush();
};

bool ChessBusiness::initial()	//��������
{
	initgraph(m_config.m_weight + m_config.m_leftMargin + m_config.m_rightMargin, m_config.m_height + m_config.m_upMargin + m_config.m_downMargin, EX_SHOWCONSOLE);	// ������ͼ����
	//initgraph(m_config.m_weight + m_config.m_leftMargin + m_config.m_rightMargin, m_config.m_height + m_config.m_upMargin + m_config.m_downMargin);	// ������ͼ����
	GameStatus status;
	if (m_chessControl) m_chessControl->getChessBoardStatus(m_chessBoard, status);
	
	drawChess();

	if (m_log) m_log->info("ChessBusiness::initial: chess board initial success");

	//��ʾ״̬

	std::thread(monitorMouse, this).detach();
	 //monitorMouse(this);
	std::thread(updateChess, this).detach();
	return true;
}

void ChessBusiness::drawChessBoard()//������
{
	int iInterval = m_config.m_innerInterval;
	for (int i = 0; i < ChessWeightUnit + 1; i++) {	//�����9��ͼ��
		if (i == ChessWeightUnit / 2) {	//���Ӻ���
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
			//settextstyle(y2-y1, x2-x1, "����");
			drawtext("����    ����", &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);	//���ִ���ˮƽ��ֱ����
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

void ChessBusiness::drawChess()	//������
{
	//BeginBatchDraw(); FlushBatchDraw(); EndBatchDraw();	//������ͼ�����������˸����	
	BeginBatchDraw();
	clearcliprgn();
	drawChessBoard();
	setlinecolor(WHITE);
	for (int y = 0; y < ChessBoard_Y; y++) {
		for (int x = 0; x < ChessBoard_X; x++) {
			if (m_chessBoard[y][x] == ChessType::ChessType_None) continue;

			if (m_chessBoard[y][x] > ChessType::ChessType_None) { //�췽
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
				str = "��";
				break;
			case ChessType::ChessType_RedCar:
				str = "��";
				break;
			case ChessType_BlueHorse:
			case ChessType_RedHorse:
				str = "��";
				break;
			case ChessType_BluePrimeMinister:
				str = "��";
				break;
			case ChessType_RedPrimeMinister:
				str = "��";
				break;
			case ChessType_BlueGuards:
				str = "ʿ";
				break;
			case ChessType_RedGuards:
				str = "��";
				break;
			case ChessType_BlueKing:
				str = "��";
				break;
			case ChessType_RedKing:
				str = "˧";
				break;
			case ChessType_BlueCamion:
			case ChessType_RedCamion:
				str = "��";
				break;
			case ChessType_BluePawn:
				str = "��";
				break;
			case ChessType_RedPawn:
				str = "��";
				break;
			default:
				break;
			}
			RECT rect;
			double dLength = m_config.m_radius * sqrt(2);	//Բ���������εı߳�
			rect.left = x1 - dLength / 2;
			rect.top = y1 - dLength / 2;
			rect.right = x1 + dLength / 2;
			rect.bottom = y1 + dLength / 2;

			settextstyle(dLength / 2, dLength / 2, "����");
			drawtext(str, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);	//���ִ���ˮƽ��ֱ����
			//outtextxy(x1,y1, str);
			//�ڸ�Բ���������
		}
	}
	//EndBatchDraw();
	FlushBatchDraw();	

	//printVal();
}

void ChessBusiness::drawPromptInfo(GameStatus status)	//����ʾ��Ϣ
{
	int x1 = m_config.m_leftMargin + m_config.m_innerInterval;
	int y1 = m_config.m_innerInterval;
	int x2 = x1 + m_config.m_weight - m_config.m_innerInterval;
	int y2 = m_config.m_upMargin - m_config.m_innerInterval;
	setlinecolor(YELLOW);
	//rectangle(x1, y1, x2, y2);
	fillrectangle(x1, y1, x2, y2);
}

int ChessBusiness::roundNum(double val)	//������������
{
	//floor() �����ڱ������������
	//ceil() ��С�ڱ������������
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
		//Sleep(500);	//100ms	//���ܼ���ʱ
	}
}

void ChessBusiness::updateChess(ChessBusiness* p)
{
	int curChessBoard[ChessBoard_Y][ChessBoard_X];	//����
	memset(curChessBoard, 0, sizeof(curChessBoard));
	while (p) {
		GameStatus status = GameStatus::GameStatus_None;
		if (p->m_chessControl) p->m_chessControl->getChessBoardStatus(curChessBoard, status);
		//��ʾ��Ӯ��״̬��Ϣ
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
