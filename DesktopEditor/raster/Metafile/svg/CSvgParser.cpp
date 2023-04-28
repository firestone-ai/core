#include "CSvgParser.h"

#include "SvgUtils.h"

#include <iostream>

#include "CSvgFile.h"

#include "SvgObjects/CContainer.h"
#include "SvgObjects/CPolyline.h"
#include "SvgObjects/CGradient.h"
#include "SvgObjects/CClipPath.h"
#include "SvgObjects/CPattern.h"
#include "SvgObjects/CEllipse.h"
#include "SvgObjects/CCircle.h"
#include "SvgObjects/CStyle.h"
#include "SvgObjects/CImage.h"
#include "SvgObjects/CRect.h"
#include "SvgObjects/CLine.h"
#include "SvgObjects/CPath.h"
#include "SvgObjects/CText.h"
#include "SvgObjects/CUse.h"

namespace SVG
{
	CSvgParser::CSvgParser()
	    : m_pFontManager(NULL)
	{
	}

	CSvgParser::~CSvgParser()
	{
	}

	void CSvgParser::SetFontManager(NSFonts::IFontManager *pFontManager)
	{
		m_pFontManager = pFontManager;
	}

	bool CSvgParser::LoadFromFile(const std::wstring &wsFile, CGraphicsContainer*& pContainer, CSvgFile* pFile) const
	{
		if (wsFile.empty() || NULL == pFile)
			return false;

		std::wstring wsXml;
		NSFile::CFileBinary::ReadAllTextUtf8(wsFile, wsXml);

		XmlUtils::IXmlDOMDocument::DisableOutput();
		bool bResult = LoadFromString(wsXml, pContainer, pFile);
		XmlUtils::IXmlDOMDocument::EnableOutput();

		return bResult;
	}

	bool CSvgParser::LoadFromString(const std::wstring &wsContent, CGraphicsContainer*& pContainer, CSvgFile* pFile) const
	{
		if (wsContent.empty() || NULL == pFile)
			return false;

		XmlUtils::CXmlNode oXml;
		if (!oXml.FromXmlString(wsContent))
			return false;

		// TODO:: Временный вариант
		// В дальнейшем сделать либо мнетод поиска стиля и defs,
		// (проблема в том что тогда придется 2 раза запускать поиск (сначала стиля, а потом defs),
		// а лишь потом запускать само чтение)
		// либо использовать SvgCalculator при отрисовке, а не при чтении
		// (но тогда скорость самой отрисовки падает)
		ScanStyles(oXml, pFile);
		ScanDefs  (oXml, pFile);

		return LoadFromXmlNode(oXml, pContainer, pFile);
	}

	bool CSvgParser::LoadFromXmlNode(XmlUtils::CXmlNode &oElement, CGraphicsContainer*& pContainer, CSvgFile* pFile) const
	{
		if (NULL == pFile || !oElement.IsValid())
			return false;

		if (NULL == pContainer)
			pContainer = new CGraphicsContainer(oElement);

		const CSvgCalculator *pSvgCalculator = pFile->GetSvgCalculator();

		if (NULL != pSvgCalculator)
			pSvgCalculator->SetData(pContainer);

		return ReadChildrens(oElement, pContainer, pFile, pContainer);
	}

	template <typename TypeContainer>
	bool CSvgParser::ReadGraphicsObject(XmlUtils::CXmlNode &oElement, CContainer<TypeContainer>* pContainer, CSvgFile *pFile, CSvgGraphicsObject* pParent) const
	{
		if (NULL == pFile || NULL == pContainer)
			return false;

		std::wstring wsElementName = oElement.GetName();

		CSvgGraphicsObject *pObject = NULL;

		if (L"svg" == wsElementName || L"g" == wsElementName)
		{
			CGraphicsContainer *pNewContainer = new CGraphicsContainer(oElement, pParent);

			if (NULL == pNewContainer)
				return false;

			if (LoadFromXmlNode(oElement, pNewContainer, pFile))
			{
				pContainer->AddObject(pNewContainer);
				return true;
			}
			else
				return false;
		}
		else if (L"line" == wsElementName)
			pObject = new CLine(oElement, pParent);
		else if (L"rect" == wsElementName)
			pObject = new CRect(oElement, pParent);
		else if (L"circle" == wsElementName)
			pObject = new CCircle(oElement, pParent);
		else if (L"ellipse" == wsElementName)
			pObject = new CEllipse(oElement, pParent);
		else if (L"path" == wsElementName)
			pObject = new CPath(oElement, pParent);
		else if (L"textPath" == wsElementName)
		{
			CTextPath *pTextPath = CTextPath::Create(oElement, pParent, m_pFontManager, pFile);

			if (!AddObject(pTextPath, pContainer, pFile))
				return false;

			return ReadChildrens(oElement, pTextPath, pFile, pTextPath);
		}
		else if (L"text" == wsElementName)
		{
			CText *pText = CText::Create(oElement, pParent, m_pFontManager);

			if (!AddObject(pText, pContainer, pFile))
				return false;

			return ReadChildrens(oElement, pText, pFile, pText);
		}
		else if (L"tspan" == wsElementName)
		{
			CTSpan *pTSpan = CTSpan::Create(oElement, pParent, m_pFontManager);

			if (!AddObject(pTSpan, pContainer, pFile))
				return false;

			return ReadChildrens(oElement, pTSpan, pFile, pTSpan);
		}
		else if (L"polyline" == wsElementName)
			pObject = new CPolyline(oElement, pParent);
		else if (L"polygon" == wsElementName)
			pObject = new CPolygon(oElement, pParent);
		else if (L"image" == wsElementName)
			pObject = new CImage(oElement, pParent);
		else if (L"use" == wsElementName)
			pObject = new CUse(oElement, pParent, pFile);

		return AddObject(pObject, pContainer, pFile);
	}

	bool CSvgParser::ReadDefs(XmlUtils::CXmlNode &oElement, CDefs *pDefs, CSvgFile *pFile) const
	{
		if (NULL == pFile || NULL == pDefs)
			return false;

		std::wstring wsElementName = oElement.GetName();

		if (L"defs" == wsElementName)
			return ReadChildrens(oElement, pDefs, pFile);

		CDefObject* pDefObject = NULL;

		if(L"linearGradient" == wsElementName)
			pDefObject = CreateAndReadChildrens<CLinearGradient, CDefs>(oElement, pFile);
		else if (L"radialGradient" == wsElementName)
			pDefObject = CreateAndReadChildrens<CRadialGradient, CDefs>(oElement, pFile);
		else if (L"stop" == wsElementName)
			pDefObject = new CStopElement(oElement);
		else if (L"pattern" == wsElementName)
			pDefObject = CreateAndReadChildrens<CPattern, CGraphicsContainer>(oElement, pFile);
		else if (L"clipPath" == wsElementName)
			pDefObject = CreateAndReadChildrens<CClipPath, CGraphicsContainer>(oElement, pFile);

		return AddObject(pDefObject, pDefs, pFile);
	}

	bool CSvgParser::ScanDefs(XmlUtils::CXmlNode &oElement, CSvgFile *pFile) const
	{
		if (NULL == pFile || !oElement.IsValid())
			return false;

		XmlUtils::CXmlNodes arChilds;

		oElement.GetChilds(arChilds);

		XmlUtils::CXmlNode oChild;

		for (unsigned int unChildrenIndex = 0; unChildrenIndex < arChilds.GetCount(); ++unChildrenIndex)
		{
			if (!arChilds.GetAt(unChildrenIndex, oChild))
				break;

			if (L"defs" == oChild.GetName())
				pFile->AddDefs(oChild);

			oChild.Clear();
		}

		return true;
	}

	bool CSvgParser::ScanStyles(XmlUtils::CXmlNode &oElement, CSvgFile *pFile) const
	{
		if (NULL == pFile || !oElement.IsValid())
			return false;

		XmlUtils::CXmlNodes arChilds;

		oElement.GetChilds(arChilds);

		XmlUtils::CXmlNode oChild;

		for (unsigned int unChildrenIndex = 0; unChildrenIndex < arChilds.GetCount(); ++unChildrenIndex)
		{
			if (!arChilds.GetAt(unChildrenIndex, oChild))
				break;

			if (L"style" == oChild.GetName())
				pFile->AddStyles(oChild.GetText());
			else if (L"defs" == oChild.GetName())
				ScanStyles(oChild, pFile);

			oChild.Clear();
		}

		return true;
	}

	template<typename TypeContainer>
	bool CSvgParser::ReadChildrens(XmlUtils::CXmlNode &oElement, CContainer<TypeContainer>* pContainer, CSvgFile* pFile, CSvgGraphicsObject* pParent) const
	{
		if (NULL == pContainer || NULL == pFile)
			return false;

		XmlUtils::CXmlNodes arChilds;

		oElement.GetChilds(arChilds);

		XmlUtils::CXmlNode oChild;

		for (unsigned int unChildrenIndex = 0; unChildrenIndex < arChilds.GetCount(); ++unChildrenIndex)
		{
			if (!arChilds.GetAt(unChildrenIndex, oChild))
				break;

			if (std::is_same_v<CDefObject, TypeContainer>)
				ReadDefs(oChild, (CDefs*)(pContainer), pFile);
			else
				ReadGraphicsObject(oChild, (CGraphicsContainer*)pContainer, pFile, pParent);

			oChild.Clear();
		}

		return true;
	}

	template <typename TypeObject, typename TypeContainer>
	bool CSvgParser::AddObject(TypeObject *pObject, CContainer<TypeContainer> *pContainer, CSvgFile *pFile) const
	{
		if (NULL == pObject || NULL == pContainer)
			return false;

		if (!pContainer->AddObject(pObject))
		{
			delete pObject;
			return false;
		}

		if (NULL == pFile)
			return true;

		pFile->AddMarkedObject(dynamic_cast<CSvgGraphicsObject*>(pObject));

		const CSvgCalculator *pSvgCalculator = pFile->GetSvgCalculator();

		if (NULL != pSvgCalculator)
			pSvgCalculator->SetData(pObject);

		return true;
	}

	template<typename ElementClass, typename ContainerClass>
	CDefObject *CSvgParser::CreateAndReadChildrens(XmlUtils::CXmlNode &oElement, CSvgFile* pFile) const
	{
		if (!oElement.IsValid())
			return NULL;

		ElementClass* pSvgElement = new ElementClass(oElement);
		if (ReadChildrens(oElement, (ContainerClass*)pSvgElement, pFile))
			return pSvgElement;
		else
			delete pSvgElement;

		return NULL;
	}

}
