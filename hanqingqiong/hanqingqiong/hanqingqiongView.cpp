
// hanqingqiongView.cpp: ChanqingqiongView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "hanqingqiong.h"
#endif

#include "hanqingqiongDoc.h"
#include "hanqingqiongView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChanqingqiongView

IMPLEMENT_DYNCREATE(ChanqingqiongView, CView)

BEGIN_MESSAGE_MAP(ChanqingqiongView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_OPEN, &ChanqingqiongView::OnFileOpen)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// ChanqingqiongView 构造/析构

ChanqingqiongView::ChanqingqiongView() noexcept
{
	// TODO: 在此处添加构造代码
	this->m_GLPixelIndex = 0;
	this->m_hGLContext = NULL;

	m_read= 0;

	m_xrotation = 0.0;
	m_yrotation = 0.0;

	m_leftButtonDown = FALSE;


}

ChanqingqiongView::~ChanqingqiongView()
{
}

BOOL ChanqingqiongView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	return CView::PreCreateWindow(cs);
}

// ChanqingqiongView 绘图

void ChanqingqiongView::OnDraw(CDC* pDC)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(m_xrotation, 1.0, 0.0, 0.0);
	glRotatef(m_yrotation, 0.0, 1.0, 0.0);

	if (m_read==1)
		m_obj.DrawOBJ();
	else if (m_read == 2)
		rw.DrawObject();

	SwapBuffers(pDC->m_hDC);

}


// ChanqingqiongView 打印

BOOL ChanqingqiongView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void ChanqingqiongView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void ChanqingqiongView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// ChanqingqiongView 诊断

#ifdef _DEBUG
void ChanqingqiongView::AssertValid() const
{
	CView::AssertValid();
}

void ChanqingqiongView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

ChanqingqiongDoc* ChanqingqiongView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ChanqingqiongDoc)));
	return (ChanqingqiongDoc*)m_pDocument;
}
#endif //_DEBUG


// ChanqingqiongView 消息处理程序
BOOL ChanqingqiongView::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR  pixelDesc = { sizeof(PIXELFORMATDESCRIPTOR), 1,
																			   PFD_DRAW_TO_WINDOW |
																			   PFD_SUPPORT_OPENGL |
																			   PFD_DOUBLEBUFFER,
																			   PFD_TYPE_RGBA,   24,  0,0,0,0,0,0,  0,  0,  0,
																			   0,0,0,0,  32,   0,  0, 0,  0,  0,0,0 };
	m_GLPixelIndex = ChoosePixelFormat(hDC, &pixelDesc);

	if (m_GLPixelIndex == 0)
	{
		return FALSE;
	}

	if (SetPixelFormat(hDC, m_GLPixelIndex, &pixelDesc) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL ChanqingqiongView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	if (m_hGLContext == NULL) //创建失败
	{
		return FALSE;
	}

	if (wglMakeCurrent(hDC, m_hGLContext) == FALSE)
	{//选为当前RC失败
		return FALSE;
	}
	return TRUE;
}

int ChanqingqiongView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	HWND hWnd = this->GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	if (this->SetWindowPixelFormat(hDC) == FALSE)
	{
		return 0;
	}

	if (this->CreateViewGLContext(hDC) == FALSE)
	{
		return 0;
	}

	glewInit();
	glDrawBuffer(GL_BACK);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	///////////////////
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float light_ambient[] = { 0.1,0.1,0.1,1 };
	float light_diffuse[] = { 1,1,1,1 };
	float light_specular[] = { 1,1,1,1 };
	float light_position[] = { 1,1,1,0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	///////////////////


	return 0;
}

void ChanqingqiongView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	if (wglGetCurrentContext() != NULL)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (this->m_hGLContext != NULL)
	{
		wglDeleteContext(this->m_hGLContext);
		this->m_hGLContext = NULL;
	}
}

void ChanqingqiongView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	glViewport(0, 0, cx, cy);
	GLdouble aspect;
	if (cy == 0)
	{
		aspect = (GLdouble)cx;
	}
	else
	{
		aspect = (GLdouble)cx / (GLdouble)cy;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-3.0 * aspect, 3.0 * aspect, -3.0, 3.0, -13.0, 13.0);
	Invalidate();

}

BOOL ChanqingqiongView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CView::OnEraseBkgnd(pDC);
	return true;
}

void ChanqingqiongView::OnFileOpen()
{
	CString filename;
	TCHAR strFilter[] = _T("OBJ file (*.obj)|*.obj|PLY file (*.ply)|*.ply|所有文件 (*.*)|*.*||");
	CFileDialog fDlg(TRUE, _T("obj"), _T("*.obj"),
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter, this);
	TCHAR strTitle[] = _T("Open Model file...");
	fDlg.m_ofn.lpstrTitle = strTitle;

	if (fDlg.DoModal() == IDOK)
	{
		filename = fDlg.GetPathName();
		// CStringA 负责把 Unicode 路径转换为当前系统代码页，避免手动使用 len。
		CStringA ansiFilename(filename);
		char* buffer = ansiFilename.GetBuffer();
		if (buffer == NULL || buffer[0] == '\0')
		{
			ansiFilename.ReleaseBuffer();
			return;
		}

		CString extension = filename;
		extension.MakeLower();
		if (extension.Right(4) == _T(".ply"))
		{
			ReadPlyInCore reader;
			int nvf[2] = { 0, 0 };
			reader.get_nverts_nfaces(buffer, nvf);
			if (nvf[0] <= 0 || nvf[1] <= 0)
			{
				ansiFilename.ReleaseBuffer();
				AfxMessageBox(_T("PLY 文件中未找到有效的顶点或三角面数据。"));
				return;
			}

			rw.init(nvf[0], nvf[1]);
			FILE* input = fopen(buffer, "rb");
			if (input == NULL)
			{
				ansiFilename.ReleaseBuffer();
				AfxMessageBox(_T("无法打开 PLY 文件。"));
				return;
			}
			reader.read_ply_incore(input, &rw);
			fclose(input);

			rw.bb_center();
			rw.bb_normolize();
			m_read = 2;
		}
		else
		{
			m_obj.readOBJ(buffer);
			m_read = 1;
		}
		ansiFilename.ReleaseBuffer();
	}

	Invalidate(TRUE);
}

void ChanqingqiongView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_leftButtonDown = TRUE;
	m_leftDownPos = point;

	CView::OnLButtonDown(nFlags, point);
}

void ChanqingqiongView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_leftButtonDown)
	{
		m_yrotation += (point.x - m_leftDownPos.x) / 3.0f;
		m_xrotation += (point.y - m_leftDownPos.y) / 3.0f;
		m_leftDownPos = point;
		Invalidate();
	}

	CView::OnMouseMove(nFlags, point);
}

void ChanqingqiongView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_leftButtonDown = FALSE;

	CView::OnLButtonUp(nFlags, point);
}
