#pragma once
#include "../../Core/Engine.h"
#if defined(_WIN32)
#include "../../Platform/Windows/WindowsEngine.h"
#else
#endif



class IRenderingInterface
{
	friend class FWindowsEngine;

public:
	IRenderingInterface();
	virtual ~IRenderingInterface();

	virtual void Init();

	virtual void Draw(float DeltaTime);

	bool operator==(const IRenderingInterface& InOther)
	{
		return guid_equal(&Guid, &InOther.Guid);
	}
	simple_c_guid GetGuid() { return Guid; }
protected:
	ComPtr<ID3D12Device> GetD3dDevice();
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList();
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator();

protected:
	//��Դ����Buffer����
	ComPtr<ID3D12Resource> ConstructDefaultBuffer(ComPtr<ID3D12Resource>& OutTmpBuffer, const void* InData, UINT64 InDataSize);

#if defined(_WIN32)
	FWindowsEngine* GetEngine();
#else
	FEngine* GetEngine();
#endif

	

private:
	static vector<IRenderingInterface*> RenderingInterface;
	simple_c_guid Guid;
};
//�����������洢�ṹ
class FRenderingResourcesUpdate :public enable_shared_from_this<FRenderingResourcesUpdate>
{
public:
	FRenderingResourcesUpdate();
	~FRenderingResourcesUpdate();

	void Init(ID3D12Device* InDevice, UINT InElemetSize, UINT InElemetCount);//��ʼ��������Դ

	void Update(int Index, const void* InData);//��������

	UINT GetConstantBufferByteSize(UINT InTypeSzie);//��ó��������ֽ�Size
	UINT GetConstantBufferByteSize();

	ID3D12Resource* GetBuffer() { return UploadBuffer.Get(); } //���Buffer
private:
	ComPtr<ID3D12Resource> UploadBuffer;//�ϴ�Buffer
	UINT ElementSize;//Ԫ�ش�С
	BYTE* Data;//ʵ������
};