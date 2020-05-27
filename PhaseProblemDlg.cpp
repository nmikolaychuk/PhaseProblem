
// PhaseProblemDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "PhaseProblem.h"
#include "PhaseProblemDlg.h"
#include "afxdialogex.h"

#include <math.h>
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DOTS(x,y) (xp*((x)-xmin)),(yp*((y)-ymax))							// макрос перевода координат. график сигнала
#define DOTSSPEC(x,y) (xpspec*((x)-xminspec)),(ypspec*((y)-ymaxspec))							// макрос перевода координат. график сигнала

// Диалоговое окно CPhaseProblemDlg

void CPhaseProblemDlg::fourea(struct cmplx* data, int n, int is)
{
	int i, j, istep;
	int m, mmax;
	float r, r1, theta, w_r, w_i, temp_r, temp_i;
	float pi = 3.1415926f;

	r = pi * is;
	j = 0;
	for (i = 0; i < n; i++)
	{
		if (i < j)
		{
			temp_r = data[j].real;
			temp_i = data[j].image;
			data[j].real = data[i].real;
			data[j].image = data[i].image;
			data[i].real = temp_r;
			data[i].image = temp_i;
		}
		m = n >> 1;
		while (j >= m) { j -= m; m = (m + 1) / 2; }
		j += m;
	}
	mmax = 1;
	while (mmax < n)
	{
		istep = mmax << 1;
		r1 = r / (float)mmax;
		for (m = 0; m < mmax; m++)
		{
			theta = r1 * m;
			w_r = (float)cos((double)theta);
			w_i = (float)sin((double)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r * data[j].real - w_i * data[j].image;
				temp_i = w_r * data[j].image + w_i * data[j].real;
				data[j].real = data[i].real - temp_r;
				data[j].image = data[i].image - temp_i;
				data[i].real += temp_r;
				data[i].image += temp_i;
			}
		}
		mmax = istep;
	}
	if (is > 0)
		for (i = 0; i < n; i++)
		{
			data[i].real /= (float)n;
			data[i].image /= (float)n;
		}
}

CPhaseProblemDlg::CPhaseProblemDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PHASEPROBLEM_DIALOG, pParent)
	, Length(1024)
	, e_ampl1(4.0)
	, e_ampl2(2.0)
	, e_ampl3(3.3)
	, e_disp1(2.0)
	, e_disp2(1.0)
	, e_disp3(2.0)
	, e_center_pos1(130.0)
	, e_center_pos2(380.0)
	, e_center_pos3(600.0)
	, e_ampl4(2.5)
	, e_ampl5(3.4)
	, e_disp4(1.0)
	, e_disp5(1.5)
	, e_center_pos4(800.0)
	, e_center_pos5(910.0)
	, accurat(1.e-3)
	, SignalFlag(false)
	, SpectrFlag(false)
	, VosstFlag(false)
	, osh(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPhaseProblemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LENGTH, Length);
	DDX_Text(pDX, IDC_EDIT_AMPL_1, e_ampl1);
	DDX_Text(pDX, IDC_EDIT_AMPL_2, e_ampl2);
	DDX_Text(pDX, IDC_EDIT_AMPL_3, e_ampl3);
	DDX_Text(pDX, IDC_EDIT_DISP_1, e_disp1);
	DDX_Text(pDX, IDC_EDIT_DISP_2, e_disp2);
	DDX_Text(pDX, IDC_EDIT_DISP_3, e_disp3);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_1, e_center_pos1);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_2, e_center_pos2);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_3, e_center_pos3);
	DDX_Text(pDX, IDC_EDIT_AMPL_4, e_ampl4);
	DDX_Text(pDX, IDC_EDIT_AMPL_5, e_ampl5);
	DDX_Text(pDX, IDC_EDIT_DISP_4, e_disp4);
	DDX_Text(pDX, IDC_EDIT_DISP_5, e_disp5);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_4, e_center_pos4);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_5, e_center_pos5);
	DDX_Control(pDX, IDC_BUTTON_START_RECOVERY, button_StartRecovery);
	DDX_Text(pDX, IDC_EDIT_ACCURAT, accurat);
	DDX_Control(pDX, IDC_CHECK_REFLECT, check_reflect);
	DDX_Control(pDX, IDC_CHECK_SHIFT, check_shift);
	DDX_Control(pDX, IDC_STATIC_ERR, static_err);
}

BEGIN_MESSAGE_MAP(CPhaseProblemDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CPhaseProblemDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_START, &CPhaseProblemDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_START_RECOVERY, &CPhaseProblemDlg::OnBnClickedButtonStartRecovery)
	ON_BN_CLICKED(IDC_BUTTON_DROP_RECOVERY, &CPhaseProblemDlg::OnBnClickedButtonDropRecovery)
END_MESSAGE_MAP()


// Обработчики сообщений CPhaseProblemDlg

BOOL CPhaseProblemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	PicWnd = GetDlgItem(IDC_SIGNAL_PICTURE);			//связываем с ID окон
	PicDc = PicWnd->GetDC();
	PicWnd->GetClientRect(&Pic);

	PicWndSpec = GetDlgItem(IDC_SPECTR_PICTURE);			//связываем с ID окон
	PicDcSpec = PicWndSpec->GetDC();
	PicWndSpec->GetClientRect(&PicSpec);

	// перья
	setka_pen.CreatePen(		//для сетки
		PS_DOT,					//пунктирная
		-1,						//толщина 1 пиксель
		RGB(0, 0, 0));			//цвет  черный

	osi_pen.CreatePen(			//координатные оси
		PS_SOLID,				//сплошная линия
		2,						//толщина 2 пикселя
		RGB(0, 0, 0));			//цвет черный

	signal_pen.CreatePen(			//график
		PS_SOLID,				//сплошная линия
		-1,						//толщина -1 пикселя
		RGB(255, 0, 0));			//цвет синий

	spectr_pen.CreatePen(			//график
		PS_SOLID,				//сплошная линия
		-1,						//толщина -1 пикселя
		RGB(0, 0, 255));			//цвет синий		

	vosstanovl_pen.CreatePen(			//график
		PS_SOLID,				//сплошная линия
		-1,						//толщина -1 пикселя
		RGB(0, 0, 50));			//цвет синий	

	UpdateData(false);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CPhaseProblemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		if (SignalFlag == true)
		{
			Graph1(Signal, PicDc, Pic, &signal_pen, Length);
		}
		if (SpectrFlag == true)
		{
			Graph1(Spectr, PicDcSpec, PicSpec, &spectr_pen, Length);
		}
		if (VosstFlag == true)
		{
			Graph2(Signal, &signal_pen, RestoreSignal, &vosstanovl_pen, PicDc, Pic, Length);
		}
		UpdateData(false);
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CPhaseProblemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI MyProc(PVOID pv)
{
	CPhaseProblemDlg* p = (CPhaseProblemDlg*)pv;
	p->Fienup();
	return 0;
}

void CPhaseProblemDlg::Graph1(double* Mass, CDC* WinDc, CRect WinPic, CPen* graphpen, double AbsMax)
{
	// поиск максимального и минимального значения
	Min = Mass[0];
	Max = Mass[0];
	for (int i = 1; i < Length; i++)
	{
		if (Mass[i] < Min)
		{
			Min = Mass[i];
		}
		if (Mass[i] > Max)
		{
			Max = Mass[i];
		}
	}
	// отрисовка
	// создание контекста устройства
	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);
	bmp.CreateCompatibleBitmap(WinDc, WinPic.Width(), WinPic.Height());
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);
	// заливка фона графика белым цветом
	MemDc->FillSolidRect(WinPic, RGB(255, 255, 255));
	// отрисовка сетки координат
	MemDc->SelectObject(&setka_pen);
	// вертикальные линии сетки координат
	for (double i = WinPic.Width() / 15; i < WinPic.Width(); i += WinPic.Width() / 15)
	{
		MemDc->MoveTo(i, 0);
		MemDc->LineTo(i, WinPic.Height());
	}
	// горизонтальные линии сетки координат
	for (double i = WinPic.Height() / 10; i < WinPic.Height(); i += WinPic.Height() / 10)
	{
		MemDc->MoveTo(0, i);
		MemDc->LineTo(WinPic.Width(), i);
	}
	// отрисовка осей
	MemDc->SelectObject(&osi_pen);
	// отрисовка оси X
	//создаём Ось Y
	MemDc->MoveTo(0, WinPic.Height() * 9 / 10); MemDc->LineTo(WinPic.Width(), WinPic.Height() * 9 / 10);
	// отрисовка оси Y
	MemDc->MoveTo(WinPic.Width() * 1 / 15, WinPic.Height()); MemDc->LineTo(WinPic.Width() * 1 / 15, 0);
	// установка прозрачного фона текста
	MemDc->SetBkMode(TRANSPARENT);
	// установка шрифта
	CFont font;
	font.CreateFontW(14.5, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Century Gothic"));
	MemDc->SelectObject(&font);
	// подпись оси X
	MemDc->TextOut(WinPic.Width() * 14 / 15 + 4, WinPic.Height() * 9 / 10 + 2, CString("t"));
	// подпись оси Y
	MemDc->TextOut(WinPic.Width() * 1 / 15 + 10, 0, CString("A"));
	// выбор области для рисования
	xx0 = WinPic.Width() * 1 / 15; xxmax = WinPic.Width();
	yy0 = WinPic.Height() / 10; yymax = WinPic.Height() * 9 / 10;
	// отрисовка
	MemDc->SelectObject(graphpen);
	MemDc->MoveTo(xx0, yymax + (Mass[0] - Min) / (Max - Min) * (yy0 - yymax));
	for (int i = 0; i < Length; i++)
	{
		xxi = xx0 + (xxmax - xx0) * i / (Length - 1);
		yyi = yymax + (Mass[i] - Min) / (Max - Min) * (yy0 - yymax);
		MemDc->LineTo(xxi, yyi);
	}
	/* вывод числовых значений
	 по оси X*/
	MemDc->SelectObject(&font);
	for (int i = 1; i < 15; i++)
	{
		sprintf_s(znach, "%.1f", i * AbsMax / 15);
		MemDc->TextOut(i * (WinPic.Width() / 15) + AbsMax * 0.035, WinPic.Height() * 9 / 10 + 2, CString(znach));
	}
	// по оси Y
	for (int i = 1; i < 5; i++)
	{
		sprintf_s(znach, "%.2f", i * Max / 4);
		MemDc->TextOut(AbsMax * 0.015, WinPic.Height() * (8.5 - 2 * i) / 10, CString(znach));
	}
	// вывод на экран
	WinDc->BitBlt(0, 0, WinPic.Width(), WinPic.Height(), MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

// отрисовка двух графиков
void CPhaseProblemDlg::Graph2(double* Mass1, CPen* graph1pen, double* Mass2, CPen* graph2pen, CDC* WinDc, CRect WinPic, double AbsMax)
{
	// поиск максимального и минимального значения
	Min1 = Mass1[0];
	Max1 = Mass1[0];
	Min2 = Mass2[0];
	Max2 = Mass2[0];
	for (int i = 1; i < Length; i++)
	{
		if (Mass1[i] < Min1)
		{
			Min1 = Mass1[i];
		}
		if (Mass1[i] > Max1)
		{
			Max1 = Mass1[i];
		}
		if (Mass2[i] < Min2)
		{
			Min2 = Mass2[i];
		}
		if (Mass2[i] > Max2)
		{
			Max2 = Mass2[i];
		}
	}
	if (Max2 > Max1)
	{
		Max = Max2;
	}
	else
	{
		Max = Max1;
	}
	if (Min2 < Min1)
	{
		Min = Min2;
	}
	else
	{
		Min = Min1;
	}
	// отрисовка
	// создание контекста устройства
	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);
	bmp.CreateCompatibleBitmap(WinDc, WinPic.Width(), WinPic.Height());
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);
	// заливка фона графика белым цветом
	MemDc->FillSolidRect(WinPic, RGB(255, 255, 255));
	// отрисовка сетки координат
	MemDc->SelectObject(&setka_pen);
	// вертикальные линии сетки координат
	for (double i = WinPic.Width() / 15; i < WinPic.Width(); i += WinPic.Width() / 15)
	{
		MemDc->MoveTo(i, 0);
		MemDc->LineTo(i, WinPic.Height());
	}
	// горизонтальные линии сетки координат
	for (double i = WinPic.Height() / 10; i < WinPic.Height(); i += WinPic.Height() / 10)
	{
		MemDc->MoveTo(0, i);
		MemDc->LineTo(WinPic.Width(), i);
	}
	// отрисовка осей
	MemDc->SelectObject(&osi_pen);
	// отрисовка оси X
	MemDc->MoveTo(0, WinPic.Height() * 9 / 10); MemDc->LineTo(WinPic.Width(), WinPic.Height() * 9 / 10);
	// отрисовка оси Y
	MemDc->MoveTo(WinPic.Width() * 1 / 15, WinPic.Height()); MemDc->LineTo(WinPic.Width() * 1 / 15, 0);
	// установка прозрачного фона текста
	MemDc->SetBkMode(TRANSPARENT);
	// установка шрифта
	CFont font;
	font.CreateFontW(14.5, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Century Gothic"));
	MemDc->SelectObject(&font);
	// подпись оси X
	MemDc->TextOut(WinPic.Width() * 14 / 15 + 4, WinPic.Height() * 9 / 10 + 2, CString("t"));
	// подпись оси Y
	MemDc->TextOut(WinPic.Width() * 1 / 15 + 10, 0, CString("A"));
	// выбор области для рисования
	xx0 = WinPic.Width() * 1 / 15; xxmax = WinPic.Width();
	yy0 = WinPic.Height() / 10; yymax = WinPic.Height() * 9 / 10;
	// отрисовка первого графика
	MemDc->SelectObject(graph1pen);
	MemDc->MoveTo(xx0, yymax + (Mass1[0] - Min) / (Max - Min) * (yy0 - yymax));
	for (int i = 0; i < Length; i++)
	{
		xxi = xx0 + (xxmax - xx0) * i / (Length - 1);
		yyi = yymax + (Mass1[i] - Min) / (Max - Min) * (yy0 - yymax);
		MemDc->LineTo(xxi, yyi);
	}
	// отрисовка второго графика
	MemDc->SelectObject(graph2pen);
	MemDc->MoveTo(xx0, yymax + (Mass2[0] - Min) / (Max - Min) * (yy0 - yymax));
	for (int i = 0; i < Length; i++)
	{
		xxi = xx0 + (xxmax - xx0) * i / (Length - 1);
		yyi = yymax + (Mass2[i] - Min) / (Max - Min) * (yy0 - yymax);
		MemDc->LineTo(xxi, yyi);
	}
	// вывод числовых значений
	// по оси X
	MemDc->SelectObject(&font);
	for (int i = 1; i < 15; i++)
	{
		sprintf_s(znach, "%.1f", i * AbsMax / 15);
		MemDc->TextOut(i * (WinPic.Width() / 15) + AbsMax * 0.035, WinPic.Height() * 9 / 10 + 2, CString(znach));
	}
	// по оси Y
	for (int i = 1; i < 5; i++)
	{
		sprintf_s(znach, "%.2f", i * Max / 4);
		MemDc->TextOut(AbsMax * 0.015, WinPic.Height() * (8.5 - 2 * i) / 10, CString(znach));
	}
	// вывод на экран
	WinDc->BitBlt(0, 0, WinPic.Width(), WinPic.Height(), MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

double CPhaseProblemDlg::function(int t)
{
	UpdateData(TRUE);
	double result = 0.0;
	double ampl[] = { e_ampl1, e_ampl2, e_ampl3, e_ampl4, e_ampl5 };
	double disp[] = { e_disp1, e_disp2, e_disp3, e_disp4, e_disp5 };
	double t0[] = { e_center_pos1, e_center_pos2, e_center_pos3, e_center_pos4, e_center_pos5 };
	for (int i = 0; i < 5; i++)
	{
		result += ampl[i] * exp(-((t - t0[i]) / disp[i]) * ((t - t0[i]) / disp[i]));
	}
	return result;
}

double CPhaseProblemDlg::Psi()		//рандомизация для шума
{
	double r = 0;
	double a = 0.;
	double b = 2 * Pi;

	for (int i = 1; i <= 12; i++)
	{
		r += (double)rand() / (double)RAND_MAX * (b - a) + a;	// [0;2PI]
	}
	return r / 12;
}

void CPhaseProblemDlg::Reflection()
{
	double* Buffer = new double[Length];
	for (int i = 0; i < Length; i++)
	{
		Buffer[i] = RestoreSignal[i];
	}
	for (int i = 0; i < Length / 2; i++)
	{
		RestoreSignal[i] = Buffer[Length / 2 - i - 1];
	}
	for (int i = 0; i < Length / 2; i++)
	{
		RestoreSignal[Length / 2 + i] = Buffer[Length - i - 1];
	}
	delete[] Buffer;
}

void CPhaseProblemDlg::Shift()
{
	double* Buffer = new double[Length];
	double* RestoreHelp = new double[Length];
	double* NewMassiv = new double[Length];
	double* NewInvMassiv = new double[Length];

	for (int i = 0; i < Length; i++)
	{
		RestoreHelp[i] = RestoreSignal[i];
		Buffer[i] = RestoreSignal[i];
		NewMassiv[i] = 0;
		NewInvMassiv[i] = 0;
	}

	for (int i = 0; i < Length; i++)
	{
		for (int j = 0; j < Length; j++)
		{
			if ((i + j) < Length)
			{
				RestoreHelp[j] = Buffer[i + j];
			}
			else
			{
				RestoreHelp[j] = Buffer[i + j - Length];
			}
		}

		for (int k = 0; k < Length; k++)
		{
			NewMassiv[i] += sqrt((Signal[k] - RestoreHelp[k]) * (Signal[k] - RestoreHelp[k]));
		}
	}
	if (check_reflect.GetCheck() == BST_CHECKED && check_shift.GetCheck() == BST_CHECKED)
	{
		Reflection();
	}
	//Reflection();

	for (int i = 0; i < Length; i++)
	{
		RestoreHelp[i] = RestoreSignal[i];
		Buffer[i] = RestoreSignal[i];
	}

	for (int i = 0; i < Length; i++)
	{
		for (int j = 0; j < Length; j++)
		{
			if ((i + j) < Length)
			{
				RestoreHelp[j] = Buffer[i + j];
			}
			else
			{
				RestoreHelp[j] = Buffer[i + j - Length];
			}
		}

		for (int k = 0; k < Length; k++)
		{
			NewInvMassiv[i] += sqrt((Signal[k] - RestoreHelp[k]) * (Signal[k] - RestoreHelp[k]));
		}
	}
	if (check_reflect.GetCheck() == BST_CHECKED && check_shift.GetCheck() == BST_CHECKED)
	{
		Reflection();
	}
	//Reflection();

	double mininv = NewInvMassiv[0];
	double min = NewMassiv[0];
	int minimum = 0;
	int minimuminv = 0;

	for (int i = 1; i < Length; i++)
	{
		if (NewMassiv[i] < min)
		{
			min = NewMassiv[i];
			minimum = i;
		}

		if (NewInvMassiv[i] < mininv)
		{
			mininv = NewInvMassiv[i];
			minimuminv = i;
		}
	}

	if (mininv < min)
	{
		if (check_reflect.GetCheck() == BST_CHECKED && check_shift.GetCheck() == BST_CHECKED)
		{
			Reflection();
		}
		//Reflection();

		for (int i = 0; i < Length; i++)
		{
			Buffer[i] = RestoreSignal[i];
		}

		for (int i = 0; i < Length; i++)
		{
			if ((i + minimuminv) < Length)
			{
				RestoreSignal[i] = Buffer[i + minimuminv];
			}
			else
			{
				RestoreSignal[i] = Buffer[i + minimuminv - Length];
			}
		}
	}
	else
	{
		for (int i = 0; i < Length; i++)
		{
			Buffer[i] = RestoreSignal[i];
		}

		for (int i = 0; i < Length; i++)
		{
			if ((i + minimum) < Length)
			{
				RestoreSignal[i] = Buffer[i + minimum];
			}
			else
			{
				RestoreSignal[i] = Buffer[i + minimum - Length];
			}
		}
	}
	delete[] Buffer;
	delete[] RestoreHelp;
	delete[] NewMassiv;
	delete[] NewInvMassiv;
}

void CPhaseProblemDlg::Fienup()
{
	cmplx* InitialMassiv = new cmplx[Length];
	double* RSignal = new double[Length];

	double tau = accurat;

	int j = 0;
	while (1)
	{
		j++;
		if (j == 1)
		{
			double* phase = new double[Length];
			for (int i = 0; i < Length; i++)
			{
				phase[i] = Psi();
			}

			for (int i = 0; i < Length; i++)
			{
				InitialMassiv[i].real = Spectr[i] * cos(2 * Pi * phase[i]);
				InitialMassiv[i].image = Spectr[i] * sin(2 * Pi * phase[i]);
				RSignal[i] = InitialMassiv[i].real;
			}
			delete[] phase;
		}

		fourea(InitialMassiv, Length, 1);

		for (int i = 0; i < Length; i++)
		{
			if (InitialMassiv[i].real < 0) {
				InitialMassiv[i].real = 0;
			}
			RestoreSignal[i] = InitialMassiv[i].real;
			InitialMassiv[i].image = 0;
		}
		//Sleep(1);

		fourea(InitialMassiv, Length, -1);

		double* phase = new double[Length];
		for (int i = 0; i < Length; i++)
		{
			phase[i] = atan2(InitialMassiv[i].image, InitialMassiv[i].real);
			InitialMassiv[i].real = Spectr[i] * cos(phase[i]);
			InitialMassiv[i].image = Spectr[i] * sin(phase[i]);
		}

		if (check_reflect.GetCheck() == BST_CHECKED && check_shift.GetCheck() == BST_UNCHECKED)
		{
			Reflection();
			Invalidate(0);
		}

		if (check_shift.GetCheck() == BST_CHECKED && check_reflect.GetCheck() == BST_UNCHECKED)
		{
			Shift();
			Invalidate(0);
		}

		if (check_reflect.GetCheck() == BST_CHECKED && check_shift.GetCheck() == BST_CHECKED)
		{
			Shift();
			Invalidate(0);
		}

		if (check_reflect.GetCheck() == BST_UNCHECKED && check_shift.GetCheck() == BST_UNCHECKED)
		{
			Invalidate(0);
		}

		osh = 0;
		for (int i = 0; i < Length; i++)
		{
			osh += sqrt((RSignal[i] - RestoreSignal[i]) * (RSignal[i] - RestoreSignal[i]));
			RSignal[i] = RestoreSignal[i];
		}

		sprintf_s(err, "%.10f", osh);
		static_err.SetWindowTextW((CString)err);
		if ((osh) <= tau) break;
		delete[] phase;
	}
	delete[] InitialMassiv;
}

void CPhaseProblemDlg::OnBnClickedButtonExit()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialogEx::OnCancel();
}


void CPhaseProblemDlg::OnBnClickedButtonStart()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);

	SignalFlag = true;
	SpectrFlag = true;

	Signal = new double[Length];
	Spectr = new double[Length];

	for (int i = 0; i < Length; i++)
	{
		Signal[i] = 0;
		Spectr[i] = 0;
	}

	float* signal = new float[Length];
	memset(signal, 0, Length * sizeof(float));

	for (int i = 0; i < Length; ++i)
	{
		signal[i] = function(i);
		Signal[i] = signal[i];
	}

	int is = -1;	// ППФ

	cmplx* sp = new cmplx[Length];
	for (int i = 0; i < Length; i++)
	{
		sp[i].real = signal[i];
		sp[i].image = 0;
	}

	fourea(sp, Length, is);

	float* mas_mod = new float[Length];
	memset(mas_mod, 0, Length * sizeof(float));
	for (int i = 0; i < Length; i++)
	{
		mas_mod[i] = sqrt((sp[i].real) * (sp[i].real) + (sp[i].image) * (sp[i].image));
		Spectr[i] = mas_mod[i];
	}

	Graph1(Signal, PicDc, Pic, &signal_pen, Length);
	Graph1(Spectr, PicDcSpec, PicSpec, &spectr_pen, Length);

	delete[] signal;
	delete[] sp;
	delete[] mas_mod;
}


void CPhaseProblemDlg::OnBnClickedButtonStartRecovery()
{
	// TODO: добавьте свой код обработчика уведомлений
	SignalFlag = false;
	SpectrFlag = false;
	if (!bRunTh)
	{
		button_StartRecovery.SetWindowTextW(bPauseString);
		if (hThread == NULL)
		{
			RestoreSignal = new double[Length];

			for (int i = 0; i < Length; i++)
			{
				RestoreSignal[i] = 0;
			}

			hThread = CreateThread(NULL, 0, MyProc, this, 0, &dwThread);
			VosstFlag = true;
		}
		else
		{
			ResumeThread(hThread);
		}
		bRunTh = true;
	}
	else
	{
		button_StartRecovery.SetWindowTextW(bStartString);
		bRunTh = false;

		SuspendThread(hThread);
	}
}


void CPhaseProblemDlg::OnBnClickedButtonDropRecovery()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);
	TerminateThread(hThread, 0);		//убиваем поток
	CloseHandle(hThread);
	hThread = NULL;

	button_StartRecovery.SetWindowTextW(bStartString);
	bRunTh = false;

	VosstFlag = false;
	Graph1(Signal, PicDc, Pic, &signal_pen, Length);
	static_err.SetWindowTextW(L"");
}
