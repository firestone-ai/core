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

#include "BWMode.h"

namespace PPTX
{
	namespace Limit
	{		
		BWMode::BWMode()
		{
			m_strValue = L"auto";
		}
		void BWMode::set(const std::wstring& strValue)
		{
			if ((L"auto"			== strValue) ||
				(L"black"		== strValue) ||
				(L"blackGray"	== strValue) ||
				(L"blackWhite"	== strValue) ||
				(L"clr"			== strValue) ||
				(L"gray"			== strValue) ||
				(L"grayWhite"	== strValue) ||
				(L"hidden"		== strValue) ||
				(L"invGray"		== strValue) ||
				(L"ltGray"		== strValue) ||
				(L"white"		== strValue))
			{
				m_strValue = strValue;
			}
		}
		unsigned char BWMode::GetBYTECode() const
		{
			if (L"auto" == m_strValue)		return 0;
			if (L"black" == m_strValue)		return 1;
			if (L"blackGray" == m_strValue)	return 2;
			if (L"blackWhite" == m_strValue)return 3;
			if (L"clr" == m_strValue)		return 4;
			if (L"gray" == m_strValue)		return 5;
			if (L"grayWhite" == m_strValue)	return 6;
			if (L"hidden" == m_strValue)	return 7;
			if (L"invGray" == m_strValue)	return 8;
			if (L"ltGray" == m_strValue)	return 9;
			if (L"white" == m_strValue)		return 10;
			return 0;
		}
		void BWMode::SetBYTECode(const unsigned char& src)
		{
			switch (src)
			{
			case 0: m_strValue = L"auto"; break;
			case 1: m_strValue = L"black"; break;
			case 2: m_strValue = L"blackGray"; break;
			case 3: m_strValue = L"blackWhite"; break;
			case 4: m_strValue = L"clr"; break;
			case 5: m_strValue = L"gray"; break;
			case 6: m_strValue = L"grayWhite"; break;
			case 7: m_strValue = L"hidden"; break;
			case 8: m_strValue = L"invGray"; break;
			case 9: m_strValue = L"ltGray"; break;
			case 10: m_strValue = L"white"; break;
			default:
				m_strValue = L"auto";
				break;
			}
		}
//------------------------------------------------------------------------------------------
		PrintClrMode::PrintClrMode()
		{
			m_strValue = L"clr";
		}
		void PrintClrMode::set(const std::wstring& strValue)
		{
			if ((L"bw" == strValue) ||
				(L"clr" == strValue) ||
				(L"gray" == strValue))
			{
				m_strValue = strValue;
			}
		}
		unsigned char PrintClrMode::GetBYTECode() const
		{
			if (L"bw" == m_strValue)		return 0;
			if (L"clr" == m_strValue)		return 1;
			if (L"gray" == m_strValue)		return 2;
			return 0;
		}
		void PrintClrMode::SetBYTECode(const unsigned char& src)
		{
			switch (src)
			{
			case 0: m_strValue = L"bw"; break;
			case 1: m_strValue = L"clr"; break;
			case 2: m_strValue = L"gray"; break;
			default:
				m_strValue = L"clr";
				break;
			}
		}
//------------------------------------------------------------------------------------------
		PrintWhat::PrintWhat()
		{
			m_strValue = L"slides";
		}
		void PrintWhat::set(const std::wstring& strValue)
		{
			if ((L"slides" == strValue) ||
				(L"handouts1" == strValue) ||
				(L"handouts2" == strValue) ||
				(L"handouts3" == strValue) ||
				(L"handouts4" == strValue) ||
				(L"handouts6" == strValue) ||
				(L"handouts9" == strValue) ||
				(L"notes" == strValue) ||
				(L"outline" == strValue))
			{
				m_strValue = strValue;
			}
		}
		unsigned char PrintWhat::GetBYTECode() const
		{
			if (L"slides" == m_strValue)		return 0;
			if (L"handouts1" == m_strValue)		return 1;
			if (L"handouts2" == m_strValue)		return 2;
			if (L"handouts3" == m_strValue)		return 3;
			if (L"handouts4" == m_strValue)		return 4;
			if (L"handouts6" == m_strValue)		return 5;
			if (L"handouts9" == m_strValue)		return 6;
			if (L"notes" == m_strValue)			return 7;
			if (L"outline" == m_strValue)		return 8;
			return 0;
		}
		void PrintWhat::SetBYTECode(const unsigned char& src)
		{
			switch (src)
			{
			case 0: m_strValue = L"slides"; break;
			case 1: m_strValue = L"handouts1"; break;
			case 2: m_strValue = L"handouts2"; break;
			case 3: m_strValue = L"handouts3"; break;
			case 4: m_strValue = L"handouts4"; break;
			case 5: m_strValue = L"handouts6"; break;
			case 6: m_strValue = L"handouts9"; break;
			case 7: m_strValue = L"notes"; break;
			case 8: m_strValue = L"outline"; break;
			default:
				m_strValue = L"slides";
				break;
			}
		}
	} // namespace Limit
} // namespace PPTX
