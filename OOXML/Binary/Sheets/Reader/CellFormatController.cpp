/*
 * (c) Copyright Ascensio System SIA 2010-2023
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at 20A-6 Ernesta Birznieka-Upish
 * street, Riga, Latvia, EU, LV-1050.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */

#include "CellFormatController.h"

#include "../../../XlsxFormat/Styles/NumFmts.h"
#include "../../../XlsxFormat/Styles/Xfs.h"

CellFormatController::CellFormatController(OOX::Spreadsheet::CStyles *styles):
	m_pStyles{styles}
{
	// Добавим стили для wrap-а
	m_pStyles->m_oCellXfs.Init();
	m_pStyles->m_oCellXfs->m_oCount.Init();
	m_pStyles->m_oCellXfs->m_oCount->SetValue(2);

	// Normall default
	OOX::Spreadsheet::CXfs* pXfs = NULL;
	pXfs = new OOX::Spreadsheet::CXfs();
	pXfs->m_oBorderId.Init();		pXfs->m_oBorderId->SetValue(0);
	pXfs->m_oFillId.Init();			pXfs->m_oFillId->SetValue(0);
	pXfs->m_oFontId.Init();			pXfs->m_oFontId->SetValue(0);
	pXfs->m_oNumFmtId.Init();		pXfs->m_oNumFmtId->SetValue(0);

	m_pStyles->m_oCellXfs->m_arrItems.push_back(pXfs);

	// Wrap style
	pXfs = new OOX::Spreadsheet::CXfs();
	pXfs->m_oBorderId.Init();		pXfs->m_oBorderId->SetValue(0);
	pXfs->m_oFillId.Init();			pXfs->m_oFillId->SetValue(0);
	pXfs->m_oFontId.Init();			pXfs->m_oFontId->SetValue(0);
	pXfs->m_oNumFmtId.Init();		pXfs->m_oNumFmtId->SetValue(0);

	pXfs->m_oApplyAlignment.Init();	pXfs->m_oApplyAlignment->SetValue(SimpleTypes::onoffTrue);
	pXfs->m_oAligment.Init();		pXfs->m_oAligment->m_oWrapText.Init();
	pXfs->m_oAligment->m_oWrapText->SetValue(SimpleTypes::onoffTrue);

	m_pStyles->m_oCellXfs->m_arrItems.push_back(pXfs);
}

void CellFormatController::ProcessCellType(OOX::Spreadsheet::CCell *pCell, const std::wstring &value, bool bIsWrap)
{
    size_t length = value.length();
    wchar_t *pEndPtr;
	double dValue = wcstod(value.c_str(), &pEndPtr);

	if (std::isnan(dValue) || std::isinf(dValue))
		pEndPtr = (wchar_t *)value.c_str();

	if ((0 == *pEndPtr) || (pEndPtr != value.c_str() && (value.c_str() + length  - pEndPtr) < 3))
	{
		std::wstring data_format;
		std::wstring postfix;

		if (0 != *pEndPtr)
		{
			size_t sz = length - (pEndPtr - value.c_str());

			while (sz > 0)
			{
				if (pEndPtr[sz - 1] != L' ')
					break;
				sz--;
			}

			if (sz > 0)
			{
				postfix = std::wstring(pEndPtr, sz);
			}
		}

		size_t pos = value.find(L".");
		if (pos != std::wstring::npos)
		{
			size_t fraction = value.length() - pos - ((0 != *pEndPtr) ? 2 : 1);
			for (size_t i = 0; i < fraction && fraction != std::wstring::npos; ++i)
				data_format += L"0";
		}
		if (false == data_format.empty()) data_format = L"." + data_format;

		pCell->m_oValue.Init();

		if (0 != *pEndPtr)
		{
			if (false == postfix.empty())
			{
				if (postfix[0] == L'%')
				{
					pCell->m_oValue->m_sText = std::to_wstring(dValue / 100.);
				}
				else
				{
					pCell->m_oValue->m_sText = value.substr(0, length - 1);

					for (size_t i = 0; i < postfix.size(); ++i)
					{
						data_format += std::wstring(L"\\") + postfix[i];
					}
				}
			}
		}
		else
        {
			pCell->m_oValue->m_sText = value;
        }
		if (false == data_format.empty())
		{
			data_format = L"0" + data_format;

			std::map<std::wstring, unsigned int>::iterator pFind = mapDataNumber.find(data_format);
			if (pFind != mapDataNumber.end())
			{
				pCell->m_oStyle = pFind->second;
			}
			else
			{
				if (!m_pStyles->m_oNumFmts.IsInit()) m_pStyles->m_oNumFmts.Init();

				m_pStyles->m_oNumFmts->m_arrItems.push_back(new OOX::Spreadsheet::CNumFmt());
				m_pStyles->m_oNumFmts->m_arrItems.back()->m_oFormatCode = data_format;
				m_pStyles->m_oNumFmts->m_arrItems.back()->m_oNumFmtId.Init();
				m_pStyles->m_oNumFmts->m_arrItems.back()->m_oNumFmtId->SetValue(164 + m_pStyles->m_oNumFmts->m_arrItems.size());

				// Normal + data format
				OOX::Spreadsheet::CXfs* pXfs = new OOX::Spreadsheet::CXfs();

				pXfs->m_oBorderId.Init();	pXfs->m_oBorderId->SetValue(0);
				pXfs->m_oFillId.Init();		pXfs->m_oFillId->SetValue(0);
				pXfs->m_oFontId.Init();		pXfs->m_oFontId->SetValue(0);
				pXfs->m_oNumFmtId.Init();	pXfs->m_oNumFmtId->SetValue(m_pStyles->m_oNumFmts->m_arrItems.back()->m_oNumFmtId->GetValue());

				m_pStyles->m_oCellXfs->m_arrItems.push_back(pXfs);

				pCell->m_oStyle = (unsigned int)(m_pStyles->m_oCellXfs->m_arrItems.size() - 1);
				mapDataNumber.insert(std::make_pair(data_format, *pCell->m_oStyle));
			}
		}

    }
	else
	{
		if (value[0] == L'='/* && bCalcFormulas*/)
		{
			pCell->m_oFormula.Init();
			pCell->m_oFormula->m_sText = value;
		}
		else
		{
			pCell->m_oType->SetValue(SimpleTypes::Spreadsheet::celltypeInlineStr);
			pCell->m_oRichText.Init();
			OOX::Spreadsheet::CText *pText = new OOX::Spreadsheet::CText();
			pText->m_sText = value;
			pCell->m_oRichText->m_arrItems.push_back(pText);
		}
	}

	if (bIsWrap)
	{
		pCell->m_oStyle = 1;
	}
}