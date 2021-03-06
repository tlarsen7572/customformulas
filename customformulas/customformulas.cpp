#include "stdafx.h"
#include "customformulas.h"
#include <map>
#include <string>

using namespace std;

void SetString(FormulaAddInData *pReturnValue, const wchar_t *pString)
{
	size_t nLen = wcslen(pString);
	wchar_t *pStringRet = (wchar_t *)GlobalAlloc(GMEM_FIXED, (nLen + 1) * sizeof(wchar_t));
	wcscpy(pStringRet, pString);
	pReturnValue->pVal = pStringRet;
	pReturnValue->nVarType = 2;
	pReturnValue->isNull = 0;
}

void SetDouble(FormulaAddInData *pReturnValue, double value)
{
	pReturnValue->dVal = value;
	pReturnValue->nVarType = 1;
	pReturnValue->isNull = 0;
}

unsigned long long longPow(unsigned long value, int pow) {
	unsigned long long returnVal = value;
	if (pow == 0) return 1;
	int x = 1;
	while (x < pow) {
		returnVal = returnVal * value;
		x++;
	}
	return returnVal;
}

long CleanNulls(int nNumArgs, FormulaAddInData *pArgs, FormulaAddInData *pReturnValue)
{
	if (nNumArgs != 1)
	{
		SetString(pReturnValue, L"Only one parameter can be provided.");
		return 0;
	}

	FormulaAddInData arg = pArgs[0];
	if (arg.isNull == 1)
	{
		if (arg.nVarType == 2)
		{
			SetString(pReturnValue, L"");
		}
		else
		{
			SetDouble(pReturnValue, 0.0);
		}
	}
	else
	{
		if (arg.nVarType == 2)
		{
			SetString(pReturnValue, arg.pVal);
		}
		else
		{
			SetDouble(pReturnValue, arg.dVal);
		}
	}

	return 1;
}

long SimpleStrToInt(int nNumArgs, FormulaAddInData *pArgs, FormulaAddInData *pReturnValue)
{
	if (pArgs->isNull == 1) {
		pReturnValue->isNull = 1;
		pReturnValue->nVarType = 1;
		return 1;
	}
	if (nNumArgs != 1)
	{
		SetString(pReturnValue, L"Only one parameter can be provided.");
		pReturnValue->isNull = 1;
		return 0;
	}
	if (pArgs->nVarType == 1) {
		SetString(pReturnValue, L"Value must be a string.");
		pReturnValue->isNull = 1;
		return 0;
	}
	if (wcslen(pArgs->pVal) > 12) {
		SetString(pReturnValue, L"String must be 12 characters or less.");
		pReturnValue->isNull = 1;
		return 0;
	}
	if (wcslen(pArgs->pVal) == 0) {
		pReturnValue->isNull = 1;
		pReturnValue->nVarType = 1;
		return 1;
	}

	map<WCHAR, signed long long> longmap = {
		{ '0', 0 },
		{ '1', 1 },
		{ '2', 2 },
		{ '3', 3 },
		{ '4', 4 },
		{ '5', 5 },
		{ '6', 6 },
		{ '7', 7 },
		{ '8', 8 },
		{ '9', 9 },
		{ ' ', 10 },
		{ '_', 11 },
		{ '-', 12 },
		{ '.', 13 },
		{ 'A', 14 },
		{ 'B', 15 },
		{ 'C', 16 },
		{ 'D', 17 },
		{ 'E', 18 },
		{ 'F', 19 },
		{ 'G', 20 },
		{ 'H', 21 },
		{ 'I', 22 },
		{ 'J', 23 },
		{ 'K', 24 },
		{ 'L', 25 },
		{ 'M', 26 },
		{ 'N', 27 },
		{ 'O', 28 },
		{ 'P', 29 },
		{ 'Q', 30 },
		{ 'R', 31 },
		{ 'S', 32 },
		{ 'T', 33 },
		{ 'U', 34 },
		{ 'V', 35 },
		{ 'W', 36 },
		{ 'X', 37 },
		{ 'Y', 38 },
		{ 'Z', 39 },
	};
	signed long long start = INT64_MIN;
	int powvalue = wcslen(pArgs->pVal) - 1;
	int x = 0;
	while (powvalue >= 0)
	{
		WCHAR digit = pArgs->pVal[x];
		if (longmap.count(digit) == 0) {
			SetString(pReturnValue, L"Out of range character.  String can only contain 0-9, A-Z, underscore, period, space, or hyphen.");
			pReturnValue->isNull = 1;
			pReturnValue->nVarType = 1;
			return 0;
		}
		start += (longPow(40, powvalue) * longmap[digit]);
		powvalue--;
		x++;
	}
	wstring startStr = to_wstring(start);
	const wchar_t * startReturnVal{ startStr.c_str() };
	SetString(pReturnValue, startReturnVal);
	return 1;
}

long StrToInt(int nNumArgs, FormulaAddInData *pArgs, FormulaAddInData *pReturnValue)
{
	if (pArgs->isNull == 1) {
		pReturnValue->isNull = 1;
		pReturnValue->nVarType = 1;
		return 1;
	}
	if (nNumArgs != 1)
	{
		SetString(pReturnValue, L"Only one parameter can be provided.");
		pReturnValue->isNull = 1;
		pReturnValue->nVarType = 1;
		return 0;
	}
	if (pArgs->nVarType == 1) {
		SetString(pReturnValue, L"Value must be a string.");
		pReturnValue->isNull = 1;
		pReturnValue->nVarType = 1;
		return 0;
	}
	if (wcslen(pArgs->pVal) == 0) {
		pReturnValue->isNull = 1;
		pReturnValue->nVarType = 1;
		return 1;
	}

	WCHAR validChars[] = {'0','1','2','3','4','5','6','7','8','9' };
	if (pArgs->pVal[0] == '-') {
		if (wcslen(pArgs->pVal) == 1) {
			SetString(pReturnValue, L"Not a valid integer");
			return 0;
		}
	}
	else if (find(begin(validChars), end(validChars), pArgs->pVal[0]) == end(validChars)) {
		SetString(pReturnValue, L"Not a valid integer");
		return 0;
	}

	int i = 1;
	while (i < wcslen(pArgs->pVal)) {
		WCHAR val = pArgs->pVal[i];
		if (find(begin(validChars), end(validChars), val) == end(validChars)) {
			SetString(pReturnValue, L"Not a valid integer");
			return 0;
		}
		i++;
	}
	SetString(pReturnValue, pArgs->pVal);
	return 1;
}