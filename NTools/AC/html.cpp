#include "html.h"
#include "AC/stringUtil.h"
namespace {
	struct _BSCodeMap {
		std::string		replaceWhat;		// 被转换
		std::string		replaceWithWhat;	// 

		_BSCodeMap(char *pwithWhat, char *pwhat)
			: replaceWhat(pwhat?pwhat:"")
			, replaceWithWhat(pwithWhat?pwithWhat:"")
		{}
	};
#pragma warning(disable:4566)
	_BSCodeMap nc_pReplaceItem[] = {
		_BSCodeMap("\"", "&quot;"),
		_BSCodeMap("&", "&amp;"),
		_BSCodeMap("<", "&lt;"),
		_BSCodeMap(">", "&gt;"),
		_BSCodeMap(" ", "&nbsp;"),
		_BSCodeMap("¡"/*&#161;*/,	"&iexcl;"),		// Inverted exclamation
		_BSCodeMap("¢"/*&#162;*/,	"&cent;"),		// 货币分标志Cent sign
		_BSCodeMap("£"/*&#163;*/,	"&pound;"),		// 英镑标志Pound sterling
		_BSCodeMap("¤"/*&#164;*/,	"&curren;"),	// 通用货币标志General currency sign
		_BSCodeMap("¥"/*&#165;*/,	"&yen;"),		// 日元标志Yen sign
		_BSCodeMap("¦"/*&#166;*/,	"&brvbar;"),	// 断竖线Broken vertical bar
		_BSCodeMap("¦"/*&#166;*/,	"&brkbar;"),	// 断竖线Broken vertical bar
		_BSCodeMap("§"/*&#167;*/,	"&sect;"),		// 分节号Section sign
		_BSCodeMap("¨"/*&#168;*/,	"&uml;"),		// 变音符号Umlaut
		_BSCodeMap("¨"/*&#168;*/,	"&die;"),		// 变音符号Umlaut
		_BSCodeMap("©"/*&#169;*/,	"&copy;"),		// 版权标志Copyright
		_BSCodeMap("ª"/*&#170;*/,	"&ordf;"),		// Feminine ordinal
		_BSCodeMap("«"/*&#171;*/,	"&laquo;"),		// Left angle quote, guillemet left
		_BSCodeMap("¬"/*&#172;*/,	"&not"),		// Not sign
		_BSCodeMap(" "/*&#173;*/,	"&shy;"),		// Soft hyphen
		_BSCodeMap("®"/*&#174;*/,	"&reg;"),		// 注册商标标志Registered trademark
		_BSCodeMap("¯"/*&#175;*/,	"&macr;"),		// 长音符号Macron accent
		_BSCodeMap("¯"/*&#175;*/,	"&hibar;"),		// 长音符号Macron accent
		_BSCodeMap("°"/*&#176;*/,	"&deg;"),		// 度数标志Degree sign
		_BSCodeMap("±"/*&#177;*/,	"&plusmn;"),	// 加或减Plus or minus
		_BSCodeMap("²"/*&#178;*/,	"&sup2;"),		// 上标2 Superscript two
		_BSCodeMap("³"/*&#179;*/,	"&sup3;"),		// 上标3 Superscript three
		_BSCodeMap("´"/*&#180;*/,	"&acute;"),		// 尖重音符Acute accent
		_BSCodeMap("µ"/*&#181;*/,	"&micro;"),		// Micro sign
		_BSCodeMap("¶"/*&#182;*/,	"&para;"),		// Paragraph sign
		_BSCodeMap("·"/*&#183;*/,	"&middot;"),	// Middle dot
		_BSCodeMap("¸"/*&#184;*/,	"&cedil;"),		// Cedilla
		_BSCodeMap("¹"/*&#185;*/,	"&sup1;"),		// 上标1 Superscript one
		_BSCodeMap("º"/*&#186;*/,	"&ordm;"),		// Masculine ordinal
		_BSCodeMap("»"/*&#187;*/,	"&raquo;"),		// Right angle quote, guillemet right
		_BSCodeMap("¼"/*&#188;*/,	"&frac14;"),	// 四分之一Fraction one-fourth
		_BSCodeMap("½"/*&#189;*/,	"&frac12;"),	// 二分之一Fraction one-half
		_BSCodeMap("¾"/*&#190;*/,	"&frac34;"),	// 四分之三Fraction three-fourths
		_BSCodeMap("¿"/*&#191;*/,	"&iquest;"),	// Inverted question mark
		_BSCodeMap("À"/*&#192;*/,	"&Agrave;"),	// Capital A, grave accent
		_BSCodeMap("Á"/*&#193;*/,	"&Aacute;"),	// Capital A, acute accent
		_BSCodeMap("Â"/*&#194;*/,	"&Acirc;"),		// Capital A, circumflex
		_BSCodeMap("Ã"/*&#195;*/,	"&Atilde;"),	// Capital A, tilde
		_BSCodeMap("Ä"/*&#196;*/,	"&Auml;"),		// Capital A, di?esis / umlaut
		_BSCodeMap("Å"/*&#197;*/,	"&Aring;"),		// Capital A, ring
		_BSCodeMap("Æ"/*&#198;*/,	"&AElig;"),		// Capital AE ligature
		_BSCodeMap("Ç"/*&#199;*/,	"&Ccedil;"),	// Capital C, cedilla
		_BSCodeMap("È"/*&#200;*/,	"&Egrave;"),	// Capital E, grave accent
		_BSCodeMap("É"/*&#201;*/,	"&Eacute;"),	// Capital E, acute accent
		_BSCodeMap("Ê"/*&#202;*/,	"&Ecirc;"),		// Capital E, circumflex
		_BSCodeMap("Ë"/*&#203;*/,	"&Euml;"),		// Capital E, di?esis / umlaut
		_BSCodeMap("Ì"/*&#204;*/,	"&Igrave;"),	// Capital I, grave accent
		_BSCodeMap("Í"/*&#205;*/,	"&Iacute;"),	// Capital I, acute accent
		_BSCodeMap("Î"/*&#206;*/,	"&Icirc;"),		// Capital I, circumflex
		_BSCodeMap("Ï"/*&#207;*/,	"&Iuml;"),		// Capital I, di?esis / umlaut
		_BSCodeMap("Ð"/*&#208;*/,	"&ETH;"),		// Capital Eth, Icelandic
		_BSCodeMap("Ñ"/*&#209;*/,	"&Ntilde;"),	// Capital N, tilde
		_BSCodeMap("Ò"/*&#210;*/,	"&Ograve;"),	// Capital O, grave accent
		_BSCodeMap("Ó"/*&#211;*/,	"&Oacute;"),	// Capital O, acute accent
		_BSCodeMap("Ô"/*&#212;*/,	"&Ocirc;"),		// Capital O, circumflex
		_BSCodeMap("Õ"/*&#213;*/,	"&Otilde;"),	// Capital O, tilde
		_BSCodeMap("Ö"/*&#214;*/,	"&Ouml;"),		// Capital O, di?esis / umlaut
		_BSCodeMap("×"/*&#215;*/,	"&times;"),		// 乘号Multiply sign
		_BSCodeMap("Ø"/*&#216;*/,	"&Oslash;"),	// Capital O, slash
		_BSCodeMap("Ù"/*&#217;*/,	"&Ugrave;"),	// Capital U, grave accent
		_BSCodeMap("Ú"/*&#218;*/,	"&Uacute;"),	// Capital U, acute accent
		_BSCodeMap("Û"/*&#219;*/,	"&Ucirc;"),		// Capital U, circumflex
		_BSCodeMap("Ü"/*&#220;*/,	"&Uuml;"),		// Capital U, di?esis / umlaut
		_BSCodeMap("Ý"/*&#221;*/,	"&Yacute;"),	// Capital Y, acute accent
		_BSCodeMap("Þ"/*&#222;*/,	"&THORN;"),		// Capital Thorn, Icelandic
		_BSCodeMap("ß"/*&#223;*/,	"&szlig;"),		// Small sharp s, German sz
		_BSCodeMap("à"/*&#224;*/,	"&agrave;"),	// Small a, grave accent
		_BSCodeMap("á"/*&#225;*/,	"&aacute;"),	// Small a, acute accent
		_BSCodeMap("â"/*&#226;*/,	"&acirc;"),		// Small a, circumflex
		_BSCodeMap("ã"/*&#227;*/,	"&atilde;"),	// Small a, tilde
		_BSCodeMap("ä"/*&#228;*/,	"&auml;"),		// Small a, di?esis / umlaut
		_BSCodeMap("å"/*&#229;*/,	"&aring;"),		// Small a, ring
		_BSCodeMap("æ"/*&#230;*/,	"&aelig;"),		// Small ae ligature
		_BSCodeMap("ç"/*&#231;*/,	"&ccedil;"),	// Small c, cedilla
		_BSCodeMap("è"/*&#232;*/,	"&egrave;"),	// Small e, grave accent
		_BSCodeMap("é"/*&#233;*/,	"&eacute;"),	// Small e, acute accent
		_BSCodeMap("ê"/*&#234;*/,	"&ecirc;"),		// Small e, circumflex
		_BSCodeMap("ë"/*&#235;*/,	"&euml;"),		// Small e, di?esis / umlaut
		_BSCodeMap("ì"/*&#236;*/,	"&igrave;"),	// Small i, grave accent
		_BSCodeMap("í"/*&#237;*/,	"&iacute;"),	// Small i, acute accent
		_BSCodeMap("î"/*&#238;*/,	"&icirc;"),		// Small i, circumflex
		_BSCodeMap("ï"/*&#239;*/,	"&iuml;"),		// Small i, di?esis / umlaut
		_BSCodeMap("ð"/*&#240;*/,	"&eth;"),		// Small eth, Icelandic
		_BSCodeMap("ñ"/*&#241;*/,	"&ntilde;"),	// Small n, tilde
		_BSCodeMap("ò"/*&#242;*/,	"&ograve;"),	// Small o, grave accent
		_BSCodeMap("ó"/*&#243;*/,	"&oacute;"),	// Small o, acute accent
		_BSCodeMap("ô"/*&#244;*/,	"&ocirc;"),		// Small o, circumflex
		_BSCodeMap("õ"/*&#245;*/,	"&otilde;"),	// Small o, tilde
		_BSCodeMap("ö"/*&#246;*/,	"&ouml;"),		// Small o, di?esis / umlaut
		_BSCodeMap("÷"/*&#247;*/,	"&divide;"),	// 除号Division sign
		_BSCodeMap("ø"/*&#248;*/,	"&oslash;"),	// Small o, slash
		_BSCodeMap("ù"/*&#249;*/,	"&ugrave;"),	// Small u, grave accent
		_BSCodeMap("ú"/*&#250;*/,	"&uacute;"),	// Small u, acute accent
		_BSCodeMap("û"/*&#251;*/,	"&ucirc;"),		// Small u, circumflex
		_BSCodeMap("ü"/*&#252;*/,	"&uuml;"),		// Small u, di?esis / umlaut
		_BSCodeMap("ý"/*&#253;*/,	"&yacute;"),	// Small y, acute accent
		_BSCodeMap("þ"/*&#254;*/,	"&thorn;"),		// Small thorn, Icelandic
		_BSCodeMap("ÿ"/*&#255;*/,	"&yuml;")		// Small y, umlaut

	};
}
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	html  类		2015-7-6
//	基于tidylib.lib封装，对html操作
////////////////////////////////////////////////////////////////////////////////////////////////////////

html::html()
{

}
html::~html()
{

}

std::string html::decode(const std::string &source)
{
	std::string result;
	decodeRef(result);
	return result;
}
void html::decodeRef(std::string &source)
{
	for (int idx = 0; idx < _countof(nc_pReplaceItem); ++idx)
	{
		gtc::stringUtil::replaceAllRef(source, nc_pReplaceItem[idx].replaceWhat, nc_pReplaceItem[idx].replaceWithWhat);
	}
}
}