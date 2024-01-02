	.file	"scambdin.plc.labocos"
	.text
ilc.compiled:
$BF7363616D6264696E2E706C632E6C61626F636F73:
	.long	.L1,.L2,0x0
.L2:
	.long	INICDFD
	.byte	0x3,0x10
	.long	0x1,0x1
	.asciz	"INICDFD"
	.long	INICDN
	.byte	0xf,0xf
	.long	0x1,0x1
	.asciz	"INICDN"
	.long	0x0,0x41414141
	.data
	.globl	INICDFD
INICDFD:
	.space	0x4,0x0
	.globl	INICDN
INICDN:
	.space	0x4,0x0
	.text
	.align	4
.L1:
$EF7363616D6264696E2E706C632E6C61626F636F73:
