.data

.globl schema_begin
.globl _schema_begin
.globl schema_end
.globl _schema_end
schema_begin:
_schema_begin:
.incbin SCHEMA
schema_end:
_schema_end:

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack, "", %progbits
#endif
