VOID instruction_hook(INS ins, VOID* val)
{
    disams_inst = INS_Disassemble(ins);
    INS_InsertCall(ins, IPOINT_BEFORE,
        (AFUNPTR)fpu_mmx_filter,
        IARG_ADDRINT, INS_Address(ins),
        IARG_UINT32, INS_Extension(ins),
        IARG_UINT32, INS_Opcode(ins),        
        IARG_PTR, strdup(INS_Disassemble(ins).c_str()),
        IARG_END
    );
}

BOOL fpu_mmx_filter(
    ADDRINT ea, 
    xed_extension_enum_t extension,
    xed_iclass_enum_t opcode,
    char* insAsm
)
{
    PIN_LockClient();
    
    BOOL rnt = FALSE;
    char out_buff[0x100] = { 0 };

    if (extension != XED_EXTENSION_BASE)
    {
        IMG run_img = IMG_FindByAddress(ea);
        if (IMG_Valid(run_img))
        {
            if (!IMG_IsMainExecutable(run_img))
                goto EXIT0;
        }
        sprintf(out_buff, "EXEC 0x%08X\t{", ea);
        sprintf((char*)&out_buff[strlen(out_buff)], "%s}\n", OPCODE_StringShort(opcode).c_str());
        fprintf(fpu_log_handle, out_buff);
        fflush(fpu_log_handle);
        rnt = TRUE;
    }

EXIT0:
    PIN_UnlockClient();
    return rnt;
}
