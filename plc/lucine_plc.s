	.file	"lucine.plc"
	.text
ilc.compiled:
$BF6C7563696E652E706C63:
	.long	.L1,.L2,0x0
.L2:
	.long	0x0,0x41414141
	.data
	.globl	C
C:
	.byte	0x00
	.globl	_CREATED
_CREATED:
	.long	0x4b055b40
	.globl	_VERSION
_VERSION:
	.long	.LS1
	.text
	.globl	LUCINE
LUCINE:
$FNLUCINE	=	7
$LE6$0:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%esi
	movl	8(%ebp),%esi
/LD C
$LE13$0:
.L4:
	movzbl	C,%eax
	nop
	nop
	nop
	nop
	nop
/	STN C			 
$LE14$1:
	nop
	nop
	nop
	nop
	nop
	movl	%eax,%ebx
	testl	%ebx,%ebx
	sete	%bl
	movb	%bl,C
/	
$LE15$1:
/	LD	C
$LE16$1:
.L5:
	movzbl	C,%eax
/	ST	CONTATO.CD
$LE17$1:
	movb	%al,(%esi)
/	LD	Z
$LE18$1:
.L6:
	movzbl	Z,%eax
/	ST	CONTATO.LD
$LE19$1:
	movb	%al,1(%esi)
/	LD	6
$LE20$1:
	movl	$0x6,%eax
/	ST	CONTATO.PV
$LE21$1:
	movl	%eax,2(%esi)
/	CAL	CONTATO	 
$LE22$1:
.L7:
	leal	(%esi),%ebx
	pushl	%ebx
	call	CTD
	addl	$0x4,%esp
/	LD	CONTATO.Q
$LE23$1:
.L8:
	movzbl	6(%esi),%eax
/   
$LE24$1:
/	ST	Z		 
$LE25$1:
	movb	%al,Z
/	
$LE26$1:
/ 
$LE27$1:
/ 
$LE28$1:
/
$LE29$1:
/END_PROGRAM
$LE30$1:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
$LL31$1:
.L3:
	movl	-4(%ebp),%esi
	movl	%ebp,%esp
	popl	%ebp
	ret
.LS1:
	.byte	0x31,0x2e,0x30,0x2e,0x30,0x2e,0x30,0x0
	.align	4
.L1:
$EF6C7563696E652E706C63:
