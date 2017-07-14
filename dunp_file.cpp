BOOL dump_memory(ADDRINT ea)
{
    BOOL bDump = FALSE;
    bDump = is_dumped(ea);
    if (!bDump)
    {
        OS_RETURN_CODE rnt;
        s_module mod;
        NATIVE_PID cpid = 0;
        OS_MEMORY_AT_ADDR_INFORMATION minfo = { 0 };
        CHAR szDumpFile[MAX_PATH] = { 0 };
        UINT32* base = NULL;
        UINT64 cutime = 0;

        rnt = OS_Time(&cutime);
        if (rnt.generic_err != OS_RETURN_CODE_NO_ERROR)
        {
            return FALSE;
        }

        sprintf(szDumpFile, "%s_%llx.dmp", szMoudleName.c_str(), cutime);

        rnt = OS_GetPid(&cpid);
        if (rnt.generic_err != OS_RETURN_CODE_NO_ERROR)
        {
            return FALSE;
        }

        rnt = OS_QueryMemory(cpid, (VOID *)ea, &minfo);
        if (rnt.generic_err != OS_RETURN_CODE_NO_ERROR)
        {
            return FALSE;
        }

        rnt = OS_AllocateMemory(cpid,
            OS_PAGE_PROTECTION_TYPE_WRITE | OS_PAGE_PROTECTION_TYPE_READ,
            minfo.MapSize,
            OS_MEMORY_FLAGS_PRIVATE,
            (VOID**)&base
        );
        if (rnt.generic_err != OS_RETURN_CODE_NO_ERROR)
        {
            return FALSE;
        }

        PIN_SafeCopy(base, (VOID*)ea, minfo.MapSize);
        FILE* dmp_file = fopen(szDumpFile, "wb");
        fwrite(base, 1, minfo.MapSize, dmp_file);
        fflush(dmp_file);
        fclose(dmp_file);

        rnt = OS_FreeMemory(cpid,
            (VOID*)base,
            minfo.MapSize
        );
        if (rnt.generic_err != OS_RETURN_CODE_NO_ERROR)
        {
            return FALSE;
        }

        base = NULL;
        mod.start = (ADDRINT)minfo.BaseAddress;
        mod.end = (ADDRINT)minfo.BaseAddress + minfo.MapSize;
        m_Modules[mod.start] = mod;
    }

    return TRUE;
}
