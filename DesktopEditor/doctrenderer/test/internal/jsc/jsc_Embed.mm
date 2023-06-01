// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT CHANGE IT!
// IF YOU NEED TO UPDATE THIS CODE, JUST RERUN PYTHON SCRIPT.

#include "../Embed.h"
#import "js_embed.h"

@protocol IJSCTestEmbed <JSExport>
-(JSValue*) FunctionSum : (JSValue*)param1 : (JSValue*)param2;
-(JSValue*) FunctionSquare : (JSValue*)param;
-(JSValue*) FunctionDel : (JSValue*)param1 : (JSValue*)param2;
-(JSValue*) FunctionGet;
@end

@interface CJSCTestEmbed : NSObject<IJSCTestEmbed, JSEmbedObjectProtocol>
{
@public
	CTestEmbed* m_internal;
}
@end

@implementation CJSCTestEmbed
EMBED_OBJECT_WRAPPER_METHODS(CTestEmbed);

-(JSValue*) FunctionSum : (JSValue*)param1 : (JSValue*)param2
{
	JSSmart<CJSValue> ret = m_internal->FunctionSum(CJSEmbedObjectAdapterJSC::Native2Value(param1), CJSEmbedObjectAdapterJSC::Native2Value(param2));
	return CJSEmbedObjectAdapterJSC::Value2Native(ret);
}

-(JSValue*) FunctionSquare : (JSValue*)param
{
	JSSmart<CJSValue> ret = m_internal->FunctionSquare(CJSEmbedObjectAdapterJSC::Native2Value(param));
	return CJSEmbedObjectAdapterJSC::Value2Native(ret);
}

-(JSValue*) FunctionDel : (JSValue*)param1 : (JSValue*)param2
{
	JSSmart<CJSValue> ret = m_internal->FunctionDel(CJSEmbedObjectAdapterJSC::Native2Value(param1), CJSEmbedObjectAdapterJSC::Native2Value(param2));
	return CJSEmbedObjectAdapterJSC::Value2Native(ret);
}

-(JSValue*) FunctionGet
{
	JSSmart<CJSValue> ret = m_internal->FunctionGet();
	return CJSEmbedObjectAdapterJSC::Value2Native(ret);
}

@end

class CTestEmbedAdapter : public CJSEmbedObjectAdapterJSC
{
public:
	id getExportedObject(CJSEmbedObject* pNative)
	{
		return [[CJSCTestEmbed alloc] init:(CTestEmbed*)pNative];
	}
};

CJSEmbedObjectAdapterBase* CTestEmbed::getAdapter()
{
	if (m_pAdapter == nullptr)
		m_pAdapter = new CTestEmbedAdapter();
	return m_pAdapter;
}

std::string CTestEmbed::getName() { return "CTestEmbed"; }

CJSEmbedObject* CTestEmbed::getCreator()
{
	return new CTestEmbed();
}
