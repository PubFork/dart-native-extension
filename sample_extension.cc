// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/dart_api.h"
#include "include/dart_native_api.h"

#include "If_DBRP.h"
#pragma comment(lib, "DBRx64.lib")

#define MAX_BARCODE_AMOUNT 64

struct barcode_format
{
	const char * pszFormat;
	__int64 llFormat;
};

static struct barcode_format Barcode_Formats[] =
{
	{ "CODE_39", CODE_39 },
	{ "CODE_128", CODE_128 },
	{ "CODE_93", CODE_93 },
	{ "CODABAR", CODABAR },
	{ "ITF", ITF },
	{ "UPC_A", UPC_A },
	{ "UPC_E", UPC_E },
	{ "EAN_13", EAN_13 },
	{ "EAN_8", EAN_8 },
	{ "INDUSTRIAL_25",INDUSTRIAL_25 },
	{ "OneD", OneD },
	{ "QR_CODE", QR_CODE },
	{ "PDF417",PDF417 },
	{ "DATAMATRIX", DATAMATRIX }
};

__int64 GetFormat(int iIndex)
{
	__int64 llFormat = 0;

	switch (iIndex)
	{
	case 1:
		llFormat = OneD | QR_CODE | PDF417 | DATAMATRIX;
		break;
	case 2:
		llFormat = OneD;
		break;
	case 3:
		llFormat = QR_CODE;
		break;
	case 4:
		llFormat = CODE_39;
		break;
	case 5:
		llFormat = CODE_128;
		break;
	case 6:
		llFormat = CODE_93;
		break;
	case 7:
		llFormat = CODABAR;
		break;
	case 8:
		llFormat = ITF;
		break;
	case 9:
		llFormat = INDUSTRIAL_25;
		break;
	case 10:
		llFormat = EAN_13;
		break;
	case 11:
		llFormat = EAN_8;
		break;
	case 12:
		llFormat = UPC_A;
		break;
	case 13:
		llFormat = UPC_E;
		break;
	case 14:
		llFormat = PDF417;
		break;
	case 15:
		llFormat = DATAMATRIX;
		break;
	default:
		break;
	}

	return llFormat;
}

const char * GetFormatStr(__int64 format)
{
	int iCount = sizeof(Barcode_Formats) / sizeof(Barcode_Formats[0]);

	for (int index = 0; index < iCount; index++)
	{
		if (Barcode_Formats[index].llFormat == format)
			return Barcode_Formats[index].pszFormat;
	}

	return "UNKNOWN";
}

Dart_NativeFunction ResolveName(Dart_Handle name,
                                int argc,
                                bool* auto_setup_scope);


DART_EXPORT Dart_Handle sample_extension_Init(Dart_Handle parent_library) {
  if (Dart_IsError(parent_library)) {
    return parent_library;
  }

  Dart_Handle result_code =
      Dart_SetNativeResolver(parent_library, ResolveName, NULL);
  if (Dart_IsError(result_code)) {
    return result_code;
  }

  return Dart_Null();
}


Dart_Handle HandleError(Dart_Handle handle) {
  if (Dart_IsError(handle)) {
    Dart_PropagateError(handle);
  }
  return handle;
}

char **readBarcode(char *pszFileName, int *out_len) {
	char **values = NULL;

	__int64 llFormat = (OneD | QR_CODE | PDF417 | DATAMATRIX);

	char pszBuffer[512] = { 0 };
	int iMaxCount = MAX_BARCODE_AMOUNT;
	int iIndex = 0;
	ReaderOptions ro = { 0 };
	int iRet = -1;
	char * pszTemp = NULL;
	unsigned __int64 ullTimeBegin = 0;
	unsigned __int64 ullTimeEnd = 0;
	size_t iLen = 0;
	FILE* fp = NULL;
	int iExitFlag = 0;

	// Set license
	CBarcodeReader reader;
	reader.InitLicense("38B9B94D8B0E2B41FDE1FB60861C28C0");

	// Read barcode
	ullTimeBegin = GetTickCount();
	ro.llBarcodeFormat = llFormat;
	ro.iMaxBarcodesNumPerPage = iMaxCount;
	reader.SetReaderOptions(ro);
	iRet = reader.DecodeFile(pszFileName);
	ullTimeEnd = GetTickCount();

	// Output barcode result
	pszTemp = (char*)malloc(4096);
	if (iRet != DBR_OK && iRet != DBRERR_LICENSE_EXPIRED && iRet != DBRERR_QR_LICENSE_INVALID &&
		iRet != DBRERR_1D_LICENSE_INVALID && iRet != DBRERR_PDF417_LICENSE_INVALID && iRet != DBRERR_DATAMATRIX_LICENSE_INVALID)
	{
		sprintf_s(pszTemp, 4096, "Failed to read barcode: %s\r\n", DBR_GetErrorString(iRet));
		printf(pszTemp);
		free(pszTemp);
		return NULL;
	}

	pBarcodeResultArray paryResult = NULL;
	reader.GetBarcodes(&paryResult);

	if (paryResult->iBarcodeCount == 0)
	{
		sprintf_s(pszTemp, 4096, "No barcode found. Total time spent: %.3f seconds.\r\n", ((float)(ullTimeEnd - ullTimeBegin) / 1000));
		printf(pszTemp);
		free(pszTemp);
		reader.FreeBarcodeResults(&paryResult);
		return NULL;
	}

	sprintf_s(pszTemp, 4096, "Total barcode(s) found: %d. Total time spent: %.3f seconds\r\n\r\n", paryResult->iBarcodeCount, ((float)(ullTimeEnd - ullTimeBegin) / 1000));
	//printf(pszTemp);

	values = (char **)malloc((paryResult->iBarcodeCount + 1) * sizeof(char *));

	for (iIndex = 0; iIndex < paryResult->iBarcodeCount; iIndex++)
	{
		char* pszValue = (char*)calloc(paryResult->ppBarcodes[iIndex]->iBarcodeDataLength + 1, sizeof(char));
		memcpy(pszValue, paryResult->ppBarcodes[iIndex]->pBarcodeData, paryResult->ppBarcodes[iIndex]->iBarcodeDataLength);
		values[iIndex] = pszValue;
	}

	free(pszTemp);
	reader.FreeBarcodeResults(&paryResult);
	values[iIndex] = NULL;
	*out_len = iIndex;

	return values;
}

void freeString(char** values) {
	char * pszValue = NULL;
	char **pStart = values;
	while ((pszValue = *pStart) != NULL) {
		free(pszValue);
		pStart += 1;
	}

	free(values);
}

void wrappedBarcodeReader(Dart_Port dest_port_id,
	Dart_CObject* message) {
	Dart_Port reply_port_id = ILLEGAL_PORT;
	if (message->type == Dart_CObject_kArray &&
		2 == message->value.as_array.length) {
		// Use .as_array and .as_int32 to access the data in the Dart_CObject.
		Dart_CObject* param0 = message->value.as_array.values[0];
		Dart_CObject* param1 = message->value.as_array.values[1];

		if (param0->type == Dart_CObject_kString &&
			param1->type == Dart_CObject_kSendPort) {
			char * pszFileName = param0->value.as_string;
			reply_port_id = param1->value.as_send_port.id;

			int length = 0;
			char **values = readBarcode(pszFileName, &length);
			char **pStart = values;
			if (values != NULL) {
				Dart_CObject* results[MAX_BARCODE_AMOUNT];
				int index = 0;
				char * pszValue = NULL;
				while ((pszValue = *pStart) != NULL) {
					Dart_CObject* value = new Dart_CObject();
					value->type = Dart_CObject_kString;
					value->value.as_string = pszValue;
					results[index] = value;

					++pStart;
					++index;
				}

				Dart_CObject message;
				message.type = Dart_CObject_kArray;
				message.value.as_array.length = length;
				message.value.as_array.values = results;
				Dart_PostCObject(reply_port_id, &message);
				freeString(values);
				return;
			}
		}
	}
	Dart_CObject result;
	result.type = Dart_CObject_kNull;
	Dart_PostCObject(reply_port_id, &result);
}

void barcodeReaderServicePort(Dart_NativeArguments arguments) {
	Dart_EnterScope();
	Dart_SetReturnValue(arguments, Dart_Null());
	Dart_Port service_port =
		Dart_NewNativePort("BarcodeReaderService", wrappedBarcodeReader, true);
	if (service_port != ILLEGAL_PORT) {
		Dart_Handle send_port = HandleError(Dart_NewSendPort(service_port));
		Dart_SetReturnValue(arguments, send_port);
	}
	Dart_ExitScope();
}


struct FunctionLookup {
  const char* name;
  Dart_NativeFunction function;
};


FunctionLookup function_list[] = {
	{"BarcodeReader_ServicePort", barcodeReaderServicePort},
    {NULL, NULL}};

Dart_NativeFunction ResolveName(Dart_Handle name,
                                int argc,
                                bool* auto_setup_scope) {
  if (!Dart_IsString(name)) {
    return NULL;
  }
  Dart_NativeFunction result = NULL;
  if (auto_setup_scope == NULL) {
    return NULL;
  }

  Dart_EnterScope();
  const char* cname;
  HandleError(Dart_StringToCString(name, &cname));

  for (int i=0; function_list[i].name != NULL; ++i) {
    if (strcmp(function_list[i].name, cname) == 0) {
      *auto_setup_scope = true;
      result = function_list[i].function;
      break;
    }
  }

  if (result != NULL) {
    Dart_ExitScope();
    return result;
  }

  Dart_ExitScope();
  return result;
}
