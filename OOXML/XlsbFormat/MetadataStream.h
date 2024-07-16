/*
 * (c) Copyright Ascensio System SIA 2010-2024
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

#pragma once

#include "../../DesktopEditor/common/Types.h"
#include "../Base/Base.h"
#include "../XlsxFormat/WritingElement.h"
#include <string>
#include <memory.h>
#include <iostream>
#include "../../MsBinaryFile/XlsFile/Format/Logic/CompositeObject.h"

namespace XLSB
{
    class MetadataStream;
    typedef boost::shared_ptr<MetadataStream>		MetadataStreamPtr;

    class MetadataStream: public XLS::CompositeObject
    {
        BASE_OBJECT_DEFINE_CLASS_NAME(MetadataStream)
    public:
        MetadataStream();
        ~MetadataStream();

        XLS::BaseObjectPtr clone();

        const bool loadContent(XLS::BinProcessor& proc) override;
        const bool saveContent(XLS::BinProcessor& proc) override;

        bool                            m_bBeginMetadata;
		XLS::BaseObjectPtr              m_ESMDTINFO;
		XLS::BaseObjectPtr              m_ESSTR;
        XLS::BaseObjectPtr              m_ESMDX;
        XLS::BaseObjectPtr              m_ESFMD;
        XLS::BaseObjectPtr              m_CellMetadataBlocks;
        XLS::BaseObjectPtr              m_ValueMetadataBlocks;
        bool                            m_bEndMetadata;

    };
}
