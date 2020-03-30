
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

// Диалоговое окно CPhaseProblemDlg

void CPhaseProblemDlg::Mashtab(float arr[], int dim, float* mmin, float* mmax)		//определяем функцию масштабирования
{
	*mmin = *mmax = arr[0];

	for (int i = 0; i < dim; i++)
	{
		if (*mmin > arr[i]) *mmin = arr[i];
		if (*mmax < arr[i]) *mmax = arr[i];
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
	//  DDX_Text(pDX, IDC_EDIT_CENTER_POS_2, e_center_pos1);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_2, e_center_pos2);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_3, e_center_pos3);
	DDX_Text(pDX, IDC_EDIT_AMPL_4, e_ampl4);
	DDX_Text(pDX, IDC_EDIT_AMPL_5, e_ampl5);
	DDX_Text(pDX, IDC_EDIT_DISP_4, e_disp4);
	DDX_Text(pDX, IDC_EDIT_DISP_5, e_disp5);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_4, e_center_pos4);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_5, e_center_pos5);
}

BEGIN_MESSAGE_MAP(CPhaseProblemDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CPhaseProblemDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_START, &CPhaseProblemDlg::OnBnClickedButtonStart)
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

	// перья
	setka_pen.CreatePen(		//для сетки
		PS_DOT,					//пунктирная
		1,						//толщина 1 пиксель
		RGB(0, 0, 0));			//цвет  черный

	osi_pen.CreatePen(			//координатные оси
		PS_SOLID,				//сплошная линия
		2,						//толщина 2 пикселя
		RGB(0, 0, 0));			//цвет черный

	signal_pen.CreatePen(			//график
		PS_SOLID,				//сплошная линия
		-1,						//толщина -1 пикселя
		RGB(255, 0, 0));			//цвет синий

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
		RedrawAll();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CPhaseProblemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPhaseProblemDlg::RedrawAll() 
{
	PicDc->FillSolidRect(&Pic, RGB(250, 250, 250));			//закрашиваю фон 
	PicDc->SelectObject(&osi_pen);		//выбираем перо

	//область построения
	xmin = 0;			//минимальное значение х
	xmax = Length;			//максимальное значение х
	ymin = -0.5;//mn - 0.18 * mx;			//минимальное значение y
	ymax = 5;//mx * 1.2;		//максимальное значение y

	float window_signal_width = Pic.Width();
	float window_signal_height = Pic.Height();
	xp = ((window_signal_width) / (xmax - xmin));			//Коэффициенты пересчёта координат по Х
	yp = -(window_signal_height / (ymax - ymin));			//Коэффициенты пересчёта координат по У

	//создаём Ось Y
	PicDc->MoveTo(DOTS(0, ymax));
	PicDc->LineTo(DOTS(0, ymin));
	//создаём Ось Х
	PicDc->MoveTo(DOTS(xmin, 0));
	PicDc->LineTo(DOTS(xmax, 0));

	PicDc->SelectObject(&setka_pen);

	//отрисовка сетки по y
	for (float x = 0; x <= xmax; x += Length / 10)
	{
		PicDc->MoveTo(DOTS(x, ymax));
		PicDc->LineTo(DOTS(x, ymin));
	}
	//отрисовка сетки по x
	for (float y = 0; y <= ymax; y += ymax / 6)
	{
		PicDc->MoveTo(DOTS(xmin, y));
		PicDc->LineTo(DOTS(xmax, y));
	}


	//подпись точек на оси
	CFont font;
	font.CreateFontW(16, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
	PicDc->SelectObject(font);

	//подпись осей
	PicDc->TextOutW(DOTS(Length / 10, ymax - 0.2), _T("X")); //Y
	PicDc->TextOutW(DOTS(xmax - 2, 0 + 0.8), _T("t")); //X

	//по Y с шагом 5
	for (float i = 0; i <= ymax; i += ymax / 10)
	{
		CString str;
		if (i != 0)
		{
			str.Format(_T("%.1f"), i);
			PicDc->TextOutW(DOTS(1, i + 0.25), str);
		}
	}
	//по X с шагом 0.5
	for (float j = 0; j <= xmax; j += Length / 6)
	{
		CString str;
		str.Format(_T("%.1f"), j);
		PicDc->TextOutW(DOTS(j - 1, -0.1), str);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

float CPhaseProblemDlg::function(int t)
{
	UpdateData(TRUE);
	float result = 0.0;
	float ampl[] = { e_ampl1, e_ampl2, e_ampl3, e_ampl4, e_ampl5 };
	float disp[] = { e_disp1, e_disp2, e_disp3, e_disp4, e_disp5 };
	float t0[] = { e_center_pos1, e_center_pos2, e_center_pos3, e_center_pos4, e_center_pos5 };
	for (int i = 0; i < 5; i++)
	{
		result += ampl[i] * exp(-((t - t0[i]) / disp[i]) * ((t - t0[i]) / disp[i]));
	}
	return result;
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
	RedrawAll();

	float* signal = new float[Length];
	memset(signal, 0, Length * sizeof(float));

	for (int i = 0; i < Length; ++i)
	{
		signal[i] = function(i);
	}

	PicDc->SelectObject(&signal_pen);
	PicDc->MoveTo(DOTS(0, signal[0]));

	for (int i = 0; i < Length; ++i)
	{
		PicDc->LineTo(DOTS(i, signal[i]));
	}
	delete signal;
}
