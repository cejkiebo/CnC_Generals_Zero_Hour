﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#pragma once

namespace D3D9on12
{
    namespace RegistryKeys
    {
        static const LPCTSTR g_cRegKeyPath = __TEXT("Software\\Microsoft\\Direct3D\\Direct3D9on12");

        static const LPCSTR g_cDebugLayerEnabledKey = "UseDebugLayer";
        static const LPCSTR g_cBreakOnLoadKey = "BreakOnLoad";
        static const LPCSTR g_cEnableDDISpewKey = "EnableDDISpew";
        static const LPCSTR g_cBreakOnMissingDDI = "BreakOnMissingDDI";
        static const LPCSTR g_cBreakOnInvalidArg = "BreakOnInvalidArg";
        static const LPCSTR g_cSpewDXGIFormatWarnings = "SpewDXGIFormatWarnings";
        static const LPCSTR g_cBreakOnDXGIFormatWarnings = "BreakOnDXGIFormatWarnings";
        static const LPCSTR g_cSpewPerformanceWarnings = "SpewPerformanceWarnings";
        static const LPCSTR g_cSpewConvertedShaders = "SpewConvertedShaders";
        static const LPCSTR g_cSpewD3D9Shaders = "SpewD3D9Shaders";
        static const LPCSTR g_cDebugRedPixelShader = "DebugRedPixelShader";
        static const LPCSTR g_cDebugPassThroughVertexShader = "DebugPassThroughVertexShader";
        static const LPCSTR g_cValidateShaders = "ValidateShaders";
        static const LPCSTR g_cSingleThread = "Singlethread";
        static const LPCSTR g_cPresentWithDummyWindow = "PresentWithDummyWindow";
        static const LPCSTR g_cAnythingTimes0Equals0 = "AnythingTimes0Equals0";
        static const LPCSTR g_cPSOCacheTrimLimitSize = "PSOCacheTrimLimitSize";
        static const LPCSTR g_cPSOCacheTrimLimitAge = "PSOCacheTrimLimitAge";
        static const LPCSTR g_cMaxAllocatedUploadHeapSpacePerCommandList = "MaxAllocatedUploadHeapSpacePerCommandList";
        static const LPCSTR g_cMaxSRVHeapSize = "MaxSRVHeapSize";
        static const LPCSTR g_cBufferPoolTrimThreshold = "BufferPoolTrimThreshold"; // Must be in the range 5-100 to be used by the translation layer. If there is a compat shim, will take the lesser of the two values
        static const LPCSTR g_cLockDiscardOptimization = "LockDiscardOptimization";
    };

    static DWORD CheckRegistryKeyDWORD(LPCSTR key, DWORD defaultValue = 0)
    {
        DWORD keyValue = defaultValue;
        HKEY hKey = {};

        if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegistryKeys::g_cRegKeyPath, 0, KEY_READ, &hKey))
        {
            DWORD value = 0, valueSize = sizeof(DWORD);
            LONG result = RegQueryValueEx(hKey, key, NULL, NULL, (LPBYTE)&value, &valueSize);
            RegCloseKey(hKey);
            if (result == ERROR_SUCCESS)
            {
                keyValue = value;
            }
        }
        return keyValue;
    }

    static bool CheckRegistryKey(LPCSTR key, bool defaultValue = false)
    {
        bool keyEnabled = defaultValue;
        HKEY hKey = {};

        if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegistryKeys::g_cRegKeyPath, 0, KEY_READ, &hKey))
        {
            DWORD value = 0, valueSize = sizeof(DWORD);
            LONG result = RegQueryValueEx(hKey, key, NULL, NULL, (LPBYTE)&value, &valueSize);
            RegCloseKey(hKey);
            if (result == ERROR_SUCCESS)
            {
                keyEnabled = value != 0;
            } 
        }
        return keyEnabled;
    }

    //Some regkey options will still result in branches and overhead
    //so only enable them in debug mode.
    static bool CheckRegistryKeyOnDebug(LPCSTR key)
    {
#if DBG
        return CheckRegistryKey(key);
#else
        UNREFERENCED_PARAMETER(key);
        return false;
#endif
    }

    namespace RegistryConstants
    {
        /* Debug Keys */
        static const bool g_cEnableDDISpew = CheckRegistryKeyOnDebug(RegistryKeys::g_cEnableDDISpewKey);
        static const bool g_cBreakOnMissingDDI = CheckRegistryKeyOnDebug(RegistryKeys::g_cBreakOnMissingDDI);
        static const bool g_cCheckOnInvalidArg = CheckRegistryKeyOnDebug(RegistryKeys::g_cBreakOnInvalidArg);
        static const bool g_cSpewDXGIFormatWarnings = CheckRegistryKeyOnDebug(RegistryKeys::g_cSpewDXGIFormatWarnings);
        static const bool g_cAssertDXGIFormatWarnings = CheckRegistryKeyOnDebug(RegistryKeys::g_cBreakOnDXGIFormatWarnings);
        static const bool g_cSpewPerformanceWarnings = CheckRegistryKeyOnDebug(RegistryKeys::g_cSpewPerformanceWarnings);
        static const bool g_cSpewConvertedShaders = CheckRegistryKeyOnDebug(RegistryKeys::g_cSpewConvertedShaders);
        static const bool g_cSpewD3D9Shaders = CheckRegistryKeyOnDebug(RegistryKeys::g_cSpewD3D9Shaders);
        static const bool g_cDebugRedPixelShader = CheckRegistryKeyOnDebug(RegistryKeys::g_cDebugRedPixelShader);
        static const bool g_cDebugPassThroughVertexShader = CheckRegistryKeyOnDebug(RegistryKeys::g_cDebugPassThroughVertexShader);
        static const bool g_cValidateShaders = CheckRegistryKeyOnDebug(RegistryKeys::g_cValidateShaders);

        /* Keys that work on Release as well*/
        static const bool g_cAnythingTimes0Equals0 = CheckRegistryKey(RegistryKeys::g_cAnythingTimes0Equals0);
        static const bool g_cUseDebugLayer = CheckRegistryKey(RegistryKeys::g_cDebugLayerEnabledKey);
        static const bool g_cBreakOnLoad = CheckRegistryKey(RegistryKeys::g_cBreakOnLoadKey);
        static const bool g_cSingleThread = CheckRegistryKey(RegistryKeys::g_cSingleThread);  
        static const DWORD g_cPSOCacheTrimLimitSize = CheckRegistryKeyDWORD(RegistryKeys::g_cPSOCacheTrimLimitSize, MAXDWORD);
        static const DWORD g_cPSOCacheTrimLimitAge = CheckRegistryKeyDWORD(RegistryKeys::g_cPSOCacheTrimLimitAge, MAXDWORD);
        static const DWORD g_cMaxAllocatedUploadHeapSpacePerCommandList = CheckRegistryKeyDWORD(RegistryKeys::g_cMaxAllocatedUploadHeapSpacePerCommandList, MAXDWORD);
        static const DWORD g_cMaxSRVHeapSize = CheckRegistryKeyDWORD(RegistryKeys::g_cMaxSRVHeapSize, MAXDWORD);
        static const DWORD g_cBufferPoolTrimThreshold = CheckRegistryKeyDWORD(RegistryKeys::g_cBufferPoolTrimThreshold, MAXDWORD);
        static const bool g_cLockDiscardOptimization = CheckRegistryKeyDWORD(RegistryKeys::g_cLockDiscardOptimization, 1);
    };
};