	.file	"lucine.msd"
	.text
ilc.compiled:
$BF6C7563696E652E6D7364:
	.long	.L1,.L2,LUCINE_CONTROL,0x0
.L2:
	.long	Z
	.byte	0x2,0x10
	.asciz	"IOS.0.0.1.0.128"
	.long	0x0,0x41414141
.L3:
	.asciz	"LUCINE_CONTROL"
	.data
	.globl	Z
Z:
	.byte	0x00
	.text
.L4:
	.asciz	"UNICA"
.L5:
	.asciz	"SOLA_CPU_PRESENTE"
	.data
G:
	.space	0x1,0x0
	.space	0x1,0x0
	.space	0x4,0x0
	.space	0x1,0x0
	.space	0x4,0x0
	.space	0x1,0x0
	.text
.L6:
	.long	G,LUCINE
	.long	0x0,0x41414141
.L7:
	.long	.L6,0x0
.L8:
	.long	.L4,.L5,.L7
.L9:
	.long	.L8,0x0
LUCINE_CONTROL:
	.long	.L3,.L9
	.align	4
.L1:
$EF6C7563696E652E6D7364:
