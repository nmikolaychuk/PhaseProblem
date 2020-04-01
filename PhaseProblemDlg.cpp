
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
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_2, e_center_pos2);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_3, e_center_pos3);
	DDX_Text(pDX, IDC_EDIT_AMPL_4, e_ampl4);
	DDX_Text(pDX, IDC_EDIT_AMPL_5, e_ampl5);
	DDX_Text(pDX, IDC_EDIT_DISP_4, e_disp4);
	DDX_Text(pDX, IDC_EDIT_DISP_5, e_disp5);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_4, e_center_pos4);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_5, e_center_pos5);
	DDX_Control(pDX, IDC_BUTTON_START_RECOVERY, button_StartRecovery);
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
	float* signal = new float[Length];
	memset(signal, 0, Length * sizeof(float));

	for (int i = 0; i < Length; ++i)
	{
		signal[i] = function(i);
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
	}


	Mashtab(signal, Length, &mn, &mx);
	Mashtab(mas_mod, Length, &mnspec, &mxspec);


	PicDc->FillSolidRect(&Pic, RGB(250, 250, 250));			//закрашиваю фон 
	PicDc->SelectObject(&osi_pen);		//выбираем перо

	//область построения
	xmin = -Length/14;			//минимальное значение х
	xmax = Length;			//максимальное значение х
	ymin = mn - 0.18 * mx;			//минимальное значение y
	ymax = mx * 1.2;		//максимальное значение y

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
	for (float x = 0; x <= xmax; x += Length / 14)
	{
		if (x != 0) {
			PicDc->MoveTo(DOTS(x, ymax));
			PicDc->LineTo(DOTS(x, ymin));
		}
	}
	//отрисовка сетки по x
	for (float y = 0; y < ymax; y += ymax / 6)
	{
		if (y != 0) {
			PicDc->MoveTo(DOTS(xmin, y));
			PicDc->LineTo(DOTS(xmax, y));
		}
	}


	//подпись точек на оси
	CFont font;
	font.CreateFontW(14.5, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
	PicDc->SelectObject(font);

	//подпись осей
	PicDc->TextOutW(DOTS(0.01*Length, 0.99*ymax), _T("A")); //Y
	PicDc->TextOutW(DOTS(xmax - Length/30, -ymin - 0.17 * ymax), _T("t")); //X

	//по Y с шагом 5
	for (float i = 0; i <= ymax; i += ymax / 6)
	{
		CString str;
		if (i != 0)
		{
			str.Format(_T("%.2f"), i);
			PicDc->TextOutW(DOTS(xmin + Length / 40, i + 0.05 * ymax), str);
		}
	}
	//по X с шагом 0.5
	for (float j = 0; j <= xmax; j += Length / 14)
	{
		CString str;
		if (j != 0) {
			str.Format(_T("%.0f"), j);
			PicDc->TextOutW(DOTS(j - Length / 100, -ymin - 0.17*ymax), str);
		}
	}
	


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	PicDcSpec->FillSolidRect(&PicSpec, RGB(250, 250, 250));			//закрашиваю фон 
	PicDcSpec->SelectObject(&osi_pen);		//выбираем перо

	//область построения
	xminspec = -Length/14;			//минимальное значение х
	xmaxspec = Length;			//максимальное значение х
	yminspec = mnspec - 0.18 * mxspec;			//минимальное значение y
	ymaxspec = mxspec * 1.2;		//максимальное значение y

	float window_spectr_width = PicSpec.Width();
	float window_spectr_height = PicSpec.Height();
	xpspec = ((window_spectr_width) / (xmaxspec - xminspec));			//Коэффициенты пересчёта координат по Х
	ypspec = -(window_spectr_height / (ymaxspec - yminspec));			//Коэффициенты пересчёта координат по У

	//создаём Ось Y
	PicDcSpec->MoveTo(DOTSSPEC(0, ymaxspec));
	PicDcSpec->LineTo(DOTSSPEC(0, yminspec));
	//создаём Ось Х
	PicDcSpec->MoveTo(DOTSSPEC(xminspec, 0));
	PicDcSpec->LineTo(DOTSSPEC(xmaxspec, 0));

	PicDcSpec->SelectObject(&setka_pen);

	//отрисовка сетки по y
	for (float x = 0; x <= xmaxspec; x += Length / 14)
	{
		if (x != 0) {
			PicDcSpec->MoveTo(DOTSSPEC(x, ymaxspec));
			PicDcSpec->LineTo(DOTSSPEC(x, yminspec));
		}
	}
	//отрисовка сетки по x
	for (float y = 0; y < ymaxspec; y += ymaxspec / 6)
	{
		if (y != 0) {
			PicDcSpec->MoveTo(DOTSSPEC(xminspec, y));
			PicDcSpec->LineTo(DOTSSPEC(xmaxspec, y));
		}
	}


	//подпись точек на оси
	CFont font1;
	font1.CreateFontW(14.5, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
	PicDcSpec->SelectObject(font1);

	//подпись осей
	PicDcSpec->TextOutW(DOTSSPEC(0.01 * Length, 0.99 * ymaxspec), _T("A")); //Y
	PicDcSpec->TextOutW(DOTSSPEC(xmaxspec - Length / 30, -ymin - 0.3 * ymax), _T("f")); //X

	//по Y с шагом 5
	for (float i = 0; i <= ymaxspec; i += ymaxspec / 6)
	{
		CString str;
		if (i != 0)
		{
			str.Format(_T("%.2f"), i);
			PicDcSpec->TextOutW(DOTSSPEC(xminspec + Length/40, i + 0.05 * ymaxspec), str);
		}
	}
	//по X с шагом 0.5
	for (float j = 0; j <= xmaxspec; j += Length / 14)
	{
		CString str;
		if (j != 0)
		{
			str.Format(_T("%.2f"), j / Length);
			PicDcSpec->TextOutW(DOTSSPEC(j - Length/100, -yminspec - 0.17 * ymaxspec), str);
		}
	}

	delete signal;
	delete sp;
	delete mas_mod;

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

	PicDcSpec->SelectObject(&spectr_pen);

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
	}

	PicDcSpec->MoveTo(DOTSSPEC(0, mas_mod[0]));

	for (int i = 0; i < Length; i++)
	{
		PicDcSpec->LineTo(DOTSSPEC(i, mas_mod[i]));
	}

	delete signal;
	delete sp;
	delete mas_mod;
}


void CPhaseProblemDlg::OnBnClickedButtonStartRecovery()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);
	if (!bStartRec)
	{
		button_StartRecovery.SetWindowTextW(bPauseString);
		bStartRec = true;
	}
	else
	{
		button_StartRecovery.SetWindowTextW(bStartString);
		bStartRec = false;
	}
}


void CPhaseProblemDlg::OnBnClickedButtonDropRecovery()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);
	CString bCheckString = NULL;
	button_StartRecovery.GetWindowTextW(bCheckString);
	if (!bStartRec && bCheckString != bDefaultString)
	{
		MessageBox(L"Для сброса необходимо остановить процесс восстановления!", L"Ошибка", MB_OK | MB_ICONERROR);
	}
	else
	{
		button_StartRecovery.SetWindowTextW(bDefaultString);
	}
}
