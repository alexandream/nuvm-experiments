# Assembly File Grammar

The following is a listing of the grammar for assembly files using EBNF as
described by ISO/IEC 14977, with a few differences:
 - No spaces are allowed in nonterminal/terminal names, spaces are used to
   denote concatenation instead of the comma, instead;
 - Lower case names starting with Upper Case characters denote productions;
 - Names written in all caps denote terminals.

The whole file is denoted by the nonterminal `Module` and is as follows:

    Module = Header Program;

    Header       = Version EntryPoint [GlobalsCount];
    Version      = KW_VERSION DEC_INTEGER DEC_INTEGER DEC_INTEGER;
    EntryPoint   = KW_ENTRY_POINT DEC_INTEGER;
    GlobalsCount = KW_GLOBALS_COUNT DEC_INTEGER;

    Program = Constants CodeSegment;
    
    Constants = KW_CONSTANTS {ConstantDescriptor};
    ConstantDescriptor = KW_STRING STRING
                       | KW_DOUBLE REAL
                       | KW_CHARACTER STRING
                       | KW_PROCEDURE LABEL DEC_INTEGER
                       | KW_INT32  Integer;
    
    Integer = DEC_INTEGER
            | HEX_INTEGER;

    CodeSegment = KW_CODE {CodeElement};
    CodeElement = Instruction
                | LABEL_DEF Instruction;

    Instruction = OP_RETURN TargetL
                | OP_MOVE TargetL TargetLC
                | OP_GLOBAL_REF TargetL TargetG
                | OP_GLOBAL_SET TargetG TargetLC
                | OP_JUMP LABEL
                | OP_JUMP_IF TargetL LABEL
                | OP_EQ TargetL TargetLC TargetLC
                | OP_LT TargetL TargetLC TargetLC
                | OP_LE TargetL TargetLC TargetLC
                | OP_GT TargetL TargetLC TargetLC
                | OP_GE TargetL TargetLC TargetLC
                | OP_NOT TargetL TargetL
                | OP_OR TargetL TargetLC TargetLC
                | OP_AND TargetL TargetLC TargetLC
                | OP_ADD TargetL TargetLC TargetLC
                | OP_SUB TargetL TargetLC TargetLC
                | OP_MUL TargetL TargetLC TargetLC
                | OP_DIV TargetL TargetLC TargetLC
                | OP_LOAD_BOOL TargetL DEC_INTEGER;

    TargetL  = REGISTER_L;
    TargetG  = REGISTER_G;
    TargetLC = REGISTER_L
             | REGISTER_C;

