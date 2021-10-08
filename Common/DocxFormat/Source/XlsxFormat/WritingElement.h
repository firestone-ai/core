﻿/*
 * (c) Copyright Ascensio System SIA 2010-2019
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
 * You can contact Ascensio System SIA at 20A-12 Ernesta Birznieka-Upisha
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

#include "../DocxFormat/WritingElement.h"
#include "../../../../DesktopEditor/common/StringBuilder.h"

namespace OOX
{
namespace Spreadsheet
{

#define WritingStringCommon(start, func, val, end) \
        writer.WriteString(start); \
        writer.func(val); \
        writer.WriteString(end);
//#define WritingStringCommonDouble(start, val, end) \
//		writer.WriteString(start); \
//		writer.AddDouble(val, -1); \
//		writer.WriteString(end);
#define WritingStringCommonDouble(start, val, end) \
        writer.WriteString(start); \
        writer.WriteString(XmlUtils::DoubleToString(val)); \
        writer.WriteString(end);

#define WritingValNode(ns, name, val) \
        writer.StartNodeWithNS(ns, name); \
        writer.StartAttributes(); \
        writer.WriteAttribute(L"val", val); \
        writer.EndAttributesAndNode();
#define WritingValNodeEncodeXml(ns, name, val) \
        writer.StartNodeWithNS(ns, name); \
        writer.StartAttributes(); \
        writer.WriteAttributeEncodeXml(L"val", val); \
        writer.EndAttributesAndNode();
#define WritingValNodeIf(ns, name, cond, val) \
        writer.StartNodeWithNS(ns, name); \
        writer.StartAttributes(); \
        if (cond) \
        { \
            writer.WriteAttribute(L"val", val); \
        } \
        writer.EndAttributesAndNode();

#define WritingStringValAttr(name, func, val) \
        WritingStringCommon(L"<" name L" val=\"", func, val, L"\"/>")
#define WritingStringValAttrString(name, val) \
        WritingStringValAttr(name, WriteString, val)
#define WritingStringValAttrEncodeXmlString(name, val) \
        WritingStringValAttr(name, WriteEncodeXmlString, val)
#define WritingStringValAttrInt(name, val) \
        WritingStringValAttr(name, AddInt, val)
#define WritingStringValAttrDouble(name, val) \
        WritingStringCommonDouble(L"<" name L" val=\"", val, L"\"/>")

#define WritingStringVal(name, func, val) \
        WritingStringCommon(L"<" name L">", func, val, L"</" name L">")
#define WritingStringValString(name, val) \
        WritingStringVal(name, WriteString, val)
#define WritingStringValEncodeXmlString(name, val) \
        WritingStringVal(name, WriteEncodeXmlString, val)
#define WritingStringValInt(name, val) \
        WritingStringVal(name, AddInt, val)
#define WritingStringValInt64(name, val) \
        WritingStringVal(name, AddInt64, val)
#define WritingStringValDouble(name, val) \
        WritingStringCommonDouble(L"<" name L">", val, L"</" name L">")

#define WritingStringAttr(name, func, val) \
        WritingStringCommon(L" " name L"=\"", func, val, L"\"")
#define WritingStringAttrString(name, val) \
        WritingStringAttr(name, WriteString, val)
#define WritingStringAttrEncodeXmlString(name, val) \
        WritingStringAttr(name, WriteEncodeXmlString, val)
#define WritingStringAttrInt(name, val) \
        WritingStringAttr(name, AddInt, val)
#define WritingStringAttrDouble(name, val) \
        WritingStringCommonDouble(L" " name L"=\"", val, L"\"")

#define WritingStringNullableAttr(name, func, elem, val) \
        if(elem.IsInit()) \
        { \
            WritingStringAttr(name, func, val) \
        }
#define WritingStringNullableAttrString(name, elem, val) \
        WritingStringNullableAttr(name, WriteString, elem, val)
#define WritingStringNullableAttrEncodeXmlString(name, elem, val) \
        WritingStringNullableAttr(name, WriteEncodeXmlString, elem, val)
#define WritingStringNullableAttrEncodeXmlStringHHHH(name, elem, val) \
        WritingStringNullableAttr(name, WriteEncodeXmlStringHHHH, elem, val)
#define WritingStringNullableAttrInt(name, elem, val) \
        WritingStringNullableAttr(name, AddInt, elem, val)
#define WritingStringNullableAttrUInt(name, elem, val) \
        WritingStringNullableAttr(name, AddUInt, elem, val)
#define WritingStringNullableAttrInt64(name, elem, val) \
        WritingStringNullableAttr(name, AddInt64, elem, val)
#define WritingStringNullableAttrDouble(name, elem, val) \
        if(elem.IsInit()) \
        { \
            WritingStringAttrDouble(name, val) \
        }
#define WritingStringNullableAttrBool(name, elem) \
        WritingStringNullableAttrString(name, elem, elem->ToString3(SimpleTypes::onofftostring1))

#define WritingStringNullableAttrBool2(name, elem) \
        WritingStringNullableAttrString(name, elem, *elem ? L"1": L"0")

#define WritingStringNullableAttrInt2(name, elem) \
        WritingStringNullableAttrInt(name, elem, *elem)

#define WritingStringNullableAttrDouble2(name, elem) \
        WritingStringNullableAttrDouble(name, elem, *elem)

#define WritingStringNullableAttrEncodeXmlString2(name, elem) \
        WritingStringNullableAttrEncodeXmlString(name, elem, *elem)

#define WritingNullable(elem, action) \
        if(elem.IsInit()) \
        { \
            action \
        }

    const double c_ag_Inch_to_MM	= 25.4;
    const double c_ag_1pxWidth		= 25.4 / 96;

    class WritingElement : public OOX::WritingElement
    {
    public:
        WritingElement(OOX::Document *pMain = NULL) : OOX::WritingElement(pMain) {}
        virtual EElementType    getType()   const
        {
            return OOX::et_x_Unknown;
        }
        virtual void toXML(NSStringUtils::CStringBuilder& writer) const = 0;
    };

    template<typename ElemType = WritingElement>
    class WritingElementWithChilds : public WritingElement
    {
    public:
        WritingElementWithChilds(OOX::Document *pMain = NULL) : WritingElement(pMain) {}

        virtual ~WritingElementWithChilds()
        {
            ClearItems();
        }
        virtual void ClearItems()
        {
            for ( size_t i = 0; i < m_arrItems.size(); ++i)
            {
                if ( m_arrItems[i] ) delete m_arrItems[i];
            }
            m_arrItems.clear();
        }
        std::vector<ElemType *>  m_arrItems;
    };
}
}
namespace XLSB
{
    typedef enum CF_RECORD_TYPE
    {
        rt_RowHdr =									0,
        rt_CellBlank =								1,
        rt_CellRk =									2,
        rt_CellError =								3,
        rt_CellBool =								4,
        rt_CellReal =								5,
        rt_CellSt =									6,
        rt_CellIsst =								7,
        rt_FmlaString =								8,
        rt_FmlaNum =								9,
        rt_FmlaBool =								10,
        rt_FmlaError =								11,
        rt_SSTItem =								19,
        rt_PcdiMissing =							20,
        rt_PcdiNumber =								21,
        rt_PcdiBoolean =							22,
        rt_PcdiError =								23,
        rt_PcdiString =								24,
        rt_PcdiDatetime =							25,
        rt_PcdiIndex =								26,
        rt_PcdiaMissing =							27,
        rt_PcdiaNumber =							28,
        rt_PcdiaBoolean =							29,
        rt_PcdiaError =								30,
        rt_PcdiaString =							31,
        rt_PcdiaDatetime =							32,
        rt_PcrRecord =								33,
        rt_PcrRecordDt =							34,
        rt_FRTBegin =								35,
        rt_FRTEnd =                                 36,
        rt_ACBegin =								37,
        rt_ACEnd =									38,
        rt_Name =									39,
        rt_IndexRowBlock =							40,
        rt_IndexBlock =								42,
        rt_Font =									43,
        rt_Fmt =									44,
        rt_Fill =									45,
        rt_Border =									46,
        rt_XF =										47,
        rt_Style =									48,
        rt_CellMeta =								49,
        rt_ValueMeta =								50,
        rt_Mdb =									51,
        rt_BeginFmd =								52,
        rt_EndFmd =									53,
        rt_BeginMdx =								54,
        rt_EndMdx =									55,
        rt_BeginMdxTuple =							56,
        rt_EndMdxTuple =							57,
        rt_MdxMbrIstr =								58,
        rt_Str =									59,
        rt_ColInfo =								60,
        rt_CellRString =							62,
        rt_DVal =									64,
        rt_SxvcellNum =								65,
        rt_SxvcellStr =								66,
        rt_SxvcellBool =							67,
        rt_SxvcellErr =								68,
        rt_SxvcellDate =							69,
        rt_SxvcellNil =								70,
        rt_FileVersion =							128,
        rt_BeginSheet =								129,
        rt_EndSheet =								130,
        rt_BeginBook =								131,
        rt_EndBook =								132,
        rt_BeginWsViews =							133,
        rt_EndWsViews =								134,
        rt_BeginBookViews =							135,
        rt_EndBookViews =							136,
        rt_BeginWsView =							137,
        rt_EndWsView =								138,
        rt_BeginCsViews =							139,
        rt_EndCsViews =								140,
        rt_BeginCsView =							141,
        rt_EndCsView =								142,
        rt_BeginBundleShs =							143,
        rt_EndBundleShs =							144,
        rt_BeginSheetData =							145,
        rt_EndSheetData =							146,
        rt_WsProp =									147,
        rt_WsDim =									148,
        rt_Pane =									151,
        rt_Sel =									152,
        rt_WbProp =									153,
        rt_WbFactoid =								154,
        rt_FileRecover =							155,
        rt_BundleSh =								156,
        rt_CalcProp =								157,
        rt_BookView =								158,
        rt_BeginSst =								159,
        rt_EndSst =									160,
        rt_BeginAFilter =							161,
        rt_EndAFilter =								162,
        rt_BeginFilterColumn =						163,
        rt_EndFilterColumn =						164,
        rt_BeginFilters =							165,
        rt_EndFilters =								166,
        rt_Filter =									167,
        rt_ColorFilter =							168,
        rt_IconFilter =								169,
        rt_Top10Filter =							170,
        rt_DynamicFilter =							171,
        rt_BeginCustomFilters =						172,
        rt_EndCustomFilters =						173,
        rt_CustomFilter =							174,
        rt_AFilterDateGroupItem =					175,
        rt_MergeCell =								176,
        rt_BeginMergeCells =						177,
        rt_EndMergeCells =							178,
        rt_BeginPivotCacheDef =						179,
        rt_EndPivotCacheDef =						180,
        rt_BeginPcdFields =							181,
        rt_EndPcdFields =							182,
        rt_BeginPcdField =							183,
        rt_EndPcdField =							184,
        rt_BeginPcdSource =							185,
        rt_EndPcdSource =							186,
        rt_BeginPcdsRange =							187,
        rt_EndPcdsRange =							188,
        rt_BeginPcdfAtbl =							189,
        rt_EndPcdfAtbl =							190,
        rt_BeginPcdiRun =							191,
        rt_EndPcdiRun =								192,
        rt_EndPivotCacheRecords =					194,
        rt_BeginPcdHierarchies =					195,
        rt_EndPcdHierarchies =						196,
        rt_BeginPcdHierarchy =						197,
        rt_EndPcdHierarchy =						198,
        rt_BeginPcdhFieldsUsage =					199,
        rt_EndPcdhFieldsUsage =						200,
        rt_BeginExtConnection =						201,
        rt_EndExtConnection =						202,
        rt_BeginEcDbProps =							203,
        rt_EndEcDbProps =							204,
        rt_BeginEcOlapProps =						205,
        rt_EndEcOlapProps =							206,
        rt_BeginPcdsConsol =						207,
        rt_EndPcdsConsol =							208,
        rt_BeginPcdscPages =						209,
        rt_EndPcdscPages =							210,
        rt_BeginPcdscPage =							211,
        rt_EndPcdscPage =							212,
        rt_BeginPcdscpItem =						213,
        rt_EndPcdscpItem =							214,
        rt_BeginPcdscSets =							215,
        rt_EndPcdscSets =							216,
        rt_BeginPcdscSet =							217,
        rt_EndPcdscSet =							218,
        rt_BeginPcdfGroup =							219,
        rt_EndPcdfGroup =							220,
        rt_BeginPcdfgItems =						221,
        rt_EndPcdfgItems =							222,
        rt_BeginPcdfgRange =						223,
        rt_EndPcdfgRange =							224,
        rt_BeginPcdfgDiscrete =						225,
        rt_EndPcdfgDiscrete =						226,
        rt_EndPcdsdTupleCache =						228,
        rt_BeginPcdsdtcEntries =					229,
        rt_EndPcdsdtcEntries =						230,
        rt_EndPcdsdtceMember =						234,
        rt_BeginPcdsdtcQueries =					235,
        rt_EndPcdsdtcQueries =						236,
        rt_BeginPcdsdtcQuery =						237,
        rt_EndPcdsdtcQuery =						238,
        rt_BeginPcdsdtcSets =						239,
        rt_EndPcdsdtcSets =							240,
        rt_BeginPcdsdtcSet =						241,
        rt_EndPcdsdtcSet =							242,
        rt_BeginPcdCalcItems =						243,
        rt_EndPcdCalcItems =						244,
        rt_BeginPcdCalcItem =						245,
        rt_EndPcdCalcItem =							246,
        rt_BeginPRule =								247,
        rt_EndPRule =								248,
        rt_BeginPrFilters =							249,
        rt_EndPrFilters =							250,
        rt_BeginPrFilter =							251,
        rt_EndPrFilter =							252,
        rt_BeginPNames =							253,
        rt_EndPNames =								254,
        rt_BeginPName =								255,
        rt_EndPName =								256,
        rt_BeginPnPairs =							257,
        rt_EndPnPairs =								258,
        rt_BeginPnPair =							259,
        rt_EndPnPair =								260,
        rt_BeginEcWebProps =						261,
        rt_EndEcWebProps =							262,
        rt_BeginEcWpTables =						263,
        rt_EndEcwpTables =							264,
        rt_BeginEcParams =							265,
        rt_EndEcParams =							266,
        rt_BeginEcParam =							267,
        rt_EndEcParam =								268,
        rt_BeginPcdkpis =							269,
        rt_EndPcdkpis =								270,
        rt_BeginPcdkpi =							271,
        rt_EndPcdkpi =								272,
        rt_BeginDims =								273,
        rt_EndDims =								274,
        rt_BeginDim =								275,
        rt_EndDim =									276,
        rt_IndexPart_End =							277,
        rt_BeginStyleSheet =						278,
        rt_EndStyleSheet =							279,
        rt_BeginSxView =							280,
        rt_EndSxvi =								281,
        rt_BeginSxvi =								282,
        rt_BeginSxvis =								283,
        rt_EndSxvis =								284,
        rt_BeginSxvd =								285,
        rt_EndSxvd =								286,
        rt_BeginSxvds =								287,
        rt_EndSxvds =								288,
        rt_BeginSxpi =								289,
        rt_EndSxpi =								290,
        rt_BeginSxpis =								291,
        rt_EndSxpis =								292,
        rt_BeginSxdi =								293,
        rt_EndSxdi =								294,
        rt_BeginSxdis =								295,
        rt_EndSxdis =								296,
        rt_BeginSxli =								297,
        rt_EndSxli =								298,
        rt_BeginSxliRws =							299,
        rt_EndSxliRws =								300,
        rt_BeginSxliCols =							301,
        rt_EndSxliCols =							302,
        rt_BeginSxFormat =							303,
        rt_EndSxFormat =							304,
        rt_BeginSxFormats =							305,
        rt_EndSxFormats =							306,
        rt_BeginSxSelect =							307,
        rt_EndSxSelect =							308,
        rt_BeginIsxvdRws =							309,
        rt_EndIsxvdRws =							310,
        rt_BeginIsxvdCols =							311,
        rt_EndIsxvdCols =							312,
        rt_EndSxLocation =							313,
        rt_BeginSxLocation =						314,
        rt_EndSxView =								315,
        rt_BeginSxths =								316,
        rt_EndSxths =								317,
        rt_BeginSxth =								318,
        rt_EndSxth =								319,
        rt_BeginIsxthRws =							320,
        rt_EndIsxthRws =							321,
        rt_BeginIsxthCols =							322,
        rt_EndIsxthCols =							323,
        rt_BeginSxtdmps =							324,
        rt_EndSxtdmps =								325,
        rt_BeginSxtdmp =							326,
        rt_EndSxtdmp =								327,
        rt_BeginSxthItems =							328,
        rt_EndSxthItems =							329,
        rt_BeginSxthItem =							330,
        rt_EndSxthItem =							331,
        rt_BeginMetadata =							332,
        rt_EndMetadata =							333,
        rt_BeginEsmdtinfo =							334,
        rt_Mdtinfo =								335,
        rt_EndEsmdtinfo =							336,
        rt_BeginEsmdb =								337,
        rt_EndEsmdb =								338,
        rt_BeginEsfmd =								339,
        rt_EndEsfmd =								340,
        rt_BeginSingleCells =						341,
        rt_EndSingleCells =							342,
        rt_BeginList =								343,
        rt_EndList =								344,
        rt_BeginListCols =							345,
        rt_EndListCols =							346,
        rt_BeginListCol =							347,
        rt_EndListCol =								348,
        rt_BeginListXmlCPr =						349,
        rt_EndListXmlCPr =							350,
        rt_ListCcFmla =								351,
        rt_ListTrFmla =								352,
        rt_BeginExternals =							353,
        rt_EndExternals =							354,
        rt_SupBookSrc =								355,
        rt_SupSelf =								357,
        rt_SupSame =								358,
        rt_SupTabs =								359,
        rt_BeginSupBook =							360,
        rt_PlaceholderName =						361,
        rt_ExternSheet =							362,
        rt_ExternTableStart_ =						363,
        rt_ExternTableEnd =							364,
        rt_ExternRowHdr =							366,
        rt_ExternCellBlank =						367,
        rt_ExternCellReal =							368,
        rt_ExternCellBool =							369,
        rt_ExternCellError =						370,
        rt_ExternCellString =						371,
        rt_BeginEsmdx =								372,
        rt_EndEsmdx =								373,
        rt_BeginMdxSet =							374,
        rt_EndMdxSet =								375,
        rt_BeginMdxMbrProp =						376,
        rt_EndMdxMbrProp =							377,
        rt_BeginMdxKpi =							378,
        rt_EndMdxKpi =								379,
        rt_BeginEsstr =								380,
        rt_EndEsstr =								381,
        rt_BeginPrfItem =							382,
        rt_EndPrfItem =								383,
        rt_BeginPivotCacheIds =						384,
        rt_EndPivotCacheIds =						385,
        rt_BeginPivotCacheId =						386,
        rt_EndPivotCacheId =						387,
        rt_BeginIsxvis =							388,
        rt_EndIsxvis =								389,
        rt_BeginColInfos =							390,
        rt_EndColInfos =							391,
        rt_BeginRwBrk =								392,
        rt_EndRwBrk =								393,
        rt_BeginColBrk =							394,
        rt_EndColBrk =								395,
        rt_Brk =									396,
        rt_UserBookView =							397,
        rt_Info =									398,
        rt_CUsr =									399,
        rt_Usr =									400,
        rt_BeginUsers =								401,
        rt_Eof =									403,
        rt_Ucr =									404,
        rt_RrInsDel =								405,
        rt_RrEndInsDel =							406,
        rt_RrMove =									407,
        rt_RrEndMove =								408,
        rt_RrChgCell =								409,
        rt_RrEndChgCell =							410,
        rt_RrHeader =								411,
        rt_RrUserView =								412,
        rt_RrRenSheet =								413,
        rt_RrInsert_Sh =							414,
        rt_RrDefName =								415,
        rt_RrNote =									416,
        rt_RrConflict =								417,
        rt_Rrt_qsif =								418,
        rt_RrFormat =								419,
        rt_RrEndFormat =							420,
        rt_RrAutoFmt =								421,
        rt_BeginUserShViews =						422,
        rt_BeginUserShView =						423,
        rt_EndUserShView =							424,
        rt_EndUserShViews =							425,
        rt_ArrFmla =								426,
        rt_ShrFmla =								427,
        rt_Table =									428,
        rt_BeginExtConnections =					429,
        rt_EndExtConnections =						430,
        rt_BeginPcdCalcMems =						431,
        rt_EndPcdCalcMems =							432,
        rt_BeginPcdCalcMem =						433,
        rt_EndPcdCalcMem =							434,
        rt_BeginPcdhgLevels =						435,
        rt_EndPcdhgLevels =							436,
        rt_BeginPcdhgLevel =						437,
        rt_EndPcdhgLevel =							438,
        rt_BeginPcdhglGroups =						439,
        rt_EndPcdhglGroups =						440,
        rt_BeginPcdhglGroup =						441,
        rt_EndPcdhglGroup =							442,
        rt_EndPcdhglgMembers =						444,
        rt_BeginPcdhglgMember =						445,
        rt_EndPcdhglgMember =						446,
        rt_BeginQsi =								447,
        rt_EndQsi =									448,
        rt_BeginQsir =								449,
        rt_EndQsir =								450,
        rt_BeginDeletedNames =						451,
        rt_EndDeletedNames =						452,
        rt_BeginDeletedName =						453,
        rt_EndDeletedName =							454,
        rt_BeginQsifs =								455,
        rt_EndQsifs =								456,
        rt_BeginQsif =								457,
        rt_EndQsif =								458,
        rt_BeginAutoSort_Scope =					459,
        rt_EndAutoSort_Scope =						460,
        rt_BeginConditionalFormatting =             461,
        rt_EndConditionalFormatting =               462,
        rt_BeginCFRule =							463,
        rt_EndCFRule =								464,
        rt_BeginIconSet =							465,
        rt_EndIconSet =								466,
        rt_BeginDatabar =							467,
        rt_EndDatabar =								468,
        rt_BeginColorScale =						469,
        rt_EndColorScale =							470,
        rt_CFVO =									471,
        rt_ExternValueMeta =						472,
        rt_BeginColorPalette =						473,
        rt_EndColorPalette =						474,
        rt_IndexedColor =							475,
        rt_Margins =								476,
        rt_PrintOptions =							477,
        rt_PageSetup =								478,
        rt_BeginHeaderFooter =						479,
        rt_EndHeaderFooter =						480,
        rt_BeginSxCrt_Format =						481,
        rt_EndSxCrt_Format =						482,
        rt_BeginSxCrt_Formats =						483,
        rt_EndSxCrt_Formats =						484,
        rt_WsFmtInfo =								485,
        rt_BeginMgs =								486,
        rt_EndMgs =									487,
        rt_BeginMgMaps =							488,
        rt_EndMgMaps =								489,
        rt_BeginMg =								490,
        rt_EndMg =									491,
        rt_BeginMap =								492,
        rt_EndMap =									493,
        rt_HLink =									494,
        rt_BeginDCon =								495,
        rt_EndDCon =								496,
        rt_BeginDRefs =								497,
        rt_EndDRefs =								498,
        rt_DRef =									499,
        rt_BeginScenMan =							500,
        rt_EndScenMan =								501,
        rt_BeginSct =								502,
        rt_EndSct =									503,
        rt_Slc =									504,
        rt_BeginDxfs =								505,
        rt_EndDxfs =								506,
        rt_Dxf =									507,
        rt_BeginTableStyles =						508,
        rt_EndTableStyles =							509,
        rt_BeginTableStyle =						510,
        rt_EndTableStyle =							511,
        rt_TableStyleElement =						512,
        rt_TableStyleClient =						513,
        rt_BeginVolDeps =							514,
        rt_EndVolDeps =								515,
        rt_BeginVolType =							516,
        rt_EndVolType =								517,
        rt_BeginVolMain =							518,
        rt_EndVolMain =								519,
        rt_BeginVolTopic =							520,
        rt_EndVolTopic =							521,
        rt_VolSubtopic =							522,
        rt_VolRef =									523,
        rt_VolNum =									524,
        rt_VolErr =									525,
        rt_VolStr =									526,
        rt_VolBool =								527,
        rt_BeginSortState =						530,
        rt_EndSortState =							531,
        rt_BeginSortCond =							532,
        rt_EndSortCond =							533,
        rt_BookProtection =							534,
        rt_SheetProtection =						535,
        rt_RangeProtection =						536,
        rt_PhoneticInfo =							537,
        rt_BeginEcTxtWiz =							538,
        rt_EndEcTxtWiz =							539,
        rt_BeginEctwFldInfoLst =					540,
        rt_EndEctwFldInfoLst =						541,
        rt_BeginEcTwFldInfo =						542,
        rt_FileSharing =							548,
        rt_OleSize =								549,
        rt_Drawing =								550,
        rt_LegacyDrawing =							551,
        rt_LegacyDrawingHf =						552,
        rt_WebOpt =									553,
        rt_BeginWebPubItems =						554,
        rt_EndWebPubItems =							555,
        rt_BeginWebPubItem =						556,
        rt_EndWebPubItem =							557,
        rt_BeginSxCondFmt =							558,
        rt_EndSxCondFmt =							559,
        rt_BeginSxCondFmts =						560,
        rt_EndSxCondFmts =							561,
        rt_BkHim =									562,
        rt_Color =									564,
        rt_BeginIndexedColors =						565,
        rt_EndIndexedColors =						566,
        rt_BeginMruColors =							569,
        rt_EndMruColors =							570,
        rt_MruColor =								572,
        rt_BeginDVals =								573,
        rt_EndDVals =								574,
        rt_SupNameStart_ =							577,
        rt_SupNameValueStart_ =						578,
        rt_SupNameValueEnd =						579,
        rt_SupNameNum =								580,
        rt_SupNameErr =								581,
        rt_SupNameSt =								582,
        rt_SupNameNil =								583,
        rt_SupNameBool =							584,
        rt_SupNameFmla =							585,
        rt_SupNameBits =							586,
        rt_SupNameEnd =								587,
        rt_EndSupBook =								588,
        rt_CellSmart_TagPropert_y =					589,
        rt_BeginCellSmart_Tag =						590,
        rt_EndCellSmart_Tag =						591,
        rt_BeginCellSmart_Tags =					592,
        rt_EndCellSmart_Tags =						593,
        rt_BeginSmart_Tags =						594,
        rt_EndSmart_Tags =							595,
        rt_Smart_TagType =							596,
        rt_BeginSmart_TagTypes =					597,
        rt_EndSmart_TagTypes =						598,
        rt_BeginSxFilters =							599,
        rt_EndSxFilters =							600,
        rt_BeginSxfilter =							601,
        rt_EndSxFilter =							602,
        rt_BeginFills =								603,
        rt_EndFills =								604,
        rt_BeginCellWatches =						605,
        rt_EndCellWatches =							606,
        rt_CellWatch =								607,
        rt_BeginCrErrs =							608,
        rt_EndCrErrs =								609,
        rt_CrashRecErr =							610,
        rt_BeginFonts =								611,
        rt_EndFonts =								612,
        rt_BeginBorders =							613,
        rt_EndBorders =								614,
        rt_BeginFmts =								615,
        rt_EndFmts =								616,
        rt_BeginCellXFs =							617,
        rt_EndCellXFs =								618,
        rt_BeginStyles =							619,
        rt_EndStyles =								620,
        rt_BigName =								625,
        rt_BeginCellStyleXFs =						626,
        rt_EndCellStyleXFs =						627,
        rt_BeginComments =							628,
        rt_EndComments =							629,
        rt_BeginCommentAuthors =					630,
        rt_EndCommentAuthors =						631,
        rt_CommentAuthor =							632,
        rt_BeginCommentList =						633,
        rt_EndCommentList =							634,
        rt_BeginComment =							635,
        rt_EndComment =								636,
        rt_CommentText =							637,
        rt_BeginOleObjects =						638,
        rt_OleObject =								639,
        rt_EndOleObjects =							640,
        rt_BeginSxrules =							641,
        rt_EndSxRules =								642,
        rt_BeginActiveXControls =					643,
        rt_ActiveX =								644,
        rt_EndActiveXControls =						645,
        rt_BeginPcdsdtceMembersSort_By =			646,
        rt_BeginCellIgnoreEcs =						648,
        rt_CellIgnoreEc =							649,
        rt_EndCellIgnoreEcs =						650,
        rt_CsProp =									651,
        rt_CsPageSetup =							652,
        rt_BeginUserCsViews =						653,
        rt_EndUserCsViews =							654,
        rt_BeginUserCsView =						655,
        rt_EndUserCsView =							656,
        rt_BeginPcdSfciEntries =					657,
        rt_EndPcdsfciEntries =						658,
        rt_PcdsfciEntry =							659,
        rt_BeginListParts =                         660,
        rt_ListPart =								661,
        rt_EndListParts =							662,
        rt_SheetCalcProp =							663,
        rt_BeginFnGroup =							664,
        rt_FnGroup =								665,
        rt_EndFnGroup =								666,
        rt_SupAddin =								667,
        rt_SxtdmpOrder =							668,
        rt_CsProtection =							669,
        rt_BeginWsSortMap =						671,
        rt_EndWsSortMap =							672,
        rt_BeginRrSort =							673,
        rt_EndRrSort =								674,
        rt_RrSortItem =							675,
        rt_FileSharingIso =							676,
        rt_BookProtectionIso =						677,
        rt_SheetProtectionIso =						678,
        rt_CsProtectionIso =						679,
        rt_RangeProtectionIso =						680,
        rt_RwDescent =								1024,
        rt_KnownFonts =								1025,
        rt_BeginSxTupleSet =						1026,
        rt_EndSxTupleSet =							1027,
        rt_EndSxTupleSetHeader =					1029,
        rt_BeginSxTupleSetData =					1031,
        rt_EndSxTupleSetData =						1032,
        rt_BeginSxTupleSetRow =						1033,
        rt_EndSxTupleSetRow =						1034,
        rt_SxTupleSetRowItem =						1035,
        rt_NameExt =								1036,
        rt_Pcdh14 =									1037,
        rt_BeginPcdCalcMem14 =						1038,
        rt_EndPcdCalcMem14 =						1039,
        rt_Sxth14 =									1040,
        rt_BeginSparklineGroup =					1041,
        rt_EndSparklineGroup =						1042,
        rt_Sparkline =								1043,
        rt_Sxdi14 =									1044,
        rt_WsFmtInfoEx14 =							1045,
        rt_BeginCfRule14 =							1048,
        rt_EndCfRule14 =							1049,
        rt_CFVO14 =									1050,
        rt_BeginDatabar14 =							1051,
        rt_BeginIconSet14 =							1052,
        rt_DVal14 =									1053,
        rt_BeginDVals14 =							1054,
        rt_Color14 =								1055,
        rt_BeginSparklines =						1056,
        rt_EndSparklines =							1057,
        rt_BeginSparklineGroups =					1058,
        rt_EndSparklineGroups =						1059,
        rt_Sxvd14 =									1061,
        rt_BeginSxView14 =							1062,
        rt_EndSxView14 =							1063,
        rt_BeginSxView16 =							1064,
        rt_EndSxView16 =							1065,
        rt_BeginPcd14 =								1066,
        rt_EndPcd14 =								1067,
        rt_BeginExtConn14 =							1068,
        rt_EndExtConn14 =							1069,
        rt_BeginSlicerCacheIds =					1070,
        rt_EndSlicerCacheIds =						1071,
        rt_BeginSlicerCacheId =						1072,
        rt_EndSlicerCacheId =						1073,
        rt_BeginSlicerCache =						1075,
        rt_EndSlicerCache =							1076,
        rt_BeginSlicerCacheDef =					1077,
        rt_EndSlicerCacheDef =						1078,
        rt_BeginSlicersEx =							1079,
        rt_EndSlicersEx =							1080,
        rt_BeginSlicerEx =							1081,
        rt_EndSlicerEx =							1082,
        rt_BeginSlicer =							1083,
        rt_EndSlicer =								1084,
        rt_SlicerCachePivotTables =					1085,
        rt_SlicerCacheOlapItem =					1096,
        rt_SlicerCacheSelection =					1098,
        rt_EndSlicerCacheNative =					1101,
        rt_SlicerCacheNativeItem =					1102,
        rt_RangeProtection14 =						1103,
        rt_RangeProtectionIso14 =					1104,
        rt_CellIgnoreEc14 =							1105,
        rt_List14 =									1111,
        rt_CfIcon =									1112,
        rt_BeginSlicerCachesPivotCacheIds =			1113,
        rt_BeginSlicers =							1115,
        rt_EndSlicers =								1116,
        rt_WbProp14 =								1117,
        rt_BeginSxEdit =							1118,
        rt_EndSxEdit =								1119,
        rt_BeginSxEdits =							1120,
        rt_EndSxEdits =								1121,
        rt_BeginSxChange =							1122,
        rt_EndSxChange =							1123,
        rt_BeginSxChanges =							1124,
        rt_EndSxChanges =							1125,
        rt_SxTupleItems =							1126,
        rt_BeginSlicerStyle =						1128,
        rt_EndSlicerStyle =							1129,
        rt_SlicerStyleElement =						1130,
        rt_BeginStyleSheetExt14 =					1131,
        rt_EndStyleSheetExt14 =						1132,
        rt_BeginSlicerCachesPivotCacheId =			1133,
        rt_BeginConditionalFormattings =			1135,
        rt_EndConditionalFormattings =              1136,
        rt_BeginPcdCalcMemExt =						1137,
        rt_EndPcdCalcMemExt =						1138,
        rt_BeginPcdCalcMemsExt =					1139,
        rt_EndPcdCalcMemsExt =						1140,
        rt_PcdField14 =								1141,
        rt_BeginSlicerStyles =						1142,
        rt_EndSlicerStyles =						1143,
        rt_CFRuleExt =								1146,
        rt_BeginSxCondFmt14 =						1147,
        rt_EndSxCondFmt14 =							1148,
        rt_BeginSxCondFmts14 =						1149,
        rt_EndSxCondFmts14 =						1150,
        rt_BeginSortCond14 =						1152,
        rt_EndSortCond14 =							1153,
        rt_EndDVals14 =								1154,
        rt_EndIconSet14 =							1155,
        rt_EndDatabar14 =							1156,
        rt_BeginColorScale14 =						1157,
        rt_EndColorScale14 =						1158,
        rt_BeginSxrules14 =							1159,
        rt_EndSxrules14 =							1160,
        rt_BeginPRule14 =							1161,
        rt_EndPRule14 =								1162,
        rt_BeginPrFilters14 =						1163,
        rt_EndPrFilters14 =							1164,
        rt_BeginPrFilter14 =						1165,
        rt_EndPrFilter14 =							1166,
        rt_BeginPrfItem14 =							1167,
        rt_EndPrfItem14 =							1168,
        rt_BeginCellIgnoreEcs14 =					1169,
        rt_EndCellIgnoreEcs14 =						1170,
        rt_Dxf14 =									1171,
        rt_BeginDxF14s =							1172,
        rt_EndDxf14s =								1173,
        rt_Filter14 =								1177,
        rt_BeginCustomFilters14 =					1178,
        rt_CustomFilter14 =							1180,
        rt_IconFilter14 =							1181,
        rt_BeginPivotTableRefs =					2051,
        rt_EndPivotTableRefs =						2052,
        rt_PivotTableRef =							2053,
        rt_BeginSxvcells =							2055,
        rt_EndSxvcells =							2056,
        rt_BeginSxRow =								2057,
        rt_EndSxRow =								2058,
        rt_PcdCalcMem15 =							2060,
        rt_Qsi15 =									2067,
        rt_BeginWebExtensions =						2068,
        rt_EndWebExtensions =						2069,
        rt_WebExtension =							2070,
        rt_AbsPath15 =								2071,
        rt_TableSlicerCacheIds =					2075,
        rt_TableSlicerCacheId =						2076,
        rt_BeginTableSlicerCache =					2077,
        rt_EndTableSlicerCache =					2078,
        rt_SxFilter15 =								2079,
        rt_BeginTimelineCachePivotCacheIds =		2080,
        rt_EndTimelineCachePivotCacheIds =			2081,
        rt_EndTimelineCacheIds =					2084,
        rt_BeginTimelineCacheId =					2085,
        rt_EndTimelineCacheId =						2086,
        rt_BeginTimelinesEx =						2087,
        rt_EndTimelinesEx =							2088,
        rt_BeginTimelineEx =						2089,
        rt_EndTimelineEx =							2090,
        rt_WorkBookPr15 =							2091,
        rt_Pcdh15 =									2092,
        rt_BeginTimelineStyle =						2093,
        rt_EndTimelineStyle =						2094,
        rt_TimelineStyleElement =					2095,
        rt_BeginTimelineStyles =					2098,
        rt_EndTimelineStyles =						2099,
        rt_Dxf15 =									2102,
        rt_BeginDxfs15 =							2103,
        rt_EndDxfs15 =								2104,
        rt_SlicerCacheHideItemsWithNoData =			2105,
        rt_BeginItemUniqueNames =					2106,
        rt_EndItemUniqueNames =						2107,
        rt_ItemUniqueName =							2108,
        rt_BeginExtConn15 =							2109,
        rt_EndExtConn15 =							2110,
        rt_BeginOledbPr15 =							2111,
        rt_EndOledbPr15 =							2112,
        rt_BeginDataFeedPr15 =						2113,
        rt_EndDataFeedPr15 =						2114,
        rt_TextPr15 =								2115,
        rt_RangePr15 =								2116,
        rt_DbCommand15 =							2117,
        rt_BeginDbTables15 =						2118,
        rt_EndDbTables15 =							2119,
        rt_DbTable15 =								2120,
        rt_BeginDataModel =							2121,
        rt_EndDataModel =							2122,
        rt_BeginModelTables =						2123,
        rt_EndModelTables =							2124,
        rt_ModelTable =								2125,
        rt_EndModelRelationships =					2127,
        rt_ModelRelationship =						2128,
        rt_BeginEcTxtWiz15 =						2129,
        rt_EndEcTxtWiz15 =							2130,
        rt_BeginEctwFldInfoLst15 =					2131,
        rt_EndEctwFldInfoLst15 =					2132,
        rt_BeginEctwFldInfo15 =						2133,
        rt_FieldListActiveItem =					2134,
        rt_PivotCacheIdVersion =					2135,
        rt_Sxdi15 =									2136,
        rt_Uid =                                    3072,
        rt_RevisionPtr =							3073,
        rt_BeginRichSortCondition =							5092,
        rt_EndRichSortCondition =							5093,
        rt_Unknown =								0xffff
    } RecordType;
}
