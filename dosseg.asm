dosseg

.modell small

.stack 100h

.data
    X DB 8

.code
    MOV AL,X
    DIV AL,2
    CMP AH,0
    JZ LBL_IF

 LBL_IF:
    MOV BL,X
    MUL BL,X

LBL_ELSE:
    MOV BL,X
    MUL BL,X
    MUl BL,X
    DIV BL,3







