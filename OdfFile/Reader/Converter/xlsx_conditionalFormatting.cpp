﻿/*
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

#include "xlsx_conditionalFormatting.h"
#include "xlsx_utils.h"

#include <vector>
#include <boost/lexical_cast.hpp>
#include "../../Common/xml/simple_xml_writer.h"
#include "../../../OOXML/Base/Unit.h"

#include "../../Formulas/formulasconvert.h"

namespace cpdoccore {
namespace oox {

	struct _cfvo
	{
		int						type;
		_CP_OPT(bool)			gte;
		_CP_OPT(std::wstring)	val;
		
		void serialize(std::wostream & _Wostream)
		{
			CP_XML_WRITER(_Wostream)
			{
				CP_XML_NODE(L"cfvo")
				{
					switch(type)
					{
						case 0: CP_XML_ATTR(L"type", L"percent");	break;
						case 1: CP_XML_ATTR(L"type", L"num");		break;
 						case 2: CP_XML_ATTR(L"type", L"max");		break;
  						case 3: CP_XML_ATTR(L"type", L"min");		break;
 						case 4: CP_XML_ATTR(L"type", L"max");		break; //todooo ext - autoMax
 						case 5: CP_XML_ATTR(L"type", L"min");		break;
 						case 6: CP_XML_ATTR(L"type", L"formula");	break;	
						case 7: CP_XML_ATTR(L"type", L"percentile");break;//BOA PARA ESTUDAR - JOGAR LOTOFACIL minha predileta 1.ods
							
					}
					if (val)
					{
						CP_XML_ATTR(L"val", *val);
					}
				}
			}
		}
		void serializeEx(std::wostream& _Wostream)
		{
			CP_XML_WRITER(_Wostream)
			{
				CP_XML_NODE(L"x14:cfvo")
				{
					switch (type)
					{
					case 0: CP_XML_ATTR(L"type", L"percent");	break;
					case 1: CP_XML_ATTR(L"type", L"num");		break;
					case 2: CP_XML_ATTR(L"type", L"max");		break;
					case 3: CP_XML_ATTR(L"type", L"min");		break;
					case 4: CP_XML_ATTR(L"type", L"autoMax");	break;
					case 5: CP_XML_ATTR(L"type", L"autoMin");	break;
					case 6: CP_XML_ATTR(L"type", L"formula");	break;
					case 7: CP_XML_ATTR(L"type", L"percentile"); break;//BOA PARA ESTUDAR - JOGAR LOTOFACIL minha predileta 1.ods

					}
					if (val)
					{
						CP_XML_NODE(L"xm:f")
						{
							CP_XML_CONTENT(*val);
						}
					}
				}
			}
		}
	};

	struct rule
    {
		int							type;
		_CP_OPT(int)				dxfId;
		_CP_OPT(bool)				percent;
		_CP_OPT(bool)				stopIfTrue;
		_CP_OPT(std::wstring)		operator_;
//expr		
		_CP_OPT(std::wstring)		formula;
		_CP_OPT(std::wstring)		formula_type;
		_CP_OPT(std::wstring)		text;
		_CP_OPT(std::wstring)		formula2;
		_CP_OPT(int)				rank;
		_CP_OPT(bool)				bottom;
		_CP_OPT(bool)				equal;
		_CP_OPT(bool)				above;
		_CP_OPT(int)				stdDev;
//color scale icon set data_bar
		std::vector<_cfvo>			cfvo;
//color scale data_bar(1 element)
		std::vector<std::wstring>	color;
//data bar	icon_set	
		_CP_OPT(bool)				showValue;
//data bar
		_CP_OPT(unsigned int)		minLength;
		_CP_OPT(unsigned int)		maxLength;
		_CP_OPT(std::wstring)		axis_position;
		_CP_OPT(std::wstring)		axis_color;
		_CP_OPT(std::wstring)		negative_color;
		_CP_OPT(bool)				gradient;
//icon set
		_CP_OPT(int)				icon_set_type;
		_CP_OPT(bool)				reverse;
		_CP_OPT(bool)				iconset_percent;
		_CP_OPT(int)				iconset_type;
//date is
		_CP_OPT(int)				time_period;
		
		bool isExtended()
		{
			if (gradient || axis_color || axis_position || negative_color)
			{
				return true;
			}
			return false;
		}
	};
	struct conditionalFormatting
    {
		bool bUsed = false;
		std::wstring		ref;
		std::vector<rule>	rules;
    };

class xlsx_conditionalFormatting_context::Impl
{
public:

    void serialize(std::wostream & _Wostream)
    {
		if (conditionalFormattings_.empty()) return;

		int priority = 1;
		CP_XML_WRITER(_Wostream)
		{
			for (size_t i = 0; i < conditionalFormattings_.size(); i++)
			{
				conditionalFormatting& c = conditionalFormattings_[i];

				if (c.bUsed) continue;
				if (c.rules.size() < 1) continue;

				CP_XML_NODE(L"conditionalFormatting")
				{
					CP_XML_ATTR(L"sqref", c.ref);

					for (size_t j = 0; j < c.rules.size(); j++)
					{
						if (c.rules[j].type < 1 || c.rules[j].type > 5) continue;

						CP_XML_NODE(L"cfRule")
						{
							CP_XML_ATTR(L"priority", priority++);

							if (c.rules[j].dxfId)		CP_XML_ATTR(L"dxfId", *c.rules[j].dxfId);
							if (c.rules[j].percent)		CP_XML_ATTR(L"percent", *c.rules[j].percent);
							if (c.rules[j].operator_)	CP_XML_ATTR(L"operator", *c.rules[j].operator_);
							if (c.rules[j].stopIfTrue)	CP_XML_ATTR(L"stopIfTrue", *c.rules[j].stopIfTrue);
							if (c.rules[j].text)		CP_XML_ATTR(L"text", *c.rules[j].text);
							if (c.rules[j].rank)		CP_XML_ATTR(L"rank", *c.rules[j].rank);
							if (c.rules[j].bottom)		CP_XML_ATTR(L"bottom", *c.rules[j].bottom);
							if (c.rules[j].above)		CP_XML_ATTR(L"aboveAverage", *c.rules[j].above);
							if (c.rules[j].equal)		CP_XML_ATTR(L"equalAverage", *c.rules[j].equal);
							if (c.rules[j].stdDev)		CP_XML_ATTR(L"stdDev", *c.rules[j].stdDev);

							if (c.rules[j].type == 1)
							{
								if (c.rules[j].formula_type)
									CP_XML_ATTR(L"type", *c.rules[j].formula_type);
								else
									CP_XML_ATTR(L"type", L"cellIs");
								if ((c.rules[j].formula) && (!c.rules[j].formula->empty()))
								{
									CP_XML_NODE(L"formula")
									{
										CP_XML_CONTENT(*c.rules[j].formula);
									}
								}
								if ((c.rules[j].formula2) && (!c.rules[j].formula2->empty()))
								{
									CP_XML_NODE(L"formula")
									{
										CP_XML_CONTENT(*c.rules[j].formula2);
									}
								}
							}
							else if (c.rules[j].type == 2)
							{
								CP_XML_ATTR(L"type", L"dataBar");
								CP_XML_NODE(L"dataBar")
								{
									if (c.rules[j].showValue)	CP_XML_ATTR(L"showValue", *c.rules[j].showValue);
									if (c.rules[j].minLength)	CP_XML_ATTR(L"minLength", *c.rules[j].minLength);
									if (c.rules[j].maxLength)	CP_XML_ATTR(L"maxLength", *c.rules[j].maxLength);
									if (c.rules[j].gradient)	CP_XML_ATTR(L"gradient", *c.rules[j].gradient);

									for (size_t k = 0; k < c.rules[j].cfvo.size(); k++)
									{
										c.rules[j].cfvo[k].serialize(CP_XML_STREAM());
									}

									CP_XML_NODE(L"color")
									{
										CP_XML_ATTR(L"rgb", !c.rules[j].color.empty() ? c.rules[j].color[0] : L"FF000000");
									}
								}
							}
							else if (c.rules[j].type == 3)
							{
								CP_XML_ATTR(L"type", L"colorScale");
								CP_XML_NODE(L"colorScale")
								{
									for (size_t k = 0; k < c.rules[j].cfvo.size(); k++)
									{
										c.rules[j].cfvo[k].serialize(CP_XML_STREAM());
									}
									for (size_t k = 0; k < c.rules[j].color.size(); k++)
									{
										CP_XML_NODE(L"color")
										{
											CP_XML_ATTR(L"rgb", c.rules[j].color[k]);
										}
									}
								}
							}
							else if (c.rules[j].type == 4)
							{
								CP_XML_ATTR(L"type", L"iconSet");
								CP_XML_NODE(L"iconSet")
								{
									if (c.rules[j].icon_set_type)
									{
										int h = *c.rules[j].icon_set_type;
										switch (*c.rules[j].icon_set_type)
										{
										case 1: CP_XML_ATTR(L"iconSet", L"3ArrowsGray"); break;
										case 2: CP_XML_ATTR(L"iconSet", L"3Flags"); break;
										case 3: CP_XML_ATTR(L"iconSet", L"3Signs"); break;
										case 4: CP_XML_ATTR(L"iconSet", L"3Symbols"); break;
										case 5: CP_XML_ATTR(L"iconSet", L"3Symbols2"); break;
										case 6: CP_XML_ATTR(L"iconSet", L"3TrafficLights1"); break;
										case 7: CP_XML_ATTR(L"iconSet", L"3TrafficLights2"); break;
										case 8: CP_XML_ATTR(L"iconSet", L"4Arrows"); break;
										case 9: CP_XML_ATTR(L"iconSet", L"4ArrowsGray"); break;
										case 10: CP_XML_ATTR(L"iconSet", L"4Rating"); break;
										case 11: CP_XML_ATTR(L"iconSet", L"4RedToBlack"); break;
										case 12: CP_XML_ATTR(L"iconSet", L"4TrafficLights"); break;
										case 13: CP_XML_ATTR(L"iconSet", L"5Arrows"); break;
										case 14: CP_XML_ATTR(L"iconSet", L"5ArrowsGray"); break;
										case 15: CP_XML_ATTR(L"iconSet", L"5Quarters"); break;
										case 16: CP_XML_ATTR(L"iconSet", L"5Rating"); break;
										case 17: CP_XML_ATTR(L"iconSet", L"3Triangles"); break;
										case 18: CP_XML_ATTR(L"iconSet", L"3Stars"); break;
										case 19: CP_XML_ATTR(L"iconSet", L"5Boxes"); break; //todooo to ext
										case 0:
										default: CP_XML_ATTR(L"iconSet", L"3Arrows"); break;
											break;
										}
									}
									if (c.rules[j].showValue)	CP_XML_ATTR(L"showValue", *c.rules[j].showValue);

									for (size_t k = 0; k < c.rules[j].cfvo.size(); k++)
									{
										c.rules[j].cfvo[k].serialize(CP_XML_STREAM());
									}
								}
							}
							else if (c.rules[j].type == 5)
							{
								CP_XML_ATTR(L"type", L"timePeriod");
								switch (*c.rules[j].time_period)
								{
								case 0: CP_XML_ATTR(L"timePeriod", L"today"); break;
								case 1: CP_XML_ATTR(L"timePeriod", L"yesterday"); break;
								case 2: CP_XML_ATTR(L"timePeriod", L"tomorrow"); break;
								case 3: CP_XML_ATTR(L"timePeriod", L"last7Days"); break;
								case 4: CP_XML_ATTR(L"timePeriod", L"thisMonth"); break;
								case 5: CP_XML_ATTR(L"timePeriod", L"lastMonth"); break;
								case 6: CP_XML_ATTR(L"timePeriod", L"nextMonth");	break;
								case 7: CP_XML_ATTR(L"timePeriod", L"thisWeek"); break;
								case 8: CP_XML_ATTR(L"timePeriod", L"lastWeek"); break;
								case 9: CP_XML_ATTR(L"timePeriod", L"nextWeek"); break;
								}
							}
						}
					}
				}
			}
		}
    }
	void serializeEx(std::wostream& _Wostream)
	{
		if (conditionalFormattings_.empty()) return;

		int priority = 1;
		CP_XML_WRITER(_Wostream)
		{
			for (size_t i = 0; i < conditionalFormattings_.size(); i++)
			{
				conditionalFormatting& c = conditionalFormattings_[i];

				if (c.bUsed) continue;
				if (c.rules.size() < 1) continue;
				
				for (size_t j = 0; j < c.rules.size(); j++)
				{
					if (c.rules[j].isExtended())
					{
						c.bUsed = true;
						break;
					}
				}
				if (!c.bUsed) continue;

				CP_XML_NODE(L"x14:conditionalFormatting")
				{
					CP_XML_ATTR(L"xmlns:xm", L"http://schemas.microsoft.com/office/excel/2006/main");

					for (size_t j = 0; j < c.rules.size(); j++)
					{
						if (c.rules[j].type < 1 || c.rules[j].type > 5) continue;

						CP_XML_NODE(L"x14:cfRule")
						{
							CP_XML_ATTR(L"priority", priority++);
							
							if (c.rules[j].dxfId)		CP_XML_ATTR(L"dxfId", *c.rules[j].dxfId);
							if (c.rules[j].percent)		CP_XML_ATTR(L"percent", *c.rules[j].percent);
							if (c.rules[j].operator_)	CP_XML_ATTR(L"operator", *c.rules[j].operator_);
							if (c.rules[j].stopIfTrue)	CP_XML_ATTR(L"stopIfTrue", *c.rules[j].stopIfTrue);
							if (c.rules[j].text)		CP_XML_ATTR(L"text", *c.rules[j].text);
							if (c.rules[j].rank)		CP_XML_ATTR(L"rank", *c.rules[j].rank);
							if (c.rules[j].bottom)		CP_XML_ATTR(L"bottom", *c.rules[j].bottom);
							if (c.rules[j].above)		CP_XML_ATTR(L"aboveAverage", *c.rules[j].above);
							if (c.rules[j].equal)		CP_XML_ATTR(L"equalAverage", *c.rules[j].equal);
							if (c.rules[j].stdDev)		CP_XML_ATTR(L"stdDev", *c.rules[j].stdDev);

							if (c.rules[j].type == 1)
							{
								if (c.rules[j].formula_type)
									CP_XML_ATTR(L"type", *c.rules[j].formula_type);
								else
									CP_XML_ATTR(L"type", L"cellIs");
								if ((c.rules[j].formula) && (!c.rules[j].formula->empty()))
								{
									CP_XML_NODE(L"formula")
									{
										CP_XML_CONTENT(*c.rules[j].formula);
									}
								}
								if ((c.rules[j].formula2) && (!c.rules[j].formula2->empty()))
								{
									CP_XML_NODE(L"formula")
									{
										CP_XML_CONTENT(*c.rules[j].formula2);
									}
								}
							}
							else if (c.rules[j].type == 2)
							{
								CP_XML_ATTR(L"type", L"dataBar");
								CP_XML_NODE(L"x14:dataBar")
								{
									if (c.rules[j].showValue)	CP_XML_ATTR(L"showValue", *c.rules[j].showValue);
									if (c.rules[j].minLength)	CP_XML_ATTR(L"minLength", *c.rules[j].minLength);
									if (c.rules[j].maxLength)	CP_XML_ATTR(L"maxLength", *c.rules[j].maxLength);
									if (c.rules[j].gradient)	CP_XML_ATTR(L"gradient", *c.rules[j].gradient);
									if (c.rules[j].axis_position) CP_XML_ATTR(L"axisPosition", *c.rules[j].axis_position);

									for (size_t k = 0; k < c.rules[j].cfvo.size(); k++)
									{
										c.rules[j].cfvo[k].serializeEx(CP_XML_STREAM());
									}
									CP_XML_NODE(L"x14:fillColor")
									{
										CP_XML_ATTR(L"rgb", !c.rules[j].color.empty() ? c.rules[j].color[0] : L"FF000000");
									}
									if (c.rules[j].negative_color)
									{
										CP_XML_NODE(L"x14:negativeFillColor")
										{
											CP_XML_ATTR(L"rgb", *c.rules[j].negative_color);
										}
									}
									if (c.rules[j].axis_color)
									{
										CP_XML_NODE(L"x14:axisColor")
										{
											CP_XML_ATTR(L"rgb", *c.rules[j].axis_color);
										}
									}
								}
							}
							else if (c.rules[j].type == 3)
							{
								CP_XML_ATTR(L"type", L"colorScale");
								CP_XML_NODE(L"x14:colorScale")
								{
									for (size_t k = 0; k < c.rules[j].cfvo.size(); k++)
									{
										c.rules[j].cfvo[k].serialize(CP_XML_STREAM());
									}
									for (size_t k = 0; k < c.rules[j].color.size(); k++)
									{
										CP_XML_NODE(L"x14:color")
										{
											CP_XML_ATTR(L"rgb", c.rules[j].color[k]);
										}
									}
								}
							}
							else if (c.rules[j].type == 4)
							{
								CP_XML_ATTR(L"type", L"iconSet");
								CP_XML_NODE(L"x14:iconSet")
								{
									if (c.rules[j].icon_set_type)
									{
										switch (*c.rules[j].icon_set_type)
										{
										case 1: CP_XML_ATTR(L"iconSet", L"3ArrowsGray"); break;
										case 2: CP_XML_ATTR(L"iconSet", L"3Flags"); break;
										case 3: CP_XML_ATTR(L"iconSet", L"3Signs"); break;
										case 4: CP_XML_ATTR(L"iconSet", L"3Symbols"); break;
										case 5: CP_XML_ATTR(L"iconSet", L"3Symbols2"); break;
										case 6: CP_XML_ATTR(L"iconSet", L"3TrafficLights1"); break;
										case 7: CP_XML_ATTR(L"iconSet", L"3TrafficLights2"); break;
										case 8: CP_XML_ATTR(L"iconSet", L"4Arrows"); break;
										case 9: CP_XML_ATTR(L"iconSet", L"4ArrowsGray"); break;
										case 10: CP_XML_ATTR(L"iconSet", L"4Rating"); break;
										case 11: CP_XML_ATTR(L"iconSet", L"4RedToBlack"); break;
										case 12: CP_XML_ATTR(L"iconSet", L"4TrafficLights"); break;
										case 13: CP_XML_ATTR(L"iconSet", L"5Arrows"); break;
										case 14: CP_XML_ATTR(L"iconSet", L"5ArrowsGray"); break;
										case 15: CP_XML_ATTR(L"iconSet", L"5Quarters"); break;
										case 16: CP_XML_ATTR(L"iconSet", L"5Rating"); break;
										case 0:
										default: CP_XML_ATTR(L"iconSet", L"3Arrows"); break;
											break;
										}
									}
									if (c.rules[j].showValue) CP_XML_ATTR(L"showValue", *c.rules[j].showValue);

									for (size_t k = 0; k < c.rules[j].cfvo.size(); k++)
									{
										c.rules[j].cfvo[k].serialize(CP_XML_STREAM());
									}
								}
							}
							else if (c.rules[j].type == 5)
							{
								CP_XML_ATTR(L"type", L"timePeriod");
								switch (*c.rules[j].time_period)
								{
								case 0: CP_XML_ATTR(L"timePeriod", L"today"); break;
								case 1: CP_XML_ATTR(L"timePeriod", L"yesterday"); break;
								case 2: CP_XML_ATTR(L"timePeriod", L"tomorrow"); break;
								case 3: CP_XML_ATTR(L"timePeriod", L"last7Days"); break;
								case 4: CP_XML_ATTR(L"timePeriod", L"thisMonth"); break;
								case 5: CP_XML_ATTR(L"timePeriod", L"lastMonth"); break;
								case 6: CP_XML_ATTR(L"timePeriod", L"nextMonth");	break;
								case 7: CP_XML_ATTR(L"timePeriod", L"thisWeek"); break;
								case 8: CP_XML_ATTR(L"timePeriod", L"lastWeek"); break;
								case 9: CP_XML_ATTR(L"timePeriod", L"nextWeek"); break;
								}
							}
						}
					}
					
					CP_XML_NODE(L"xm:sqref")
					{
						CP_XML_CONTENT(c.ref);
					}
				}
			}
		}
	}

    std::vector<conditionalFormatting> conditionalFormattings_;
};

xlsx_conditionalFormatting_context::xlsx_conditionalFormatting_context() :
			impl_(new xlsx_conditionalFormatting_context::Impl())
{}

xlsx_conditionalFormatting_context::~xlsx_conditionalFormatting_context()
{
}
void xlsx_conditionalFormatting_context::serialize(std::wostream & _Wostream)
{
    return impl_->serialize(_Wostream);
}
void xlsx_conditionalFormatting_context::serializeEx(std::wostream& _Wostream)
{
	return impl_->serializeEx(_Wostream);
}
void xlsx_conditionalFormatting_context::start(std::wstring ref)
{
	formulasconvert::odf2oox_converter converter;
	impl_->conditionalFormattings_.push_back(conditionalFormatting());
	
	impl_->conditionalFormattings_.back().ref = converter.convert_named_ref(ref, false, L" ");
}

void xlsx_conditionalFormatting_context::add_rule(int type)
{
	impl_->conditionalFormattings_.back().rules.push_back(rule());
	impl_->conditionalFormattings_.back().rules.back().type = type;
}
void xlsx_conditionalFormatting_context::set_formula(std::wstring f)
{
	formulasconvert::odf2oox_converter converter;
	int pos = -1;
	std::wstring val;
	
	if ( f == L"unique")
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"uniqueValues";
		impl_->conditionalFormattings_.back().rules.back().formula = L"0";
	}
	else if ( f == L"duplicate")
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"duplicateValues";
		impl_->conditionalFormattings_.back().rules.back().formula = L"0";
	}	
	else if ( f == L"above-average")
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"aboveAverage";
	}
	else if (f == L"below-average")
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"aboveAverage";
		impl_->conditionalFormattings_.back().rules.back().above = false;
	}
	else if (f == L"above-equal-average")
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"aboveAverage";
		impl_->conditionalFormattings_.back().rules.back().equal = true;
	}
	else if (f == L"below-equal-average")
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"aboveAverage";
		impl_->conditionalFormattings_.back().rules.back().above = false;
		impl_->conditionalFormattings_.back().rules.back().equal = true;
	}
	else if ( 0 <= (pos = f.find(L"formula-is(")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"expression";
		val = f.substr(11, f.size() - 12);

		if (0 == (pos = val.find(L"\"")))	//Raport_7A.ods или выкинуть ограждающие кавычки с формулы?
		{
			impl_->conditionalFormattings_.back().rules.back().text = val;
			val.clear();
		}
		
		impl_->conditionalFormattings_.back().rules.back().formula = converter.convert(val);
	}
	else if (0 <= (pos = f.find(L"is-between(")))
	{
		val = f.substr(11, f.size() - 12);
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"expression";
		impl_->conditionalFormattings_.back().rules.back().formula = converter.convert_named_expr(val);
	}
	else if (0 <= (pos = f.find(L"is-time(")))
	{
		val = f.substr(8, f.size() - 9);
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"expression";
		impl_->conditionalFormattings_.back().rules.back().formula = converter.convert_named_expr(val);
	}
	else if (0 <= (pos = f.find(L"is-error")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"containsErrors";
		impl_->conditionalFormattings_.back().rules.back().formula = L"0";
	}
	else if (0 <= (pos = f.find(L"is-no-error")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"notContainsErrors";
	}	
	else if (0 <= (pos = f.find(L"duplicate")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"duplicateValues";
	}
	else if (0 <= (pos = f.find(L"begins-with")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"beginsWith";
		std::wstring text = f.substr(pos + 12, f.length() - pos - 13);
		
		if (0 == text.find(L"\"") && text.length() - 1 == text.rfind(L"\""))
		{
			text = text.substr(1, text.length() - 2);
		}
		
		impl_->conditionalFormattings_.back().rules.back().text = text;
	}
	else if (0 <= (pos = f.find(L"ends-with")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"endsWith";
		std::wstring text = f.substr(pos + 10, f.length() - pos - 11);
		
		if (0 == text.find(L"\"") && text.length() - 1 == text.rfind(L"\""))
		{
			text = text.substr(1, text.length() - 2);
		}
		
		impl_->conditionalFormattings_.back().rules.back().text = text;
	}
	else if (0 <= (pos = f.find(L"contains-text")))
	{
		if (std::wstring::npos != f.find(L"not-contains-text")) 
				impl_->conditionalFormattings_.back().rules.back().formula_type = L"notContainsText";
		else	impl_->conditionalFormattings_.back().rules.back().formula_type = L"containsText";

		std::wstring text = f.substr(pos + 14, f.length() - pos - 15);

		if (std::wstring::npos != text.find(L"IF(") || 
			std::wstring::npos != text.find(L"AND(") ||
			std::wstring::npos != text.find(L"NOT(") ||
			std::wstring::npos != text.find(L"ISERROR(") ||
			std::wstring::npos != text.find(L"SEARCH("))
		{
			impl_->conditionalFormattings_.back().rules.back().text = L"";
			impl_->conditionalFormattings_.back().rules.back().formula = converter.convert(text);
		}
		else if (std::wstring::npos != text.find(L"LEN(TRIM(") && 
			std::wstring::npos != text.find(L"=0"))
		{
			impl_->conditionalFormattings_.back().rules.back().text = L"";
			impl_->conditionalFormattings_.back().rules.back().formula = L"0";
		}
		else
		{
			impl_->conditionalFormattings_.back().rules.back().operator_ = L"containsText";
			if ( 0 == text.find(L"\"") && text.length() - 1 == text.rfind(L"\""))
			{
				text = text.substr(1, text.length() - 2);
			}
			std::wstring ref = impl_->conditionalFormattings_.back().ref;
			
			size_t pos;
			if ((pos = ref.find(L":")) != std::wstring::npos)
			{
				ref = ref.substr(0, pos);
			}
			impl_->conditionalFormattings_.back().rules.back().text = text;
			impl_->conditionalFormattings_.back().rules.back().formula = L"NOT(ISERROR(SEARCH(\"" + text + L"\"," + ref + L")))";
		}
	}
	else if (0 <= (pos = f.find(L"top")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"top10";
		if (0 < (pos = f.find(L"percent")))
		{
			impl_->conditionalFormattings_.back().rules.back().percent = true;
		}
		if (0 <= (pos = f.find(L"(")))
		{
			val = f.substr(pos + 1, f.length() - pos - 2);
			if (!val.empty())
				impl_->conditionalFormattings_.back().rules.back().rank = boost::lexical_cast<int>(val);
		}	
	}
	else if (0 <= (pos = f.find(L"bottom")))
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"top10";
		impl_->conditionalFormattings_.back().rules.back().bottom = true;
		if (0 < (pos = f.find(L"percent")))
		{
			impl_->conditionalFormattings_.back().rules.back().percent = true;
		}
		if (0 <= (pos = f.find(L"(")))
		{
			val = f.substr(pos + 1, f.length() - pos - 2);
			if (!val.empty())
				impl_->conditionalFormattings_.back().rules.back().rank = boost::lexical_cast<int>(val);
		}
	}
	else
	{
		impl_->conditionalFormattings_.back().rules.back().formula_type = L"cellIs";
		
		if (0 <= (pos = f.find(L"!empty")))
		{
			val = converter.convert_named_expr( f );
		}
		else if (0 <= (pos = f.find(L"empty")))
		{
			val = converter.convert_named_expr( f );
		}
		else if (0 <= (pos = f.find(L"bottom")))
		{
			val = converter.convert_named_expr( f );
		}
		else if (0 <= (pos = f.find(L"between")))
		{
			if (0 <= (pos = f.find(L"not-between")))
			{
				impl_->conditionalFormattings_.back().rules.back().operator_ = L"notBetween";
				val = f.substr(12, f.length() - 13);
			}
			else
			{
				impl_->conditionalFormattings_.back().rules.back().operator_ = L"between";
				val = f.substr(8, f.length() - 9);
			}

			XmlUtils::replace_all(val, L"(", L"");
			XmlUtils::replace_all(val, L")", L"");
			if (0 <= (pos = val.find(L",")))
			{
				impl_->conditionalFormattings_.back().rules.back().formula2 = converter.convert_named_expr(val.substr(pos + 1));
				val = val.substr(0, pos);
			}
			val = converter.convert_named_expr(val);
		}
		else if (0 <= (pos = f.find(L"!=")))
		{
			impl_->conditionalFormattings_.back().rules.back().operator_ = L"notEqual";
			val = converter.convert_named_expr( f.substr(2) );
		}
		else if (0 <= (pos = f.find(L"<=")))
		{
			impl_->conditionalFormattings_.back().rules.back().operator_ = L"lessThanOrEqual";
			val = converter.convert_named_expr( f.substr(2) );
		}	
		else if (0 <= (pos = f.find(L">=")))
		{
			impl_->conditionalFormattings_.back().rules.back().operator_ = L"greaterThanOrEqual";
			val = converter.convert_named_expr( f.substr(2) );
		}
		else if (0 <= (pos = f.find(L"=")))
		{
			impl_->conditionalFormattings_.back().rules.back().operator_ = L"equal";
			val = converter.convert_named_expr( f.substr(1) );
		}
		else if (0 <= (pos = f.find(L"<")))
		{
			impl_->conditionalFormattings_.back().rules.back().operator_ = L"lessThan";
			val = converter.convert_named_expr( f.substr(1) );
		}
		else if (0 <= (pos = f.find(L">")))
		{
			impl_->conditionalFormattings_.back().rules.back().operator_ = L"greaterThan";
			val = converter.convert_named_expr( f.substr(1) );
		}
		else
		{
			val = converter.convert( f );
		}
		
		if (!val.empty())
			impl_->conditionalFormattings_.back().rules.back().formula = val;
	}		
}
void xlsx_conditionalFormatting_context::set_gradient(bool val)
{
	impl_->conditionalFormattings_.back().rules.back().gradient = val;
}
void xlsx_conditionalFormatting_context::set_dataBar(_CP_OPT(unsigned int) min, _CP_OPT(unsigned int) max)
{
	impl_->conditionalFormattings_.back().rules.back().minLength = min;
	impl_->conditionalFormattings_.back().rules.back().maxLength = max;
}
void xlsx_conditionalFormatting_context::set_dxf(int dxfId)
{
	impl_->conditionalFormattings_.back().rules.back().dxfId = dxfId;
}
void xlsx_conditionalFormatting_context::add_sfv(int type, std::wstring value)
{
	_cfvo cfvo;
	cfvo.type = type;

	if ( type == 6)
	{
		set_formula(value);
		cfvo.val =impl_->conditionalFormattings_.back().rules.back().formula;
		
		impl_->conditionalFormattings_.back().rules.back().formula.reset(); 
	}
	else
	{
		if (!value.empty()) cfvo.val = value;
		else cfvo.val = L"0";
	}

	impl_->conditionalFormattings_.back().rules.back().cfvo.push_back(cfvo);
}
void xlsx_conditionalFormatting_context::add_color(std::wstring col)
{
	impl_->conditionalFormattings_.back().rules.back().color.push_back(col);
}
void xlsx_conditionalFormatting_context::set_negative_color(std::wstring col) 
{
	impl_->conditionalFormattings_.back().rules.back().negative_color = col;
}
void xlsx_conditionalFormatting_context::set_axis_position(std::wstring val) 
{
	impl_->conditionalFormattings_.back().rules.back().axis_position = val;
}
void xlsx_conditionalFormatting_context::set_axis_color(std::wstring val)
{
	impl_->conditionalFormattings_.back().rules.back().axis_color = val;
}
void xlsx_conditionalFormatting_context::set_icon_set_type(int type)
{
	impl_->conditionalFormattings_.back().rules.back().icon_set_type = type;
}
void xlsx_conditionalFormatting_context::set_showVal(bool val)
{
	impl_->conditionalFormattings_.back().rules.back().showValue = val;
}
void xlsx_conditionalFormatting_context::set_time_period(int val)
{
	impl_->conditionalFormattings_.back().rules.back().time_period = val;
}
void xlsx_conditionalFormatting_context::set_stdDev(int val)
{
	impl_->conditionalFormattings_.back().rules.back().stdDev = val;
}

}
}
