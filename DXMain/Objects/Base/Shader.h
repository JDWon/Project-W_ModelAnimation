#pragma once

#include "HLSL\BindingBufferStructs.h"

/// <summary>
/// 추상 Shader 클래스입니다. 인스턴스 생성이 불가능합니다.
/// 실제 구현은 CShader에서 처리합니다.
/// </summary>
class CShaderBase {

public:
	
	
	CShaderBase() = delete;
	virtual ~CShaderBase() = 0;

	static inline vector<byte> ReadCompiledShaderOutputFile(LPCTSTR path);
	static inline vector<byte> ReadCompliedShaderInterResource(int res);

	bool CreateVertexShaderAndInputLayoutByPath(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreateHullShaderByPath		(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreateDomainShaderByPath	(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreateGeometryShaderByPath	(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreatePixelShaderByPath	(LPCTSTR path, ID3D11Device* pd3dDevice);

	bool CreateVertexShaderAndInputLayoutByResource(int resource, ID3D11Device* pd3dDevice);
	bool CreateHullShaderByResource		(int resource, ID3D11Device* pd3dDevice);
	bool CreateDomainShaderByResource	(int resource, ID3D11Device* pd3dDevice);
	bool CreateGeometryShaderByResource	(int resource, ID3D11Device* pd3dDevice);
	bool CreatePixelShaderByResource	(int resource, ID3D11Device* pd3dDevice);

	virtual bool CreateVertexShaderAndInputLayout(ID3D11Device* pd3dDevice) { return false; }
	virtual bool CreateHullShader		(ID3D11Device* pd3dDevice) { return false; }
	virtual bool CreateDomainShader		(ID3D11Device* pd3dDevice) { return false; }
	virtual bool CreateGeometryShader	(ID3D11Device* pd3dDevice) { return false; }
	virtual bool CreatePixelShader		(ID3D11Device* pd3dDevice) { return false; }

	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

protected:

	CShaderBase(vector<D3D11_INPUT_ELEMENT_DESC>&& v) { m_vInputElementDescs = move(v); }

	vector<D3D11_INPUT_ELEMENT_DESC>		m_vInputElementDescs						;
	ID3D11InputLayout					*	m_pInputLayout					{ nullptr }	;
	ID3D11VertexShader					*	m_pVertexShader					{ nullptr }	;
	ID3D11PixelShader					*	m_pPixelShader					{ nullptr }	;
	ID3D11HullShader					*	m_pHullShader					{ nullptr }	;
	ID3D11DomainShader					*	m_pDomainShader					{ nullptr }	;
	ID3D11GeometryShader				*	m_pGeometryShader				{ nullptr }	;
	ID3D11VertexShader					*	m_pStreamOutputVertexShader		{ nullptr }	;
	ID3D11GeometryShader				*	m_pStreamOutputGeometryShader	{ nullptr }	;

};

template<	  typename DescType
			, bool _IsConstant = is_base_of<ConstantBuffer, DescType>::value
		>
class CShader;

/// <summary>
/// Shader 클래스입니다. 템플릿으로 상수 버퍼를 받습니다.
/// 이외의 경우 컴파일 에러가 납니다.
/// </summary>
template<typename DescType>
class CShader<DescType, true> : public CShaderBase {

public:
	using cbuffer = DescType;

public:

	CShader() : CShaderBase { DescType::GetInputElementDesc() } {}
	virtual ~CShader() {}
	
	/// <summary>
	/// <para>Vertex Shader 와 Input Layout 을 생성합니다.</para>
	/// <para>반드시 <value> m_vInputElementDescs </value> 에 유효한 값이 있어야만 합니다.</para>
	/// <para>m_vInputElementDescs 에 값이 없을 경우 오류가 발생하고 함수가 실패합니다.</para>
	/// </summary>
	/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
	/// <remarks> 해당 함수는 m_vInputElementDescs 가 반드시 초기화되어 유효한 값이 있어야만 정상 동작합니다. </remarks>
	/// <returns> 정상적으로 Vertex Shader 와 Input Layout 이 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
	bool CreateVertexShaderAndInputLayout(ID3D11Device* pd3dDevice)
	{
		CSOInfo buf = cbuffer::GetCSOInfo();

		if (buf.vs_ResID > 0)				return CShaderBase::CreateVertexShaderAndInputLayoutByResource(buf.vs_ResID, pd3dDevice);
		else if (buf.vs_Path != nullptr)	return CShaderBase::CreateVertexShaderAndInputLayoutByPath(buf.vs_Path, pd3dDevice);
		else if (FailureMessage(S_FALSE, TEXT("Input Layout 생성에 실패했습니다."))) return false;

		return true;
	}
	/// <summary>
	/// <para>Hull Shader 를 생성합니다.</para>
	/// </summary>
	/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
	/// <remarks> </remarks>
	/// <returns> 정상적으로 Hull Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
	bool CreateHullShader(ID3D11Device* pd3dDevice)
	{
		CSOInfo buf = cbuffer::GetCSOInfo();

		if (buf.hs_ResID > 0)				return CShaderBase::CreateHullShaderByResource(buf.hs_ResID, pd3dDevice);
		else if (buf.hs_Path != nullptr)	return CShaderBase::CreateHullShaderByPath(buf.hs_Path, pd3dDevice);
		else if (FailureMessage(S_FALSE, TEXT("Hull Shader 생성에 실패했습니다."))) return false;

		return true;
	}
	/// <summary>
	/// <para>Domain Shader 를 생성합니다.</para>
	/// </summary>
	/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
	/// <remarks> </remarks>
	/// <returns> 정상적으로 Domain Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
	bool CreateDomainShader(ID3D11Device* pd3dDevice)
	{
		CSOInfo buf = cbuffer::GetCSOInfo();

		if (buf.ds_ResID > 0)				return CShaderBase::CreateDomainShaderByResource(buf.ds_ResID, pd3dDevice);
		else if (buf.ds_Path != nullptr)	return CShaderBase::CreateDomainShaderByPath(buf.ds_Path, pd3dDevice);
		else if (FailureMessage(S_FALSE, TEXT("Domain Shader 생성에 실패했습니다."))) return false;

		return true;
	}
	/// <summary>
	/// <para>Geometry Shader 를 생성합니다.</para>
	/// </summary>
	/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
	/// <remarks> </remarks>
	/// <returns> 정상적으로 Geometry Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
	bool CreateGeometryShader(ID3D11Device* pd3dDevice)
	{
		CSOInfo buf = cbuffer::GetCSOInfo();

		if (buf.gs_ResID > 0)				return CShaderBase::CreateGeometryShaderByResource(buf.gs_ResID, pd3dDevice);
		else if (buf.gs_Path != nullptr)	return CShaderBase::CreateGeometryShaderByPath(buf.gs_Path, pd3dDevice);
		else if (FailureMessage(S_FALSE, TEXT("Geometry Shader 생성에 실패했습니다."))) return false;

		return true;
	}
	/// <summary>
	/// <para>Pixel Shader 를 생성합니다.</para>
	/// </summary>
	/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
	/// <remarks> </remarks>
	/// <returns> 정상적으로 Pixel Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
	bool CreatePixelShader(ID3D11Device* pd3dDevice)
	{
		CSOInfo buf = cbuffer::GetCSOInfo();

		if (buf.ps_ResID > 0)				return CShaderBase::CreatePixelShaderByResource(buf.ps_ResID, pd3dDevice);
		else if (buf.ps_Path != nullptr)	return CShaderBase::CreatePixelShaderByPath(buf.ps_Path, pd3dDevice);
		else if (FailureMessage(S_FALSE, TEXT("Pixel Shader 생성에 실패했습니다."))) return false;

		return true;
	}

};

class CShaderAnimation : public CShader<FBXModelConstantBuffer, true>
{

};

