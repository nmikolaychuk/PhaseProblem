
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
	double function(int t);
	double Psi();
	void Fienup();
	void Reflection();
	void Shift();
	void Graph1(double*, CDC*, CRect, CPen*, double);
	void Graph2(double*, CPen*, double*, CPen*, CDC*, CRect, double);

	DWORD dwThread;
	HANDLE hThread;

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

	double Pi = 3.141592653589;

	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStartRecovery();

	int Length;
	double e_ampl1;
	double e_ampl2;
	double e_ampl3;
	double e_disp1;
	double e_disp2;
	double e_disp3;
	double e_center_pos1;
	double e_center_pos2;
	double e_center_pos3;
	double e_ampl4;
	double e_ampl5;
	double e_disp4;
	double e_disp5;
	double e_center_pos4;
	double e_center_pos5;

	double* Signal;
	double* Spectr;
	double* RestoreSignal;

	bool SignalFlag, SpectrFlag, VosstFlag;
	double Min, Max, Min1, Max1, Min2, Max2;
	double xx0, xxmax, yy0, yymax, xxi, yyi;
	
	double osh;
	char err[100];
	char znach[1000];

	CButton check_reflect;
	CButton check_shift;
	CStatic static_err;
	double accurat;

	BOOL bRunTh = false;
	CButton button_StartRecovery;
	CString bStartString = L"Восстановить";
	CString bPauseString = L"Остановить";

	typedef struct cmplx { float real; float image; } Cmplx;
	//========================================================
	void CPhaseProblemDlg::fourea(struct cmplx* data, int n, int is);
	afx_msg void OnBnClickedButtonDropRecovery();
};
