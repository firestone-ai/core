#ifndef CCONTAINER_H
#define CCONTAINER_H

#include "CObjectBase.h"

#include "../../../graphics/pro/Fonts.h"

class CSvgFile;

namespace SVG
{
	class CSvgParser;

	template<typename TypeObject>
	class CContainer
	{
	public:
		CContainer(){};
		~CContainer()
		{
			Clear();
		};

		void Clear()
		{
			for (TypeObject* pObject : m_arObjects)
				delete pObject;
		}

		 bool Empty() const
		 {
			return m_arObjects.empty();
		 };

		virtual bool AddObject(TypeObject* pObject)
		{
			if (NULL != pObject)
			{
				m_arObjects.push_back(pObject);
				return true;
			}
			return false;
		};

		virtual TypeObject* GetObject(const std::wstring& wsId) const
		{
			if (wsId.empty() || m_arObjects.empty())
				return NULL;

			std::wstring wsNewId = wsId;

			size_t unFound = wsNewId.find(L'#');

			if (std::wstring::npos != unFound)
				wsNewId.erase(0, unFound + 1);

			std::vector<TypeObject*>::const_iterator oFound = std::find_if(m_arObjects.begin(), m_arObjects.end(), [&wsNewId](TypeObject* pObject){ if (wsNewId == pObject->GetId()) return true; else return false;});

			if (m_arObjects.end() != oFound)
				return *oFound;

			return NULL;
		}
	private:
		std::vector<TypeObject*> m_arObjects;

		friend class CDefs;
		friend class CText;
		friend class CMask;
		friend class CTSpan;
		friend class CMarker;
		friend class CPattern;
		friend class CGradient;
		friend class CClipPath;
		friend class CTextPath;
		friend class CGraphicsContainer;
	};

	class CGraphicsContainer : public CContainer<CSvgGraphicsObject>, public CSvgGraphicsObject
	{
	public:
		CGraphicsContainer(XmlUtils::CXmlNode& oNode, CSvgGraphicsObject* pParent = NULL);
		CGraphicsContainer(double dWidth, double dHeight, XmlUtils::CXmlNode& oNode, CSvgGraphicsObject* pParent = NULL);
		CGraphicsContainer(const CGraphicsContainer& oGraphicsContainer);

		void SetData(const std::map<std::wstring, std::wstring>& mAttributes, unsigned short ushLevel, bool bHardMode = false) override;

		bool Draw(IRenderer* pRenderer, const CDefs *pDefs, CommandeMode oMode = CommandeModeDraw, const TSvgStyles* pOtherStyles = NULL) const override;

		TRect GetWindow() const;
		TRect GetViewBox() const;
	private:
		void ApplyStyle(IRenderer* pRenderer, const TSvgStyles* pStyles, const CDefs *pDefs, int& nTypePath, Aggplus::CMatrix& oOldMatrix) const override;

		TBounds GetBounds() const override;

		friend class CPattern;
		friend class CMarker;
		friend class CMask;

		TRect m_oWindow;
		TRect m_oViewBox;
	};
}

#endif // CCONTAINER_H
