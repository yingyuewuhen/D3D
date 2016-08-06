#include "BillBoard.h"
#include "Convert.h"
#include <d3dx9.h>


const DWORD BillBoard::BILL_BOARD_VERTEX::FVF = D3DFVF_XYZ | D3DFVF_TEX1;


BillBoard::BillBoard(IDirect3DDevice9 *device)
	:m_device(device), m_pos(0, 0, 0), m_size(50, 50, 0)
{
	D3DXMatrixIdentity(&m_mat);

	Init();
}

BillBoard::~BillBoard()
{
	if (m_vb)
		m_vb->Release();
	if (m_tex)
		m_tex->Release();

}

bool BillBoard::LoadTexture(std::string fileName)
{
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(fileName).c_str(), &m_tex)))
		return false;
	return true;
}

void BillBoard::Render()
{
	if (m_device)
	{

		D3DXMATRIX mat;
		///D3DXMatrixTranslation(&mat, -m_pos.x, -m_pos.y, -m_pos.z);
		//m_mat *= mat;
		m_device->SetTransform(D3DTS_WORLD, &m_mat);


		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_device->SetRenderState(D3DRS_LIGHTING, false);

		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


		m_device->SetStreamSource(0, m_vb, 0, sizeof(BILL_BOARD_VERTEX));

		m_device->SetTransform(D3DTS_WORLD, &m_mat);

		m_device->SetTexture(0, m_tex);

		m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		m_device->SetRenderState(D3DRS_LIGHTING, true);
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}
}

void BillBoard::Update(DWORD elapsed, const Camera &cam)
{
	//更新广告版的方向
	D3DXVECTOR3 camPos = cam.getPosition();

	D3DXVECTOR3 lookAt = cam.getLook();
	D3DXVECTOR3 vDir = lookAt - camPos;

	D3DXVec3Normalize(&vDir, &vDir);

	static D3DXMATRIX matBillBoard;

	if (vDir.x > 0.0f)
		D3DXMatrixRotationY(&matBillBoard, -atanf(vDir.z / vDir.x) + D3DX_PI / 2);
	else
		D3DXMatrixRotationY(&matBillBoard, -atanf(vDir.z / vDir.x) - D3DX_PI / 2);
	
	m_mat = matBillBoard;
}

bool BillBoard::Init()
{
	if (FAILED(m_device->CreateVertexBuffer(4 * sizeof(BILL_BOARD_VERTEX), D3DUSAGE_WRITEONLY,
		BILL_BOARD_VERTEX::FVF, D3DPOOL_MANAGED, &m_vb, 0)))
		return false;

	BILL_BOARD_VERTEX *vertex = 0;

	m_vb->Lock(0, 0, (void **)&vertex, 0);
	vertex[0] = BILL_BOARD_VERTEX(0, 0, 0, 0, 1);
	vertex[1] = BILL_BOARD_VERTEX(m_size.x, 0, 0, 1, 1);
	vertex[2] = BILL_BOARD_VERTEX(m_size.x, m_size.y, 0, 1, 0);
	vertex[3] = BILL_BOARD_VERTEX(0, m_size.y, 0, 0, 0);

	m_vb->Unlock();


}

void BillBoard::SetPos(const D3DXVECTOR3 &pos)
{
	m_pos = pos;
}
