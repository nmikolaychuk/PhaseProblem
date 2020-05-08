
// PhaseProblemDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CPhaseProblemDlg
class CPhaseProblemDlg : public CDialogEx
{
	// Создание
public:
	CPhaseProblemDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PHASEPROBLEM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CWnd* PicWnd;				//области рисования
	CDC* PicDc;
	CRect Pic;

	CWnd* PicWndSpec;				//области рисования
	CDC* PicDcSpec;
	CRect PicSpec;

	CPen osi_pen;				//ручки
	CPen setka_pen;
	CPen signal_pen;
	CPen spectr_pen;
	CPen vosstanovl_pen;

	float xp, yp,				//коэфициенты пересчета
		xmin, xmax,				//максисимальное и минимальное значение х 
		ymin, ymax,				//максисимальное и минимальное значение y
		mn, mx;					//коэффициенты масштабирования

	float xpspec, ypspec,				//коэфициенты пересчета
		xminspec, xmaxspec,				//максисимальное и минимальное значение х 
		yminspec, ymaxspec,				//максисимальное и минимальное значение y
		mnspec, mxspec;					//коэффициенты масштабирования

	float Pi = 3.141592653589;

	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStartRecovery();
	void RedrawAll();
	void CPhaseProblemDlg::Mashtab(float arr[], int dim, float* mmin, float* mmax);
	float CPhaseProblemDlg::function(int t);
	float CPhaseProblemDlg::Psi();
	void CPhaseProblemDlg::Fienup();
	void CPhaseProblemDlg::Reflection();
	void CPhaseProblemDlg::Shift();

	float RestoreSignal[1024];
	float Signal[1024];
	float Spectr[1024];

	int Length;
	float e_ampl1;
	float e_ampl2;
	float e_ampl3;
	float e_disp1;
	float e_disp2;
	float e_disp3;
	float e_center_pos1;
	float e_center_pos2;
	float e_center_pos3;
	float e_ampl4;
	float e_ampl5;
	float e_disp4;
	float e_disp5;
	float e_center_pos4;
	float e_center_pos5;
	CButton button_StartRecovery;
	BOOL bStartRec = false;

	CString st_error;
	CButton check_reflect;
	CButton check_shift;
	float accurat;

	CString bStartString = L"Продолжить восстановление";
	CString bPauseString = L"Остановить восстановление";
	CString bDefaultString = L"Запуск восстановления";

	typedef struct cmplx { float real; float image; } Cmplx;
	//========================================================
	void CPhaseProblemDlg::fourea(struct cmplx* data, int n, int is);
	afx_msg void OnBnClickedButtonDropRecovery();
};
