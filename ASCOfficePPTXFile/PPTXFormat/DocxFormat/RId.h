﻿/*
 * (c) Copyright Ascensio System SIA 2010-2016
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
 * You can contact Ascensio System SIA at Lubanas st. 125a-25, Riga, Latvia,
 * EU, LV-1021.
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
#pragma once
#ifndef PPTXOOX_RID_INCLUDE_H_
#define PPTXOOX_RID_INCLUDE_H_

#include "../../../Common/DocxFormat/Source/Base/Base.h"
#include "../../../Common/DocxFormat/Source/XML/xmlutils.h"

namespace PPTX
{
	class RId
	{
	public:
		RId() : m_id(_T("rId0"))
		{
		}
		RId(const size_t id)
		{
			m_id = _T("rId") + XmlUtils::UIntToString(id);
		}
		RId(const CString& rid)
		{
			m_id = rid;
		}
		RId(const RId& oSrc)
		{
			m_id = oSrc.m_id;
		}

	public:
		const RId& operator= (const size_t id)
		{
			m_id = _T("rId") + XmlUtils::UIntToString(id);
			return *this;
		}		
		const RId& operator= (const CString& rid)
		{
			m_id = rid;			
			return *this;
		}
#if defined(_WIN32) || defined (_WIN64)
		const RId& operator= (const BSTR& rid)
		{
			m_id = (CString)rid;	
			return *this;
		}
#endif
		const RId& operator= (const RId& oSrc)
		{
			m_id = oSrc.m_id;
			return *this;
		}

	public:
		const bool operator ==(const RId& lhs) const
		{
			return m_id == lhs.m_id;
		}
		const bool operator !=(const RId& lhs) const
		{
			return m_id != lhs.m_id;
		}
		const bool operator < (const RId& lhs) const
		{
			return m_id < lhs.m_id;
		}
		const bool operator <=(const RId& lhs) const
		{
			return m_id <= lhs.m_id;
		}
		const bool operator >(const RId& lhs) const
		{
			return m_id > lhs.m_id;
		}
		const bool operator >=(const RId& lhs) const
		{
			return m_id >= lhs.m_id;
		}

		AVSINLINE CString get() const { return m_id; }

	public:
		const RId	next() const
		{
			return RId(m_id + _T("1"));
		}
		
	public:
		const CString ToString() const
		{
			return m_id;
		}

		template<typename T>
		void toPPTY(BYTE type, T pWriter) const
		{
			pWriter->WriteBYTE(type);
			pWriter->WriteStringW(m_id);
		}

	private:
		CString m_id;
	};
} // namespace PPTX

#endif // PPTXOOX_RID_INCLUDE_H_
